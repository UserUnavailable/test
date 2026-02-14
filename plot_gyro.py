#!/usr/bin/env python3
"""
VEX Run_gyro 陀螺仪纠偏测试数据可视化
读取 test_gyro 输出的 CSV 数据，绘制三个子图帮助调参：
  1. 航向误差 (heading_err) 随时间变化
  2. 左右电机平均转速对比
  3. 左右速差 (lr_diff) 与航向误差的叠加对比
"""

import pandas as pd
import matplotlib.pyplot as plt
import matplotlib
import sys
import os

# 支持中文显示（macOS 优先用 PingFang，否则回退）
matplotlib.rcParams['font.sans-serif'] = ['PingFang SC', 'Heiti TC', 'SimHei', 'Arial']
matplotlib.rcParams['axes.unicode_minus'] = False

def load_data(filepath):
    """读取 test_gyro 输出的 CSV，自动跳过尾部非数据行和字段数异常的行"""
    with open(filepath, 'r') as f:
        raw_lines = f.readlines()

    # 过滤空行和非数据行（如 "--- test_gyro complete ..."）
    clean_lines = [l.strip() for l in raw_lines
                   if l.strip() and not l.strip().startswith('---')]

    if not clean_lines:
        raise ValueError("CSV 文件中没有有效数据行")

    header = clean_lines[0]
    num_cols = len(header.split(','))

    # 只保留字段数与表头一致的数据行，丢弃串口传输中损坏的行
    good_lines = [header]
    skipped = 0
    for line in clean_lines[1:]:
        if len(line.split(',')) == num_cols:
            good_lines.append(line)
        else:
            skipped += 1

    if skipped:
        print(f"⚠ 跳过了 {skipped} 行字段数异常的数据")

    from io import StringIO
    csv_text = '\n'.join(good_lines)
    df = pd.read_csv(StringIO(csv_text), skipinitialspace=True)
    # 列名清洗
    df.columns = [c.strip() for c in df.columns]
    return df


def plot(df, save_path=None):
    fig, axes = plt.subplots(3, 1, figsize=(12, 9), sharex=True)
    fig.suptitle('Run_gyro 陀螺仪纠偏测试分析', fontsize=15, fontweight='bold')

    time_s = df['time_ms'] / 1000.0  # 转换为秒

    # ---------- 子图1: 航向误差 ----------
    ax1 = axes[0]
    ax1.plot(time_s, df['heading_err'], color='#e74c3c', linewidth=1.8, marker='o', markersize=3)
    ax1.axhline(y=0, color='gray', linestyle='--', linewidth=0.8, alpha=0.6)
    ax1.set_ylabel('航向误差 (°)')
    ax1.set_title('Heading Error — 目标航向与实际航向的偏差')
    ax1.fill_between(time_s, df['heading_err'], 0, alpha=0.15, color='#e74c3c')
    ax1.grid(True, alpha=0.3)

    # 标注最大偏差
    max_idx = df['heading_err'].abs().idxmax()
    max_err = df.loc[max_idx, 'heading_err']
    max_t = time_s.iloc[max_idx]
    ax1.annotate(f'最大偏差: {max_err:.2f}°',
                 xy=(max_t, max_err),
                 xytext=(max_t + 0.15, max_err + 0.3 * (-1 if max_err < 0 else 1)),
                 arrowprops=dict(arrowstyle='->', color='#e74c3c'),
                 fontsize=9, color='#e74c3c')

    # 标注终态误差
    final_err = df['heading_err'].iloc[-1]
    ax1.annotate(f'终态: {final_err:.2f}°',
                 xy=(time_s.iloc[-1], final_err),
                 xytext=(time_s.iloc[-1] - 0.3, final_err + 0.4),
                 fontsize=9, color='#888888')

    # ---------- 子图2: 左右电机转速对比 ----------
    ax2 = axes[1]
    ax2.plot(time_s, df['left_avg'], color='#2980b9', linewidth=1.5, label='左侧均速 (RPM)', marker='s', markersize=2)
    ax2.plot(time_s, df['right_avg'], color='#e67e22', linewidth=1.5, label='右侧均速 (RPM)', marker='^', markersize=2)
    ax2.axhline(y=0, color='gray', linestyle='--', linewidth=0.8, alpha=0.6)
    ax2.set_ylabel('电机转速 (RPM)')
    ax2.set_title('Left vs Right 电机平均转速')
    ax2.legend(loc='upper right', fontsize=9)
    ax2.grid(True, alpha=0.3)

    # ---------- 子图3: 左右速差 + 航向误差叠加 ----------
    ax3 = axes[2]
    ax3.bar(time_s, df['lr_diff'], width=0.04, color='#27ae60', alpha=0.5, label='左右速差 (lr_diff)')
    ax3.set_ylabel('左右速差 (RPM)', color='#27ae60')
    ax3.tick_params(axis='y', labelcolor='#27ae60')
    ax3.grid(True, alpha=0.3)

    # 右轴叠加航向误差
    ax3b = ax3.twinx()
    ax3b.plot(time_s, df['heading_err'], color='#e74c3c', linewidth=1.5, linestyle='--', label='航向误差 (°)')
    ax3b.set_ylabel('航向误差 (°)', color='#e74c3c')
    ax3b.tick_params(axis='y', labelcolor='#e74c3c')

    # 合并两个轴的图例
    lines1, labels1 = ax3.get_legend_handles_labels()
    lines2, labels2 = ax3b.get_legend_handles_labels()
    ax3.legend(lines1 + lines2, labels1 + labels2, loc='upper right', fontsize=9)

    ax3.set_xlabel('时间 (秒)')
    ax3.set_title('纠偏效果 — 左右速差 vs 航向误差')

    plt.tight_layout()

    if save_path:
        fig.savefig(save_path, dpi=150, bbox_inches='tight')
        print(f"图表已保存: {save_path}")

    plt.show()


if __name__ == '__main__':
    # 默认读取同目录的 out.csv，也可通过命令行参数指定
    default_csv = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'out.csv')
    csv_path = sys.argv[1] if len(sys.argv) > 1 else default_csv

    if not os.path.exists(csv_path):
        print(f"错误: 找不到文件 {csv_path}")
        sys.exit(1)

    df = load_data(csv_path)
    print(f"加载 {len(df)} 条数据，时间范围 0 ~ {df['time_ms'].max():.0f} ms")
    print(f"航向误差范围: {df['heading_err'].min():.2f}° ~ {df['heading_err'].max():.2f}°")

    save_png = os.path.join(os.path.dirname(csv_path), 'gyro_analysis.png')
    plot(df, save_path=save_png)
