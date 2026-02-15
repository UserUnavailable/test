#!/usr/bin/env python3
"""
VEX V5 Test Log Visualization Tool
====================================
Auto-detect test data from terminal output and generate charts.

Supported test types:
  - test_straight : Straight-line PD control log (10 columns)
  - test_turn     : PID turning log              (6 columns)
  - test_minspeed : Min drive power sweep log     (3 columns)

Usage:
  1. Default (out.csv):    python3 plot.py
  2. Specify file:         python3 plot.py data.txt
  3. From clipboard:       python3 plot.py --clipboard
  4. Watch terminal file:  python3 plot.py --watch <terminal_file>
  5. Save as PNG:          python3 plot.py --save output

Install deps:  pip3 install matplotlib pandas
"""

from __future__ import annotations
import sys
import csv
import io
import argparse
from pathlib import Path
from dataclasses import dataclass, field

# --- Dependency check ---------------------------------------------------------
_missing = []
try:
    import matplotlib
    try:
        matplotlib.use("TkAgg")
    except Exception:
        try:
            matplotlib.use("macosx")
        except Exception:
            pass
    import matplotlib.pyplot as plt
    import matplotlib.ticker as ticker
except ImportError:
    _missing.append("matplotlib")

try:
    import pandas as pd
except ImportError:
    _missing.append("pandas")

if _missing:
    sys.exit(
        f"Missing dependencies: {', '.join(_missing)}\n"
        f"Run:  pip3 install {' '.join(_missing)}\n"
        f"Or use venv:\n"
        f"  python3 -m venv .venv && source .venv/bin/activate\n"
        f"  pip install matplotlib pandas"
    )


# --- Style (Catppuccin Mocha dark theme) -------------------------------------
plt.rcParams.update({
    "figure.facecolor":  "#1e1e2e",
    "axes.facecolor":    "#1e1e2e",
    "axes.edgecolor":    "#6c7086",
    "axes.labelcolor":   "#cdd6f4",
    "axes.grid":         True,
    "grid.color":        "#313244",
    "grid.linestyle":    "--",
    "grid.alpha":        0.6,
    "xtick.color":       "#a6adc8",
    "ytick.color":       "#a6adc8",
    "text.color":        "#cdd6f4",
    "legend.facecolor":  "#313244",
    "legend.edgecolor":  "#6c7086",
    "legend.fontsize":   9,
    "font.size":         10,
    "figure.dpi":        120,
    "axes.unicode_minus": False,
})

# Colors
C_BLUE   = "#89b4fa"
C_GREEN  = "#a6e3a1"
C_RED    = "#f38ba8"
C_YELLOW = "#f9e2af"
C_MAUVE  = "#cba6f7"
C_PEACH  = "#fab387"
C_TEAL   = "#94e2d5"
C_PINK   = "#f5c2e7"
C_DIM    = "#585b70"


# --- Data structures ----------------------------------------------------------

TEST_HEADERS: dict[str, str] = {
    "test_straight_v2": "time_s,menc,move_err,last_move_error,delta_move_err,vm,dt,current_power,gyro_err,vg,turnpower,left_avg,right_avg",
    "test_straight": "time_s,menc,move_err,vm,current_power,gyro_err,vg,turnpower,left_avg,right_avg",
    "test_turn":     "time_s,gyro_err,vg,turnpower,left_avg,right_avg",
    "test_minspeed": "time_s,power,diff",
    "test_gyro_pd":  "time_s,menc,move_err,last_move_error,delta_move_err,vm,dt,current_power,gyro_err,vg,turnpower,left_avg,right_avg",
}

@dataclass
class TestBlock:
    test_type: str
    df: pd.DataFrame
    meta: str = ""


# --- Parsing ------------------------------------------------------------------

def _is_csv_data(line: str, ncols: int) -> bool:
    """Check if a line is valid numeric CSV with expected column count."""
    parts = line.split(",")
    if len(parts) != ncols:
        return False
    try:
        [float(p) for p in parts]
        return True
    except ValueError:
        return False


def parse_blocks(text: str) -> list[TestBlock]:
    """Extract all test data blocks from raw terminal text."""
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
        expected_cols = expected_header.split(",")
        ncols = len(expected_cols)
        i += 1

        # Skip blank lines
        while i < len(lines) and not lines[i].strip():
            i += 1

        # Skip CSV header if present
        if i < len(lines) and lines[i].strip() == expected_header:
            i += 1

        # Collect data rows
        csv_rows: list[str] = []
        while i < len(lines):
            row = lines[i].strip()
            if not row or row.startswith("---") or row in TEST_HEADERS:
                break
            if _is_csv_data(row, ncols):
                csv_rows.append(row)
            i += 1

        # Collect meta (--- xxx complete ---)
        meta = ""
        while i < len(lines):
            row = lines[i].strip()
            if row.startswith("---"):
                if "complete" in row:
                    meta = row
                i += 1
                continue
            break

        # Build DataFrame
        if csv_rows:
            csv_text = expected_header + "\n" + "\n".join(csv_rows)
            try:
                df = pd.read_csv(io.StringIO(csv_text))
                blocks.append(TestBlock(test_type=test_type, df=df, meta=meta))
            except Exception as e:
                print(f"  [WARN] Failed to parse {test_type}: {e}")

    return blocks


# --- Plot: test_straight ------------------------------------------------------

def plot_straight(blk: TestBlock, idx: int) -> plt.Figure:
    """
    Straight-line test — 4 subplots:
      TL) Encoder position & remaining distance
      TR) Drive power (with min-power line)
      BL) Gyro error & turn compensation
      BR) Left/Right motor speed
    """
    df, meta = blk.df, blk.meta
    t = df["time_s"]

    fig, axes = plt.subplots(2, 2, figsize=(14, 8), sharex=True)
    fig.suptitle(f"Straight-Line Test (test_straight)  #{idx + 1}",
                 fontsize=14, fontweight="bold", y=0.97)

    # -- Position & remaining distance --
    ax = axes[0, 0]
    ax.plot(t, df["menc"],     color=C_BLUE, lw=1.5, label="Encoder pos (deg)")
    ax.plot(t, df["move_err"], color=C_RED,  lw=1.2, ls="--", label="Remaining dist (deg)")
    ax.set_ylabel("Degrees")
    ax.set_title("Position Tracking")
    ax.legend(loc="center right")

    # -- Drive power --
    ax = axes[0, 1]
    ax.plot(t, df["current_power"], color=C_GREEN, lw=1.5, label="Output power (%)")
    ax.fill_between(t, 0, df["current_power"], alpha=0.12, color=C_GREEN)
    ax.axhline(20, color=C_YELLOW, ls=":", lw=0.8, label="Min power 20%")
    ax.set_ylabel("Power (%)")
    ax.set_title("Drive Power")
    ax.legend(loc="upper right")

    # -- Gyro correction --
    ax = axes[1, 0]
    ax.plot(t, df["gyro_err"],  color=C_MAUVE, lw=1.5, label="Heading error (deg)")
    ax.plot(t, df["turnpower"], color=C_PEACH, lw=1.2, ls="--", label="Turn compensation")
    ax.axhline(0, color=C_DIM, lw=0.6)
    ax.set_ylabel("Value")
    ax.set_xlabel("Time (s)")
    ax.set_title("Gyro Correction")
    ax.legend(loc="upper right")

    # -- Motor speed --
    ax = axes[1, 1]
    ax.plot(t, df["left_avg"],  color=C_BLUE, lw=1.3, label="Left avg (RPM)")
    ax.plot(t, df["right_avg"], color=C_RED,  lw=1.3, label="Right avg (RPM)")
    ax.fill_between(t, df["left_avg"], df["right_avg"], alpha=0.08, color=C_YELLOW)
    ax.set_ylabel("RPM")
    ax.set_xlabel("Time (s)")
    ax.set_title("L/R Motor Speed")
    ax.legend(loc="upper right")

    if meta:
        fig.text(0.5, 0.005, meta, ha="center", fontsize=8, color="#6c7086", style="italic")
    fig.tight_layout(rect=[0, 0.025, 1, 0.95])
    return fig


# --- Plot: test_turn ----------------------------------------------------------

def plot_turn(blk: TestBlock, idx: int) -> plt.Figure:
    """
    PID turn test — 3 subplots:
      Top) Angle error + angular velocity (dual Y)
      Mid) Turn power output
      Bot) Left/Right motor speed
    """
    df, meta = blk.df, blk.meta
    t = df["time_s"]

    fig, axes = plt.subplots(3, 1, figsize=(12, 9), sharex=True)
    fig.suptitle(f"PID Turn Test (test_turn)  #{idx + 1}",
                 fontsize=14, fontweight="bold", y=0.97)

    # -- Angle error + angular velocity --
    ax = axes[0]
    ln1 = ax.plot(t, df["gyro_err"], color=C_BLUE, lw=1.5, label="Angle error (deg)")
    ax.axhline(0, color=C_DIM, lw=0.6)
    ax.set_ylabel("Angle error (deg)")

    ax2 = ax.twinx()
    ln2 = ax2.plot(t, df["vg"], color=C_TEAL, lw=1.0, alpha=0.7, label="Angular vel vg (deg/tick)")
    ax2.set_ylabel("Angular vel (deg/tick)", color=C_TEAL)
    ax2.tick_params(axis="y", colors=C_TEAL)

    lns = ln1 + ln2
    ax.legend(lns, [l.get_label() for l in lns], loc="upper right")
    ax.set_title("Angle Error & Angular Velocity")

    # -- Turn power --
    ax = axes[1]
    ax.plot(t, df["turnpower"], color=C_PEACH, lw=1.5, label="Turn power")
    ax.fill_between(t, 0, df["turnpower"], alpha=0.12, color=C_PEACH)
    ax.axhline(0, color=C_DIM, lw=0.6)
    ax.set_ylabel("Power output")
    ax.set_title("PID Output")
    ax.legend(loc="upper right")

    # -- Motor speed --
    ax = axes[2]
    ax.plot(t, df["left_avg"],  color=C_BLUE, lw=1.3, label="Left avg (RPM)")
    ax.plot(t, df["right_avg"], color=C_RED,  lw=1.3, label="Right avg (RPM)")
    ax.fill_between(t, df["left_avg"], df["right_avg"], alpha=0.08, color=C_YELLOW)
    ax.set_ylabel("RPM")
    ax.set_xlabel("Time (s)")
    ax.set_title("L/R Motor Speed")
    ax.legend(loc="upper right")

    if meta:
        fig.text(0.5, 0.005, meta, ha="center", fontsize=8, color="#6c7086", style="italic")
    fig.tight_layout(rect=[0, 0.025, 1, 0.95])
    return fig


# --- Plot: test_minspeed ------------------------------------------------------

def plot_minspeed(blk: TestBlock, idx: int) -> plt.Figure:
    """
    Min drive power test — 2 subplots:
      Left)  Speed deviation vs time (colored by power level)
      Right) Steady-state avg deviation per power level (bar chart)
    """
    df, meta = blk.df, blk.meta
    t = df["time_s"]

    fig, axes = plt.subplots(1, 2, figsize=(14, 6))
    fig.suptitle(f"Min Drive Power Test (test_minspeed)  #{idx + 1}",
                 fontsize=14, fontweight="bold", y=0.97)

    powers = sorted(df["power"].unique())
    cmap = plt.cm.viridis
    vmin = min(powers) if powers else 0
    vmax = max(powers) if powers else 1
    norm = plt.Normalize(vmin=vmin, vmax=vmax)

    # -- Time series --
    ax = axes[0]
    for pw in powers:
        mask = df["power"] == pw
        ax.plot(t[mask], df["diff"][mask], color=cmap(norm(pw)), lw=0.8, alpha=0.85)

    sm = plt.cm.ScalarMappable(cmap=cmap, norm=norm)
    sm.set_array([])
    cbar = fig.colorbar(sm, ax=ax, pad=0.02)
    cbar.set_label("Power (%)", color="#cdd6f4")
    cbar.ax.yaxis.set_tick_params(color="#a6adc8")
    plt.setp(cbar.ax.yaxis.get_ticklabels(), color="#a6adc8")

    ax.axhline(0, color=C_YELLOW, lw=0.8, ls=":")
    ax.set_xlabel("Time (s)")
    ax.set_ylabel("Speed deviation RPM (actual - theoretical)")
    ax.set_title("Speed Deviation Time Series")

    # -- Bar chart: steady-state mean per power level --
    ax = axes[1]
    if powers:
        means: list[float] = []
        for pw in powers:
            subset = df.loc[df["power"] == pw, "diff"]
            n = len(subset)
            if n > 4:
                # Trim 20% at each end to remove start/stop transients
                trim = int(n * 0.2)
                subset = subset.iloc[trim: n - trim]
            means.append(float(subset.mean()) if len(subset) > 0 else 0.0)

        colors = [cmap(norm(pw)) for pw in powers]
        labels = [str(int(p)) for p in powers]
        bars = ax.bar(labels, means, color=colors, edgecolor="#45475a", lw=0.5)
        ax.axhline(0, color=C_YELLOW, lw=0.8, ls=":")
        ax.set_xlabel("Set Power (%)")
        ax.set_ylabel("Avg Speed Deviation RPM")
        ax.set_title("Steady-State Deviation per Power Level")

        for bar_item, val in zip(bars, means):
            y = bar_item.get_height()
            ax.text(bar_item.get_x() + bar_item.get_width() / 2, y,
                    f"{val:.1f}", ha="center",
                    va="bottom" if val >= 0 else "top",
                    fontsize=8, color="#cdd6f4")

    if meta:
        fig.text(0.5, 0.005, meta, ha="center", fontsize=8, color="#6c7086", style="italic")
    fig.tight_layout(rect=[0, 0.025, 1, 0.95])
    return fig


# --- Dispatch table -----------------------------------------------------------
_PLOT_FN = {
    "test_straight_v2": plot_straight,
    "test_straight": plot_straight,
    "test_turn":     plot_turn,
    "test_minspeed": plot_minspeed,
    "test_gyro_pd":  plot_straight,
}


# --- Input sources ------------------------------------------------------------

def _read_clipboard() -> str:
    """Read text from system clipboard (macOS / Linux)."""
    import subprocess
    for cmd in [["pbpaste"], ["xclip", "-selection", "clipboard", "-o"]]:
        try:
            r = subprocess.run(cmd, capture_output=True, text=True, timeout=3)
            if r.returncode == 0 and r.stdout.strip():
                return r.stdout
        except Exception:
            continue
    return ""


def _find_terminal_data() -> str:
    """Scan Cursor IDE terminal files for test data."""
    base = Path.home() / ".cursor" / "projects"
    if not base.exists():
        return ""

    files = sorted(base.rglob("terminals/*.txt"),
                   key=lambda p: p.stat().st_mtime, reverse=True)

    for f in files:
        try:
            content = f.read_text(errors="replace")
            if any(k in content for k in TEST_HEADERS):
                print(f"  [auto-detect] Found data: {f}")
                return content
        except Exception:
            continue
    return ""


# --- Main ---------------------------------------------------------------------

def main():
    ap = argparse.ArgumentParser(
        description="VEX V5 Test Log Visualizer — auto-detect terminal data and plot",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    ap.add_argument("file", nargs="?", default="out.csv",
                    help="Text file with terminal output (default: out.csv)")
    ap.add_argument("-c", "--clipboard", action="store_true",
                    help="Read from system clipboard")
    ap.add_argument("-w", "--watch", metavar="FILE",
                    help="Watch a terminal file and refresh on changes")
    ap.add_argument("-s", "--save", metavar="PREFIX",
                    help="Save charts as PNG (specify filename prefix)")
    args = ap.parse_args()

    # -- Read input --
    text = ""

    if args.file and not args.clipboard and not args.watch:
        p = Path(args.file)
        if not p.is_absolute() and not p.exists():
            p = Path(__file__).resolve().parent / args.file
        if not p.exists():
            sys.exit(f"Error: file not found — {p}")
        text = p.read_text(errors="replace")
        print(f"  [file] {p}  ({len(text)} bytes)")

    elif args.clipboard:
        text = _read_clipboard()
        if not text:
            sys.exit("Error: clipboard is empty")
        print(f"  [clipboard] {len(text)} bytes")

    elif args.watch:
        wp = Path(args.watch)
        if not wp.exists():
            sys.exit(f"Error: file not found — {wp}")
        print(f"  [watch] {wp}  Press Ctrl-C to quit")
        import time
        last_mt = 0.0
        try:
            while True:
                mt = wp.stat().st_mtime
                if mt > last_mt:
                    last_mt = mt
                    blks = parse_blocks(wp.read_text(errors="replace"))
                    if blks:
                        print(f"  -> Found {len(blks)} block(s), refreshing...")
                        plt.close("all")
                        for i, b in enumerate(blks):
                            _PLOT_FN[b.test_type](b, i)
                        plt.show(block=False)
                        plt.pause(0.5)
                time.sleep(1)
        except KeyboardInterrupt:
            print("\n  Watch stopped.")
        return

    else:
        print("  Paste terminal output, then press Ctrl-D:")
        try:
            text = sys.stdin.read()
        except KeyboardInterrupt:
            sys.exit("\n  Cancelled.")

    if not text.strip():
        sys.exit("Error: no data.")

    # -- Parse --
    blocks = parse_blocks(text)
    if not blocks:
        print("\n  No valid test data found. Ensure output contains one of:")
        for k in TEST_HEADERS:
            print(f"    - {k}")
        sys.exit(1)

    print(f"\n  Found {len(blocks)} test block(s):")
    for i, b in enumerate(blocks):
        rows = len(b.df)
        dur = b.df["time_s"].iloc[-1] if rows > 0 else 0
        print(f"    [{i+1}] {b.test_type:16s}  {rows:>5d} rows  {dur:.2f}s")

    # -- Plot --
    for i, b in enumerate(blocks):
        fig = _PLOT_FN[b.test_type](b, i)
        if args.save:
            fname = f"{args.save}_{b.test_type}_{i+1}.png"
            fig.savefig(fname, dpi=150, bbox_inches="tight",
                        facecolor=fig.get_facecolor(), edgecolor="none")
            print(f"  [saved] {fname}")

    print("\n  Charts ready. Close window to exit.")
    plt.show()


if __name__ == "__main__":
    main()
