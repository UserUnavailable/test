# VEX 机器人陀螺仪转向 (Turn_Gyro) 升级计划书

## 1. 现状分析 (Current State)
目前 `test` 项目中使用的 `Turn_Gyro` 函数（位于 [void.h](file:///Users/ianqian/Desktop/VEX/test/include/void.h)）主要依赖**自适应 PD 控制**和**最小功率限幅 (Min Power)** 来实现转向。

### 现有优势：
*   **自适应 P 参数**：根据误差大小动态切换 `kp`，大角度转向稳定，小角度精调有力。
*   **场地方向补偿**：通过 `Side` 和 `Start` 变量，自动适应场地初始摆放。
*   **基础防卡死机制**：通过 `pow = sgn(pow) * 14` 的最小功率保底，克服了机器人的静摩擦力。

### 存在的问题与痛点：
1.  **缺乏最短路径计算**：机器人可能会选择绕远路转向（例如从 350° 转到 10° 时，会反向旋转 340°，而不是正向旋转 20°），浪费时间并增加累积误差。
2.  **退出条件不够稳健**：目前仅依赖瞬时的速度和误差判断（`fabs(error) <= 2 && fabs(V) <= 0.2`），在高速冲过目标点时容易误判导致提前停止或反复震荡。
3.  **最小功率引起的“乒乓效应”**：硬性的最小功率 (14) 在误差极小时可能依然过大，导致机器人在目标点附近来回摆动（Ping-Pong Effect）；如果调小，又可能导致机器人卡在目标点附近无法归零（稳态误差）。

---

## 2. 升级目标 (Upgrade Goals)
结合目前主流开源模板（JAR-Template, EZ-Template, LemLib）的优秀特性，本次升级旨在实现：
1.  **更智能的路径规划**：自动选择最短转向路径。
2.  **更平滑、精准的停止**：引入积分 (I) 项消除稳态误差，并采用“时间窗口”式的稳定退出机制。
3.  **更高的可控性与兼容性**：保留原有优秀的自适应 P 和方向补偿逻辑，平滑过渡。

---

## 3. 具体改动计划 (Implementation Plan)

### 改动一：引入“最短路径”算法 (Shortest Path Logic)
*   **原理**：借鉴 JAR-Template 和 LemLib，将目标角度与当前角度的差值映射到 `[-180, 180]` 区间内。
*   **实现方式**：
    在 `void.h` 中新增一个辅助函数：
    ```cpp
    float reduce_negative_180_to_180(float angle) {
        while(angle > 180) angle -= 360;
        while(angle < -180) angle += 360;
        return angle;
    }
    ```
    在计算误差时，使用该函数：
    ```cpp
    // 旧代码：float error = target - Gyro.rotation(degrees);
    // 新代码：
    float error = reduce_negative_180_to_180(target - Gyro.rotation(degrees));
    ```

### 改动二：引入受限的积分控制 (Anti-Windup Integral)
*   **原理**：解决最小功率带来的震荡或稳态误差问题。降低最小功率的阈值，让积分项负责提供最后几度的微调动力。
*   **实现方式**：
    1. 增加 `ki` 参数（建议初始值 `0.1`）。
    2. 增加积分激活区间（例如误差 `< 15°` 时才开始积分，防止大角度转向时积分过大）。
    3. 增加过零清零逻辑（当误差越过目标点改变符号时，立刻清零积分，防止过冲）。
    4. 降低原有的 `min_power`（从 14 降至 5~8 左右）。

### 改动三：优化退出条件 (Settle Time Exit Condition)
*   **原理**：借鉴 EZ-Template 和 JAR-Template，要求机器人不仅误差要小，而且必须**在这个小误差范围内保持稳定一段时间**（例如 200ms），才算真正完成转向。
*   **实现方式**：
    引入一个计时器变量 `time_settled`。
    ```cpp
    // 当误差和速度都在容忍范围内时，累加稳定时间
    if (fabs(error) < errortolerance && fabs(V) < dtol) {
        time_settled += 10; // 假设循环周期是 10ms
    } else {
        time_settled = 0;   // 一旦偏离，重新计时
    }
    
    // 只有稳定时间达到要求（例如 200ms），或者达到绝对超时时间，才退出
    if (time_settled >= 200 || (Brain.timer(timeUnits::sec) - Time) >= timeout) {
        arrived = true;
    }
    ```

---

## 4. 代码重构预览 (Refactoring Preview)

将在 `void.h` 中创建（或替换为）新的 `Turn_Gyro_Smart` 函数：

```cpp
void Turn_Gyro_Smart(float target, float max_timeout = 3.0) {
    target = Side * target + Start; // 保留场地方向调整
    
    // PID 参数预设 (需要根据实际底盘重新微调)
    float kp = 3.2;  
    float ki = 0.1;  // 新增：积分系数
    float kd = 22.0; 
    
    float errortolerance = 2.0; 
    float dtol = 0.5; // 放宽瞬时速度要求，依赖稳定时间
    float settle_time_req = 200; // 新增：要求稳定 200ms
    float time_settled = 0;
    
    float min_power = 8; // 修改：降低最小功率，把剩下的工作交给 I
    float lim = 100;
    
    float error, lasterror = 0, V = 0, integral = 0;
    float Time = Brain.timer(timeUnits::sec);
    float timeout = fabs(reduce_negative_180_to_180(target - Gyro.rotation(degrees)) / 50.0);
    if (timeout > max_timeout || timeout != timeout) timeout = max_timeout;

    while (true) {
        // 1. 最短路径误差计算
        error = reduce_negative_180_to_180(target - Gyro.rotation(degrees));
        V = error - lasterror;
        
        // 2. 受限积分计算
        if(fabs(error) < 15) {
            integral += error;
        } else {
            integral = 0;
        }
        // 过零清零 (防过冲)
        if ((error > 0 && lasterror < 0) || (error < 0 && lasterror > 0)) integral = 0;
        // 积分限幅
        if(integral > 300) integral = 300; 
        if(integral < -300) integral = -300;

        lasterror = error;

        // 3. 动态 KP/KD (保留你原有的优秀设计，可能需要根据新的积分项微调阈值)
        kp = fabs(error) > 120 ? 2.8 : (fabs(error) > 30 ? 3.0 : 3.4);
        kd = fabs(error) > 120 ? 30  : (fabs(error) > 30 ? 24  : 20);

        // 4. 计算输出
        float pow = kp * error + ki * integral + kd * V;
        
        // 限幅与最小功率补偿
        pow = fabs(pow) > lim ? sgn(pow) * lim : pow;
        if (fabs(pow) < min_power && fabs(error) > errortolerance) {
            pow = sgn(pow) * min_power;
        }

        Turn(pow);

        // 5. 稳定退出检测 (Settling Logic)
        if (fabs(error) <= errortolerance && fabs(V) <= dtol) {
            time_settled += 10;
        } else {
            time_settled = 0;
        }

        if (time_settled >= settle_time_req || (Brain.timer(timeUnits::sec) - Time) >= timeout) {
            break;
        }

        wait(10, msec);
    }
    RunStop(brake);
}
```

## 5. 实施与测试步骤 (Testing Steps)
1.  **备份原代码**：保留原有的 `Turn_Gyro` 函数，将新函数命名为 `Turn_Gyro_Smart` 以便 A/B 测试对比。
2.  **测试最短路径**：让机器人朝向 0 度，调用 `Turn_Gyro_Smart(350)`，观察机器人是否向右转 10 度而不是向左转一整圈。
3.  **测试积分与最小功率的配合**：
    *   观察机器人在最后 2 度左右的微调阶段。
    *   如果机器人出现反复抖动，说明 `min_power` 依然偏大，或者 `ki` 过大导致积分累积太快，尝试降低这两个值。
    *   如果机器人停在 2 度外死活不动，说明 `ki` 太小或者积分限幅过紧，尝试增加 `ki`。
4.  **验证稳定时间**：观察机器人到达目标后是否能稳稳停住，并且不再轻易被外力或惯性带偏。如果感觉退出太慢，可以适当减小 `settle_time_req`（如从 200 降到 100）。