#!/usr/bin/env python3
"""
VEX V5 Interactive Web Plotter
================================
A Flask web app that auto-refreshes CSV data and renders interactive Plotly.js charts.

Usage:
  python3 app.py                  # Start on default port 5000
  python3 app.py out.csv          # Pre-load a file
  python3 app.py --port 8080      # Custom port

Open http://localhost:5000 in your browser.

Install deps:  pip3 install flask pandas
"""

from __future__ import annotations
import sys
import io
import os
import re
import json
import argparse
import threading
import webbrowser
from pathlib import Path
from dataclasses import dataclass

try:
    import pandas as pd
except ImportError:
    sys.exit("Missing: pandas\n  pip3 install pandas")

try:
    from flask import Flask, request, jsonify, Response
except ImportError:
    sys.exit("Missing: flask\n  pip3 install flask")

app = Flask(__name__)
APP_DIR = Path(__file__).resolve().parent


# ============================================================================
# Parsing logic (ported from plot.py)
# ============================================================================

TEST_HEADERS: dict[str, str] = {
    "test_straight_v2": "time_s,menc,move_err,last_move_error,delta_move_err,vm,dt,current_power,gyro_err,vg,turnpower,left_avg,right_avg",
    "test_straight":    "time_s,menc,move_err,vm,current_power,gyro_err,vg,turnpower,left_avg,right_avg",
    "test_turn":        "time_s,gyro_err,vg,turnpower,left_avg,right_avg",
    "test_minspeed":    "time_s,power,diff",
    "test_gyro_pd":     "time_s,menc,move_err,last_move_error,delta_move_err,vm,dt,current_power,gyro_err,vg,turnpower,left_avg,right_avg",
}

_RE_TOTAL_SAMPLES = re.compile(r"total\s+(\d+)\s+samples")


@dataclass
class TestBlock:
    test_type: str
    df: pd.DataFrame
    meta: str = ""


def _is_csv_data(line: str, ncols: int) -> bool:
    parts = line.split(",")
    if len(parts) != ncols:
        return False
    try:
        [float(p) for p in parts]
        return True
    except ValueError:
        return False


def _try_salvage_csv(line: str, ncols: int) -> str | None:
    parts = line.split(",")
    if len(parts) < ncols:
        return None
    candidate = parts[:ncols]
    last = candidate[-1]
    m = re.match(r"^([+-]?\d+\.?\d*)", last)
    if m:
        candidate[-1] = m.group(1)
    else:
        return None
    try:
        [float(p) for p in candidate]
        return ",".join(candidate)
    except ValueError:
        return None


def parse_blocks(text: str) -> list[TestBlock]:
    blocks: list[TestBlock] = []
    lines = text.splitlines()
    i = 0

    while i < len(lines):
        line = lines[i].strip()
        if line not in TEST_HEADERS:
            i += 1
            continue

        test_type = line
        expected_header = TEST_HEADERS[test_type]
        ncols = len(expected_header.split(","))
        i += 1

        while i < len(lines) and not lines[i].strip():
            i += 1
        if i < len(lines) and lines[i].strip() == expected_header:
            i += 1

        csv_rows: list[str] = []
        corrupted = False
        while i < len(lines):
            row = lines[i].strip()
            if not row or row.startswith("---") or row in TEST_HEADERS:
                break
            if _is_csv_data(row, ncols):
                csv_rows.append(row)
            else:
                salvaged = _try_salvage_csv(row, ncols)
                if salvaged:
                    csv_rows.append(salvaged)
                    corrupted = True
            i += 1

        meta = ""
        expected_samples = -1
        while i < len(lines):
            row = lines[i].strip()
            if row.startswith("---"):
                if "complete" in row:
                    meta = row
                m_match = _RE_TOTAL_SAMPLES.search(row)
                if m_match:
                    expected_samples = int(m_match.group(1))
                i += 1
                continue
            if "total" in row and "samples" in row:
                m_match = _RE_TOTAL_SAMPLES.search(row)
                if m_match:
                    expected_samples = int(m_match.group(1))
            break

        if expected_samples < 0 and corrupted:
            for row in lines[max(0, i - 5):i]:
                m_match = _RE_TOTAL_SAMPLES.search(row)
                if m_match:
                    expected_samples = int(m_match.group(1))
                    break

        if csv_rows:
            csv_text = expected_header + "\n" + "\n".join(csv_rows)
            try:
                df = pd.read_csv(io.StringIO(csv_text))
                blocks.append(TestBlock(test_type=test_type, df=df, meta=meta))
            except Exception:
                pass

    return blocks


def blocks_to_json(blocks: list[TestBlock]) -> list[dict]:
    result = []
    for i, blk in enumerate(blocks):
        columns = {}
        for col in blk.df.columns:
            columns[col] = blk.df[col].tolist()
        result.append({
            "trial": i + 1,
            "test_type": blk.test_type,
            "columns": columns,
            "meta": blk.meta,
            "rows": len(blk.df),
            "duration": round(float(blk.df["time_s"].iloc[-1]), 3) if len(blk.df) > 0 else 0,
        })
    return result


# ============================================================================
# API Endpoints
# ============================================================================

@app.route("/")
def index():
    default_file = app.config.get("DEFAULT_FILE", "")
    app_dir = str(APP_DIR)
    page = HTML_PAGE.replace("__DEFAULT_FILE__", json.dumps(default_file))
    page = page.replace("__APP_DIR__", json.dumps(app_dir))
    return page


@app.route("/api/data")
def api_data():
    filepath = request.args.get("path", "")
    since = request.args.get("since", "0")

    if not filepath:
        return jsonify({"error": "No file path specified"}), 400

    p = Path(filepath)
    if not p.is_absolute():
        p = APP_DIR / p

    if not p.exists():
        return jsonify({"error": f"File not found: {p}"}), 404

    try:
        mtime = p.stat().st_mtime
    except OSError as e:
        return jsonify({"error": str(e)}), 500

    try:
        if float(since) >= mtime:
            return Response(status=304)
    except (ValueError, TypeError):
        pass

    text = p.read_text(errors="replace")
    blocks = parse_blocks(text)
    data = blocks_to_json(blocks)

    return jsonify({"mtime": mtime, "path": str(p), "blocks": data})


@app.route("/api/append", methods=["POST"])
def api_append():
    body = request.get_json(force=True)
    filepath = body.get("path", "")
    text = body.get("text", "")

    if not filepath or not text:
        return jsonify({"error": "path and text are required"}), 400

    p = Path(filepath)
    if not p.is_absolute():
        p = APP_DIR / p

    with open(p, "a", encoding="utf-8") as f:
        f.write("\n" + text.strip() + "\n")

    return jsonify({"ok": True, "path": str(p)})


@app.route("/api/browse")
def api_browse():
    dir_path = request.args.get("dir", str(APP_DIR))

    p = Path(dir_path)
    if not p.is_dir():
        return jsonify({"error": "Not a directory"}), 400

    entries = []
    try:
        for item in sorted(p.iterdir(), key=lambda x: (x.is_file(), x.name.lower())):
            if item.name.startswith("."):
                continue
            entry = {"name": item.name, "path": str(item)}
            if item.is_dir():
                entry["type"] = "dir"
            else:
                entry["type"] = "file"
                try:
                    entry["size"] = item.stat().st_size
                except OSError:
                    entry["size"] = 0
            entries.append(entry)
    except PermissionError:
        return jsonify({"error": "Permission denied"}), 403

    return jsonify({"dir": str(p), "parent": str(p.parent), "entries": entries})


# ============================================================================
# HTML Page
# ============================================================================

HTML_PAGE = r"""<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>VEX Graph Plotter</title>
<script src="https://cdn.plot.ly/plotly-2.35.0.min.js"></script>
<style>
:root {
  --bg: #1e1e2e; --mantle: #181825; --crust: #11111b;
  --surface0: #313244; --surface1: #45475a; --surface2: #585b70;
  --overlay0: #6c7086; --text: #cdd6f4; --subtext: #a6adc8;
  --blue: #89b4fa; --green: #a6e3a1; --red: #f38ba8;
  --yellow: #f9e2af; --mauve: #cba6f7; --peach: #fab387;
  --teal: #94e2d5; --pink: #f5c2e7;
}
* { margin: 0; padding: 0; box-sizing: border-box; }
body { background: var(--bg); color: var(--text); font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; min-height: 100vh; }

.top-bar {
  background: var(--mantle); padding: 14px 24px; display: flex; align-items: center; gap: 10px;
  border-bottom: 1px solid var(--surface0); position: sticky; top: 0; z-index: 100;
}
.top-bar .logo { font-weight: 700; font-size: 16px; color: var(--blue); white-space: nowrap; }
.file-input {
  flex: 1; background: var(--surface0); border: 1px solid var(--surface1); border-radius: 8px;
  color: var(--text); padding: 9px 14px; font-size: 13px; outline: none; transition: border-color 0.2s;
  font-family: 'SF Mono', 'Fira Code', monospace;
}
.file-input:focus { border-color: var(--blue); }
.btn {
  padding: 9px 18px; border-radius: 8px; border: none; cursor: pointer; font-size: 13px;
  font-weight: 600; transition: all 0.15s; white-space: nowrap;
}
.btn:active { transform: scale(0.97); }
.btn-primary { background: var(--blue); color: var(--crust); }
.btn-primary:hover { background: #74c7ec; }
.btn-green { background: var(--green); color: var(--crust); }
.btn-green:hover { background: #94d89d; }
.btn-red { background: var(--red); color: var(--crust); }
.btn-red:hover { background: #e67e96; }
.btn-ghost { background: var(--surface0); color: var(--text); border: 1px solid var(--surface1); }
.btn-ghost:hover { background: var(--surface1); }

.status-bar {
  background: var(--mantle); padding: 6px 24px; font-size: 12px; color: var(--subtext);
  display: flex; justify-content: space-between; align-items: center;
  border-bottom: 1px solid var(--surface0);
}
.paste-toggle {
  background: var(--mantle); padding: 8px 24px; border-bottom: 1px solid var(--surface0);
}
.paste-toggle summary { cursor: pointer; color: var(--subtext); font-size: 13px; user-select: none; }
.paste-toggle summary:hover { color: var(--text); }
.paste-area {
  margin-top: 10px; display: flex; gap: 10px; align-items: flex-start;
}
.paste-area textarea {
  flex: 1; background: var(--surface0); border: 1px solid var(--surface1); border-radius: 8px;
  color: var(--text); padding: 10px; font-size: 12px; font-family: 'SF Mono', 'Fira Code', monospace;
  resize: vertical; min-height: 80px; max-height: 300px; outline: none;
}
.paste-area textarea:focus { border-color: var(--blue); }

.trial-bar {
  padding: 12px 24px; display: flex; flex-wrap: wrap; gap: 8px;
  border-bottom: 1px solid var(--surface0); min-height: 20px;
}
.trial-badge {
  padding: 5px 14px; border-radius: 20px; font-size: 12px; font-weight: 600;
  cursor: pointer; transition: all 0.15s; border: 1px solid var(--surface1);
  background: var(--surface0); color: var(--subtext);
}
.trial-badge:hover { background: var(--surface1); color: var(--text); }
.trial-badge.active { background: var(--blue); color: var(--crust); border-color: var(--blue); }
.trial-badge.new { animation: flash 0.6s ease-out; }
@keyframes flash { 0% { background: var(--blue); color: var(--crust); } 100% { background: var(--surface0); } }

.main { padding: 20px 24px; max-width: 1400px; margin: 0 auto; }
.chart-card {
  background: var(--mantle); border: 1px solid var(--surface0); border-radius: 12px;
  margin-bottom: 20px; overflow: hidden; transition: border-color 0.3s;
}
.chart-card.new-card { border-color: var(--blue); }
.chart-header {
  padding: 14px 20px; display: flex; justify-content: space-between; align-items: center;
  border-bottom: 1px solid var(--surface0);
}
.chart-header h3 { font-size: 15px; font-weight: 600; }
.chart-header .meta { font-size: 12px; color: var(--subtext); }
.chart-body { padding: 8px; }

.empty-state {
  text-align: center; padding: 80px 20px; color: var(--overlay0);
}
.empty-state h2 { font-size: 20px; margin-bottom: 12px; color: var(--subtext); }
.empty-state p { font-size: 14px; line-height: 1.8; }
.empty-state code { background: var(--surface0); padding: 2px 8px; border-radius: 4px; font-size: 13px; }

/* File Browser Modal */
.modal-overlay {
  display: none; position: fixed; top: 0; left: 0; width: 100%; height: 100%;
  background: rgba(0,0,0,0.6); z-index: 200; justify-content: center; align-items: center;
}
.modal-overlay.show { display: flex; }
.modal {
  background: var(--mantle); border: 1px solid var(--surface0); border-radius: 12px;
  width: 560px; max-height: 70vh; display: flex; flex-direction: column;
}
.modal-head {
  padding: 16px 20px; display: flex; justify-content: space-between; align-items: center;
  border-bottom: 1px solid var(--surface0);
}
.modal-head h3 { font-size: 15px; }
.modal-close { background: none; border: none; color: var(--subtext); font-size: 22px; cursor: pointer; }
.modal-close:hover { color: var(--text); }
.modal-path {
  padding: 10px 20px; font-size: 12px; color: var(--subtext); background: var(--crust);
  font-family: 'SF Mono', 'Fira Code', monospace; word-break: break-all;
}
.modal-list { overflow-y: auto; flex: 1; }
.modal-item {
  padding: 10px 20px; display: flex; align-items: center; gap: 10px; cursor: pointer;
  border-bottom: 1px solid var(--surface0); font-size: 13px; transition: background 0.1s;
}
.modal-item:hover { background: var(--surface0); }
.modal-item .icon { width: 20px; text-align: center; }
.modal-item.dir .icon { color: var(--blue); }
.modal-item.file .icon { color: var(--overlay0); }
.modal-item .size { margin-left: auto; color: var(--overlay0); font-size: 11px; }
</style>
</head>
<body>

<div class="top-bar">
  <span class="logo">VEX Graph Plotter</span>
  <input class="file-input" id="filePath" type="text" placeholder="Enter CSV file path or click Browse...">
  <button class="btn btn-ghost" onclick="openBrowser()">Browse</button>
  <button class="btn btn-primary" id="refreshBtn" onclick="manualRefresh()">Refresh</button>
</div>

<div class="status-bar">
  <span id="statusText">No file loaded</span>
</div>

<details class="paste-toggle">
  <summary>Paste data directly (click to expand)</summary>
  <div class="paste-area">
    <textarea id="pasteBox" placeholder="Paste VEX terminal output here...&#10;e.g.:&#10;test_turn&#10;time_s,gyro_err,vg,turnpower,left_avg,right_avg&#10;0.000,24.96,0.000,100.0,0.0,0.0&#10;..."></textarea>
    <button class="btn btn-primary" onclick="appendPaste()">Append</button>
  </div>
</details>

<div class="trial-bar" id="trialBar"></div>

<div class="main" id="chartArea">
  <div class="empty-state" id="emptyState">
    <h2>No data loaded</h2>
    <p>
      1. Enter or browse for a CSV file path above<br>
      2. Click <strong>Start Watch</strong> to begin auto-refresh<br>
      3. Paste VEX terminal data into the CSV file, then switch back here<br>
      <br>
      Supported types: <code>test_turn</code> <code>test_straight</code> <code>test_minspeed</code> <code>test_straight_v2</code> <code>test_gyro_pd</code>
    </p>
  </div>
</div>

<!-- File Browser Modal -->
<div class="modal-overlay" id="browserOverlay" onclick="if(event.target===this)closeBrowser()">
  <div class="modal">
    <div class="modal-head">
      <h3>Select File</h3>
      <button class="modal-close" onclick="closeBrowser()">&times;</button>
    </div>
    <div class="modal-path" id="browserPath"></div>
    <div class="modal-list" id="browserList"></div>
  </div>
</div>

<script>
// ============================
// State
// ============================
const S = {
  filePath: '',
  lastMtime: 0,
  blocks: [],
  prevBlockCount: 0,
  currentTrial: 1,
};

// Catppuccin Mocha Colors
const C = {
  BLUE:'#89b4fa', GREEN:'#a6e3a1', RED:'#f38ba8', YELLOW:'#f9e2af',
  MAUVE:'#cba6f7', PEACH:'#fab387', TEAL:'#94e2d5', PINK:'#f5c2e7',
  DIM:'#585b70', BG:'#1e1e2e', MANTLE:'#181825', SURFACE:'#313244',
  SURFACE1:'#45475a', TEXT:'#cdd6f4', SUBTEXT:'#a6adc8', OVERLAY:'#6c7086',
};

// Init
const defaultFile = __DEFAULT_FILE__;
const appDir = __APP_DIR__;
if (defaultFile) {
  document.getElementById('filePath').value = defaultFile;
  S.filePath = defaultFile;
  setTimeout(() => manualRefresh(), 300);
}

// ============================
// Plotly Theme Helpers
// ============================
function axisStyle(title, extra) {
  return Object.assign({
    title: { text: title, font: { color: C.TEXT, size: 11 } },
    gridcolor: C.SURFACE, gridwidth: 1, griddash: 'dot',
    zerolinecolor: C.DIM, zerolinewidth: 1,
    linecolor: C.OVERLAY, tickfont: { color: C.SUBTEXT, size: 10 },
  }, extra || {});
}

function baseLayout(title, height) {
  return {
    title: { text: title, font: { size: 15, color: C.TEXT }, x: 0.5 },
    height: height || 600,
    paper_bgcolor: C.BG, plot_bgcolor: C.BG,
    font: { color: C.TEXT, size: 10 },
    showlegend: true,
    legend: { bgcolor: C.SURFACE, bordercolor: C.OVERLAY, borderwidth: 1, font: { color: C.TEXT, size: 10 } },
    margin: { t: 50, b: 45, l: 60, r: 60 },
    hovermode: 'x',
  };
}

const plotConfig = { responsive: true, displaylogo: false, modeBarButtonsToRemove: ['lasso2d', 'select2d'] };

// ============================
// Chart: test_turn
// ============================
function renderTurn(div, trial) {
  const t = trial.columns.time_s;
  const d1 = [0.76, 0.98], d2 = [0.40, 0.62], d3 = [0, 0.22];
  const data = [
    { x: t, y: trial.columns.gyro_err, name: 'Angle error (deg)', type: 'scatter', mode: 'lines',
      line: { color: C.BLUE, width: 1.5 }, xaxis: 'x', yaxis: 'y' },
    { x: t, y: trial.columns.vg, name: 'Angular vel (deg/tick)', type: 'scatter', mode: 'lines',
      line: { color: C.TEAL, width: 1 }, opacity: 0.7, xaxis: 'x', yaxis: 'y2' },
    { x: t, y: trial.columns.turnpower, name: 'Turn power', type: 'scatter', mode: 'lines',
      line: { color: C.PEACH, width: 1.5 }, fill: 'tozeroy', fillcolor: 'rgba(250,179,135,0.12)',
      xaxis: 'x2', yaxis: 'y3' },
    { x: t, y: trial.columns.left_avg, name: 'Left avg (RPM)', type: 'scatter', mode: 'lines',
      line: { color: C.BLUE, width: 1.3 }, xaxis: 'x3', yaxis: 'y4' },
    { x: t, y: trial.columns.right_avg, name: 'Right avg (RPM)', type: 'scatter', mode: 'lines',
      line: { color: C.RED, width: 1.3 }, fill: 'tonexty', fillcolor: 'rgba(249,226,175,0.08)',
      xaxis: 'x3', yaxis: 'y4' },
  ];
  const layout = Object.assign(baseLayout('', 920), {
    legend: { bgcolor: 'rgba(49,50,68,0.85)', bordercolor: C.OVERLAY, borderwidth: 1, font: { color: C.TEXT, size: 10 },
              orientation: 'h', x: 0.5, xanchor: 'center', y: 1.04, yanchor: 'bottom' },
    margin: { t: 70, b: 50, l: 60, r: 70 },
    xaxis:  axisStyle('Time (s)', { domain: [0,1], anchor: 'y',  matches: 'x3' }),
    xaxis2: axisStyle('Time (s)', { domain: [0,1], anchor: 'y3', matches: 'x3' }),
    xaxis3: axisStyle('Time (s)', { domain: [0,1], anchor: 'y4' }),
    yaxis:  axisStyle('Angle error (deg)', { domain: d1, anchor: 'x' }),
    yaxis2: axisStyle('Angular vel (deg/tick)', { domain: d1, overlaying: 'y', side: 'right', showgrid: false,
             titlefont: { color: C.TEAL }, tickfont: { color: C.TEAL, size: 10 } }),
    yaxis3: axisStyle('Power output', { domain: d2, anchor: 'x2' }),
    yaxis4: axisStyle('RPM',          { domain: d3, anchor: 'x3' }),
    annotations: [
      { text: '<b>Angle Error & Angular Velocity</b>', x: 0.5, y: 1.0,        xref:'paper', yref:'paper', showarrow: false, font:{color:C.TEXT,size:12} },
      { text: '<b>PID Output</b>',                     x: 0.5, y: d2[1]+0.06, xref:'paper', yref:'paper', showarrow: false, font:{color:C.TEXT,size:12} },
      { text: '<b>L/R Motor Speed</b>',                x: 0.5, y: d3[1]+0.06, xref:'paper', yref:'paper', showarrow: false, font:{color:C.TEXT,size:12} },
    ],
  });
  Plotly.newPlot(div, data, layout, plotConfig);
}

// ============================
// Chart: test_straight (also test_straight_v2, test_gyro_pd)
// ============================
function renderStraight(div, trial) {
  const t = trial.columns.time_s;
  const c = trial.columns;
  const xL = [0, 0.47], xR = [0.53, 1.0], yT = [0.60, 0.98], yB = [0, 0.38];
  const data = [
    // TL: Position
    { x: t, y: c.menc, name: 'Encoder pos (deg)', type:'scatter', mode:'lines',
      line:{color:C.BLUE,width:1.5}, xaxis:'x', yaxis:'y' },
    { x: t, y: c.move_err, name: 'Remaining dist (deg)', type:'scatter', mode:'lines',
      line:{color:C.RED,width:1.2,dash:'dash'}, xaxis:'x', yaxis:'y' },
    // TR: Drive power
    { x: t, y: c.current_power, name: 'Output power (%)', type:'scatter', mode:'lines',
      line:{color:C.GREEN,width:1.5}, fill:'tozeroy', fillcolor:'rgba(166,227,161,0.12)',
      xaxis:'x2', yaxis:'y2' },
    // BL: Gyro correction
    { x: t, y: c.gyro_err, name: 'Heading error (deg)', type:'scatter', mode:'lines',
      line:{color:C.MAUVE,width:1.5}, xaxis:'x3', yaxis:'y3' },
    { x: t, y: c.turnpower, name: 'Turn compensation', type:'scatter', mode:'lines',
      line:{color:C.PEACH,width:1.2,dash:'dash'}, xaxis:'x3', yaxis:'y3' },
    // BR: Motor speed
    { x: t, y: c.left_avg, name: 'Left avg (RPM)', type:'scatter', mode:'lines',
      line:{color:C.BLUE,width:1.3}, xaxis:'x4', yaxis:'y4' },
    { x: t, y: c.right_avg, name: 'Right avg (RPM)', type:'scatter', mode:'lines',
      line:{color:C.RED,width:1.3}, fill:'tonexty', fillcolor:'rgba(249,226,175,0.08)',
      xaxis:'x4', yaxis:'y4' },
  ];
  const layout = Object.assign(baseLayout('', 800), {
    legend: { bgcolor: 'rgba(49,50,68,0.85)', bordercolor: C.OVERLAY, borderwidth: 1, font: { color: C.TEXT, size: 10 },
              orientation: 'h', x: 0.5, xanchor: 'center', y: 1.04, yanchor: 'bottom' },
    margin: { t: 70, b: 50, l: 60, r: 60 },
    xaxis:  axisStyle('Time (s)', { domain: xL, anchor:'y',  matches:'x3' }),
    xaxis2: axisStyle('Time (s)', { domain: xR, anchor:'y2', matches:'x4' }),
    xaxis3: axisStyle('Time (s)', { domain: xL, anchor:'y3' }),
    xaxis4: axisStyle('Time (s)', { domain: xR, anchor:'y4' }),
    yaxis:  axisStyle('Degrees',   { domain: yT, anchor:'x' }),
    yaxis2: axisStyle('Power (%)', { domain: yT, anchor:'x2' }),
    yaxis3: axisStyle('Value',     { domain: yB, anchor:'x3' }),
    yaxis4: axisStyle('RPM',       { domain: yB, anchor:'x4' }),
    shapes: [
      { type:'line', xref:'x2', yref:'y2', x0: Math.min(...t), x1: Math.max(...t), y0:20, y1:20,
        line:{color:C.YELLOW,width:1,dash:'dot'} },
    ],
    annotations: [
      { text:'<b>Position Tracking</b>', x: 0.235, y: 1.0,          xref:'paper', yref:'paper', showarrow:false, font:{color:C.TEXT,size:12} },
      { text:'<b>Drive Power</b>',       x: 0.765, y: 1.0,          xref:'paper', yref:'paper', showarrow:false, font:{color:C.TEXT,size:12} },
      { text:'<b>Gyro Correction</b>',   x: 0.235, y: yB[1]+0.06,  xref:'paper', yref:'paper', showarrow:false, font:{color:C.TEXT,size:12} },
      { text:'<b>L/R Motor Speed</b>',   x: 0.765, y: yB[1]+0.06,  xref:'paper', yref:'paper', showarrow:false, font:{color:C.TEXT,size:12} },
      { text:'Min power 20%', x: Math.max(...t)*0.85, y: 22, xref:'x2', yref:'y2',
        showarrow:false, font:{color:C.YELLOW,size:9} },
    ],
  });
  Plotly.newPlot(div, data, layout, plotConfig);
}

// ============================
// Chart: test_minspeed
// ============================
function viridis(t) {
  t = Math.max(0, Math.min(1, t));
  const stops = [[0,[68,1,84]],[0.25,[59,82,139]],[0.5,[33,145,140]],[0.75,[94,201,98]],[1,[253,231,37]]];
  for (let i = 0; i < stops.length - 1; i++) {
    if (t <= stops[i+1][0]) {
      const f = (t - stops[i][0]) / (stops[i+1][0] - stops[i][0]);
      const a = stops[i][1], b = stops[i+1][1];
      return `rgb(${Math.round(a[0]+f*(b[0]-a[0]))},${Math.round(a[1]+f*(b[1]-a[1]))},${Math.round(a[2]+f*(b[2]-a[2]))})`;
    }
  }
  return 'rgb(253,231,37)';
}

function renderMinspeed(div, trial) {
  const c = trial.columns;
  const powers = [...new Set(c.power)].sort((a,b) => a - b);
  const pmin = Math.min(...powers), pmax = Math.max(...powers);
  const norm = pmax > pmin ? (v => (v - pmin) / (pmax - pmin)) : (_ => 0.5);

  const data = [];
  // Left: time series by power level
  powers.forEach((pw, idx) => {
    const mask = c.power.map((v, i) => v === pw ? i : -1).filter(i => i >= 0);
    data.push({
      x: mask.map(i => c.time_s[i]), y: mask.map(i => c.diff[i]),
      name: `Power ${Math.round(pw)}%`, type:'scatter', mode:'lines',
      line: { color: viridis(norm(pw)), width: 0.8 }, opacity: 0.85,
      xaxis: 'x', yaxis: 'y', showlegend: idx < 8,
    });
  });

  // Right: bar chart of steady-state means
  const means = powers.map(pw => {
    const vals = c.power.map((v, i) => v === pw ? c.diff[i] : null).filter(v => v !== null);
    const n = vals.length;
    if (n > 4) {
      const trim = Math.floor(n * 0.2);
      const trimmed = vals.slice(trim, n - trim);
      return trimmed.reduce((a, b) => a + b, 0) / trimmed.length;
    }
    return vals.length > 0 ? vals.reduce((a, b) => a + b, 0) / vals.length : 0;
  });
  data.push({
    x: powers.map(p => String(Math.round(p))), y: means,
    type: 'bar', name: 'Avg deviation',
    marker: { color: powers.map(pw => viridis(norm(pw))), line: { color: C.SURFACE1, width: 0.5 } },
    text: means.map(v => v.toFixed(1)), textposition: 'outside', textfont: { color: C.TEXT, size: 9 },
    xaxis: 'x2', yaxis: 'y2', showlegend: false,
  });

  const layout = Object.assign(baseLayout('', 500), {
    xaxis:  axisStyle('Time (s)',       { domain: [0, 0.46], anchor: 'y' }),
    xaxis2: axisStyle('Set Power (%)',  { domain: [0.56, 1], anchor: 'y2' }),
    yaxis:  axisStyle('Speed deviation RPM', { domain: [0, 1], anchor: 'x' }),
    yaxis2: axisStyle('Avg Speed Deviation RPM', { domain: [0, 1], anchor: 'x2' }),
    annotations: [
      { text:'<b>Speed Deviation Time Series</b>', x:0.23,  y:1.04, xref:'paper', yref:'paper', showarrow:false, font:{color:C.TEXT,size:12} },
      { text:'<b>Steady-State Deviation</b>',      x:0.78,  y:1.04, xref:'paper', yref:'paper', showarrow:false, font:{color:C.TEXT,size:12} },
    ],
    shapes: [
      { type:'line', xref:'x', yref:'y', x0:Math.min(...c.time_s), x1:Math.max(...c.time_s), y0:0, y1:0,
        line:{color:C.YELLOW,width:0.8,dash:'dot'} },
      { type:'line', xref:'x2', yref:'y2', x0:-0.5, x1:powers.length-0.5, y0:0, y1:0,
        line:{color:C.YELLOW,width:0.8,dash:'dot'} },
    ],
  });
  Plotly.newPlot(div, data, layout, plotConfig);
}

// Dispatch
const RENDERERS = {
  'test_turn': renderTurn,
  'test_straight': renderStraight,
  'test_straight_v2': renderStraight,
  'test_gyro_pd': renderStraight,
  'test_minspeed': renderMinspeed,
};

// ============================
// Render Charts (single trial view)
// ============================
function renderCharts() {
  const area = document.getElementById('chartArea');
  const bar = document.getElementById('trialBar');
  const empty = document.getElementById('emptyState');

  if (S.blocks.length === 0) {
    area.innerHTML = '';
    area.appendChild(empty);
    empty.style.display = '';
    bar.innerHTML = '';
    return;
  }
  if (empty) empty.style.display = 'none';

  // Auto-select newest trial when new data arrives
  if (S.blocks.length > S.prevBlockCount) {
    S.currentTrial = S.blocks.length;
  }
  S.prevBlockCount = S.blocks.length;

  // Clamp currentTrial to valid range
  if (S.currentTrial < 1) S.currentTrial = 1;
  if (S.currentTrial > S.blocks.length) S.currentTrial = S.blocks.length;

  // Render trial tab bar
  bar.innerHTML = S.blocks.map(b =>
    `<span class="trial-badge ${b.trial === S.currentTrial ? 'active' : ''} ${b.trial > S.prevBlockCount ? 'new' : ''}" onclick="selectTrial(${b.trial})">` +
    `Trial ${b.trial} &middot; ${b.test_type.replace('test_','')}</span>`
  ).join('');

  // Render only the selected trial
  const b = S.blocks[S.currentTrial - 1];
  if (!b) return;

  const dur = b.duration ? b.duration.toFixed(2) + 's' : '';
  area.innerHTML =
    `<div class="chart-card" id="card-active">` +
    `<div class="chart-header"><h3>Trial ${b.trial} &mdash; ${b.test_type}</h3>` +
    `<span class="meta">${b.rows} samples &middot; ${dur}${b.meta ? ' &middot; ' + b.meta : ''}</span></div>` +
    `<div class="chart-body"><div id="plot-active"></div></div></div>`;

  const plotDiv = document.getElementById('plot-active');
  if (RENDERERS[b.test_type]) {
    RENDERERS[b.test_type](plotDiv, b);
  }
}

function selectTrial(n) {
  S.currentTrial = n;
  renderCharts();
}

// ============================
// Manual Refresh
// ============================
async function fetchData() {
  if (!S.filePath) return;
  try {
    const url = `/api/data?path=${encodeURIComponent(S.filePath)}&since=0`;
    const resp = await fetch(url);
    if (!resp.ok) {
      const err = await resp.json();
      updateStatus('error', err.error || 'Request failed');
      return;
    }
    const json = await resp.json();
    S.lastMtime = json.mtime;
    S.blocks = json.blocks;
    renderCharts();
    updateStatus('ok');
  } catch (e) {
    updateStatus('error', 'Connection lost');
  }
}

async function manualRefresh() {
  const pathEl = document.getElementById('filePath');
  S.filePath = pathEl.value.trim();
  if (!S.filePath) { pathEl.focus(); return; }
  const btn = document.getElementById('refreshBtn');
  btn.textContent = 'Loading...';
  btn.disabled = true;
  await fetchData();
  btn.textContent = 'Refresh';
  btn.disabled = false;
}

function updateStatus(state, msg) {
  const txt = document.getElementById('statusText');
  if (state === 'ok') {
    txt.textContent = `${S.filePath.split('/').pop()} — ${S.blocks.length} trial(s) — Viewing: Trial ${S.currentTrial}`;
  } else if (state === 'error') {
    txt.textContent = `Error: ${msg}`;
  } else {
    txt.textContent = 'No file loaded';
  }
}

// ============================
// Paste & Append
// ============================
async function appendPaste() {
  const box = document.getElementById('pasteBox');
  const text = box.value.trim();
  if (!text) return;
  const path = document.getElementById('filePath').value.trim();
  if (!path) { alert('Please set a file path first.'); return; }
  try {
    const resp = await fetch('/api/append', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ path, text }),
    });
    if (resp.ok) {
      box.value = '';
      await fetchData();
    } else {
      const err = await resp.json();
      alert('Error: ' + (err.error || 'Failed'));
    }
  } catch (e) {
    alert('Connection error');
  }
}

// ============================
// File Browser
// ============================
async function openBrowser() {
  document.getElementById('browserOverlay').classList.add('show');
  await browseTo(appDir);
}

function closeBrowser() {
  document.getElementById('browserOverlay').classList.remove('show');
}

async function browseTo(dir) {
  const list = document.getElementById('browserList');
  const pathEl = document.getElementById('browserPath');
  pathEl.textContent = dir;
  list.innerHTML = '<div style="padding:20px;color:var(--subtext)">Loading...</div>';

  try {
    const resp = await fetch(`/api/browse?dir=${encodeURIComponent(dir)}`);
    const json = await resp.json();
    if (!resp.ok) { list.innerHTML = `<div style="padding:20px;color:var(--red)">${json.error}</div>`; return; }

    let html = '';
    if (json.parent !== json.dir) {
      html += `<div class="modal-item dir" onclick="browseTo('${json.parent.replace(/'/g,"\\'")}')">` +
              `<span class="icon">\u{1F4C1}</span> ..<span class="size">Parent</span></div>`;
    }
    json.entries.forEach(e => {
      if (e.type === 'dir') {
        html += `<div class="modal-item dir" onclick="browseTo('${e.path.replace(/'/g,"\\'")}')">` +
                `<span class="icon">\u{1F4C1}</span> ${e.name}</div>`;
      } else {
        const kb = e.size > 0 ? (e.size / 1024).toFixed(1) + ' KB' : '';
        html += `<div class="modal-item file" onclick="selectFile('${e.path.replace(/'/g,"\\'")}')">` +
                `<span class="icon">\u{1F4C4}</span> ${e.name}<span class="size">${kb}</span></div>`;
      }
    });
    list.innerHTML = html || '<div style="padding:20px;color:var(--subtext)">Empty directory</div>';
  } catch (e) {
    list.innerHTML = '<div style="padding:20px;color:var(--red)">Failed to load directory</div>';
  }
}

function selectFile(path) {
  document.getElementById('filePath').value = path;
  closeBrowser();
}

// Enter key on file path
// Arrow key navigation between trials
document.addEventListener('keydown', e => {
  if (e.target.tagName === 'INPUT' || e.target.tagName === 'TEXTAREA') return;
  if (S.blocks.length === 0) return;
  if (e.key === 'ArrowLeft' && S.currentTrial > 1) {
    selectTrial(S.currentTrial - 1);
  } else if (e.key === 'ArrowRight' && S.currentTrial < S.blocks.length) {
    selectTrial(S.currentTrial + 1);
  }
});

document.getElementById('filePath').addEventListener('keydown', e => {
  if (e.key === 'Enter') {
    manualRefresh();
  }
});
</script>
</body>
</html>
"""

# ============================================================================
# Main
# ============================================================================

if __name__ == "__main__":
    ap = argparse.ArgumentParser(
        description="VEX V5 Interactive Web Plotter",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    ap.add_argument("file", nargs="?", default="",
                    help="CSV file to pre-load (optional)")
    ap.add_argument("--port", type=int, default=5000,
                    help="Server port (default: 5000)")
    ap.add_argument("--host", default="127.0.0.1",
                    help="Server host (default: 127.0.0.1)")
    ap.add_argument("--no-browser", action="store_true",
                    help="Don't auto-open the browser")
    args = ap.parse_args()

    default_file = ""
    if args.file:
        p = Path(args.file)
        if not p.is_absolute():
            p = APP_DIR / p
        default_file = str(p.resolve())

    app.config["DEFAULT_FILE"] = default_file

    url = f"http://{args.host}:{args.port}"
    print(f"\n  VEX Graph Plotter")
    print(f"  Open: {url}")
    if default_file:
        print(f"  Pre-loaded: {default_file}")
    print()

    if not args.no_browser and os.environ.get("WERKZEUG_RUN_MAIN") != "true":
        threading.Timer(1.5, lambda: webbrowser.open(url)).start()

    app.run(host=args.host, port=args.port, debug=True)
