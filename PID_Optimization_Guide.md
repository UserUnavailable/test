# VEX 直线行驶 PID 控制优化指南 (针对 Run_gyro_new)

## 1. 核心问题诊断

当前 `Run_gyro_new` 实现存在以下结构性隐患，导致调参困难且难以同时满足高速与低速场景的需求：

*   **动态参数陷阱**：尝试通过转速(RPM)动态调整 Gyro 的 KP/KD (`gyro_kp = base * avg_rpm/100`)。这导致在起步或停止的**零速状态下，航向纠正力矩为 0**，极易发生起步偏航。
*   **距离 KD 动态化**：尝试通过距离调整 KD (`move_kd = dist <= 500 ? 0.025 : ...`)。这把物理限制问题转化为数学调参问题，增加了不可预测性。
*   **缺乏物理约束**：单纯依赖 PID 计算输出，缺乏对电机加速度（打滑）和静摩擦力（低速卡死）的显式处理。

## 2. 核心架构优化原则

VEX 强队（如 EZ-Template, JAR-Template）的主流共识是：**Simple is Reliable**。
不再使用动态 PID 参数，而是采用 **"固定 PD + 辅助约束逻辑"** 的架构。

```text
[传感器数据] -> [固定参数 PD 计算] -> [应用最小功率] -> [应用减速限制] -> [应用 Slew Rate] -> [电机输出]
```

## 3. 具体改进模块

### 3.1 废除动态 PID，统一固定参数
*   **修改**：删除所有随 RPM 或距离变化的 `kp_scale`, `kd_scale` 逻辑。
*   **原因**：机器人的质量和摩擦力是恒定的。偏离 10 度所需的纠正力，在 20RPM 和 100RPM 下差别不大。起步时尤其需要坚定的 KP 来锁定航向。

### 3.2 引入最小功率 (Min Power / Feedforward)
*   **解决问题**：低速没劲、近距离走不到位、没有积分项(I)导致的稳态误差。
*   **逻辑**：如果误差依然显著（未到达终点），但 PID 算出的输出太小（无法克服齿轮箱静摩擦力），则强制输出一个能让车刚好动起来的最小电压。
*   **代码参考**：
    ```cpp
    // 方向由原始 pid_output 决定
    if (fabs(move_err) > 5 && fabs(pid_output) < min_power) {
        pid_output = sgn(pid_output) * min_power; 
    }
    ```

### 3.3 引入 Slew Rate (斜率限制)
*   **解决问题**：起步打滑导致编码器虚转、动作过于暴躁。
*   **逻辑**：限制每个循环周期（如 10ms）内电压变化的最大步进值。不让电压从 0 直接跳到 100，而是 0 -> 8 -> 16 -> 24...
*   **代码参考**：
    ```cpp
    if(target_power > current_voltage + slew_step) current_voltage += slew_step;
    else if(target_power < current_voltage - slew_step) current_voltage -= slew_step;
    else current_voltage = target_power;
    ```

### 3.4 优化退出条件 (Settle Logic)
*   **解决问题**：基于瞬间速度 `fabs(vm) < 100` 的退出条件易受物理卡顿或传感器噪声干扰，导致提前误判退出。
*   **逻辑**：采用“小误差范围 + 持续稳定时间”的双重判定。
*   **代码参考**：
    ```cpp
    if(fabs(move_err) < 10 && fabs(vm) < 50) { 
        if(!is_settling) {
            settle_start = Brain.timer(timeUnits::msec);
            is_settling = true;
        } else if(Brain.timer(timeUnits::msec) - settle_start > 150) {
            break; // 真实稳定了 150ms 才退出
        }
    } else {
        is_settling = false; // 只要出界就重置计时
    }
    ```

## 4. 为什么不需要积分项 (I) ?

在 VEX 的纯直线运动中，极不推荐使用 I 项，原因如下：
1.  **积分饱和 (Windup)**：如果车被短暂阻挡，I 会疯狂累加，一旦释放会像弹射一样严重过冲。
2.  **来回震荡**：I 的存在容易导致车子在终点线前后反复横跳。
3.  **替代方案更优**：**PD + 最小功率** 已经能完美解决“差一点推不到终点”的稳态误差问题，且响应是瞬间的，没有延迟和累积副作用。

## 5. 综合代码示例 (重构后的 Run_gyro_new)

```cpp
void Run_gyro_optimized(double enc, float g=now)
{
  g = Side * g + Start; 
  LeftRun_1.resetPosition(); RightRun_1.resetPosition();
  // ... 其他电机重置 ...

  // 1. 固定 PID 参数 (需实测微调)
  const float move_kp = 0.35;
  const float move_kd = 0.04; 
  const float gyro_kp = 7.0;  // 固定的强大航向锁定力
  const float gyro_kd = 0.8;

  // 2. 物理限制参数
  const double slew_step = 8.0;   // 加速限制 (防打滑)
  const double min_power = 24.0;  // 克服静摩擦的最小电压

  double current_voltage = 0;
  float settle_start = 0;
  bool is_settling = false;

  float last_time = Brain.timer(timeUnits::sec);
  float move_lasterror = fabs(enc);
  float gyro_lasterror = 0;
  
  float timeout = fabs(enc) / 200.0 + 1.5;
  float start_timer = Brain.timer(timeUnits::sec);

  while((Brain.timer(timeUnits::sec) - start_timer) <= timeout)
  {
    float current_time = Brain.timer(timeUnits::sec);
    float dt = current_time - last_time;
    if(dt < 0.01) { task::sleep(1); continue; } 
    last_time = current_time;

    // 获取数据与误差
    float menc = (fabs(LeftRun_1.position(deg)) + ... ) / 6.0; 
    float move_err = fabs(enc) - fabs(menc);
    float gyro_err = g - Gyro.rotation(degrees);

    float vm = (move_err - move_lasterror) / dt;
    float vg = (gyro_err - gyro_lasterror) / dt;
    
    move_lasterror = move_err;
    gyro_lasterror = gyro_err;

    // 核心 PID 计算 (单纯的数学比例)
    float move_out = move_kp * move_err + move_kd * vm;
    float turn_out = gyro_kp * gyro_err + gyro_kd * vg;

    // 约束 1：Slew Rate (平滑输出)
    if(move_out > current_voltage + slew_step) current_voltage += slew_step;
    else if(move_out < current_voltage - slew_step) current_voltage -= slew_step;
    else current_voltage = move_out;

    // 约束 2：最小功率死区补偿 (防止低速没劲)
    double final_power = current_voltage;
    if(fabs(move_err) > 5 && fabs(final_power) < min_power) {
        final_power = sgn(final_power) * min_power;
    }

    // 约束 3：稳定退出判断 (Settle)
    if(fabs(move_err) < 8 && fabs(vm) < 50) { 
        if(!is_settling) {
            settle_start = Brain.timer(timeUnits::msec);
            is_settling = true;
        } else if(Brain.timer(timeUnits::msec) - settle_start > 150) {
            break; 
        }
    } else {
        is_settling = false;
    }

    // 执行输出
    Run_Ctrl(
        (sgn(enc) * final_power) + turn_out, 
        (sgn(enc) * final_power) - turn_out
    );

    task::sleep(10);
  }
  RunStop(brake);
}
```
