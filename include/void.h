/**
 * @file void.h
 * @brief VEX机器人控制函数库
 * 
 * 本文件包含VEX机器人的所有控制函数,主要包括:
 * 1. 底盘运动控制(直行、转弯、PID控制)
 * 2. 机械结构控制(吸球、射球、分球、翼板)
 * 3. 颜色识别与自动分球系统
 * 4. 陀螺仪辅助的高精度运动控制
 * 5. 自动程序选择界面
 * 6. 屏幕显示与用户交互
 * 
 * 使用的主要算法:
 * - PID控制算法(比例-积分-微分)
 * - 陀螺仪航向锁定
 * - 编码器位置反馈
 * - 颜色传感器识别
 */

#include <thread>

///////////////////////////////////////////////////////////////////////////////
// 全局变量定义
///////////////////////////////////////////////////////////////////////////////
float motor_min_speed = 24; //电机最小驱动功率(自动阶段统一使用)

//int auto_color_ctrl=0;//自动颜色控制
extern int auto_color_ctrl=0; //自动颜色控制开关: 0-关闭, 1-开启
extern int out_count=0;       //输出计数器
int auto_color_divide=0;
extern int auto_color_divide;
int color_divide=0;
extern int color_divide;
/**
 * @brief 符号函数,返回数值的符号
 * @param d 输入的浮点数
 * @return -1(负数), 0(零), 1(正数)
 */
int sgn(double d)
{
  if (d<0) return -1;
  else if (d==0) return 0;
  else return 1;
}
/**
 * @brief 电机控制函数(电压控制模式)
 * @param motor_name 电机对象
 * @param power 功率值(-100到100), 将转换为电压(0.12倍)
 */
void m(motor motor_name,int power)
{
  motor_name.spin(fwd, 0.12*power, voltageUnits::volt);
  // motor_name.setMaxTorque(power,percentUnits::pct);
  // motor_name.spin(directionType::fwd,speed, velocityUnits::pct);
}

/**
 * @brief 电机控制函数(速度控制模式)
 * @param motor_name 电机对象
 * @param power 扭矩百分比(0-100)
 * @param speed 速度百分比(-100到100)
 */
void motorctrl(motor motor_name,int power,int speed)
{
  motor_name.setMaxTorque(power,percentUnits::pct);
  motor_name.spin(directionType::fwd,speed, velocityUnits::pct);
}

/**
 * @brief 延时函数
 * @param timers 延时时间(毫秒)
 */
void wait(int timers)
{task::sleep(timers);}

///////////////////////////////////////////////////////////////////////////////
// 底盘控制函数
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief 底盘电机控制(电压模式)
 * @param left 左侧电机功率(-100到100)
 * @param right 右侧电机功率(-100到100)
 */
void Run_Ctrl(int left,int right)
{
	m(LeftRun_1,left);
	m(LeftRun_2,left);
  m(LeftRun_3,left);
  m(RightRun_1,right);
	m(RightRun_2,right);
  m(RightRun_3,right);
}

/**
 * @brief 底盘电机控制(速度模式)
 * @param left 左侧电机速度(-100到100)
 * @param right 右侧电机速度(-100到100)
 */
void Run_V5(int left,int right)
{
	motorctrl(LeftRun_1,100,left);
	motorctrl(LeftRun_2,100,left);
  motorctrl(LeftRun_3,100,left);
  motorctrl(RightRun_1,100,right);
	motorctrl(RightRun_2,100,right);
  motorctrl(RightRun_3,100,right);
}
/**
 * @brief 左转控制(仅控制右侧电机)
 * @param spd 右侧电机速度
 */
void Left_Ctrl(int spd)
{
  // LeftRun_1.stop(coast);
  // LeftRun_2.stop(coast);
  // LeftRun_3.stop(coast);
  m(RightRun_1,spd);
	m(RightRun_2,spd);
  m(RightRun_3,spd);
}

/**
 * @brief 右转控制(仅控制左侧电机)
 * @param spd 左侧电机速度
 */
void Right_Ctrl(int spd)
{
  m(LeftRun_1,spd);
	m(LeftRun_2,spd);
  m(LeftRun_3,spd);
  // RightRun_1.stop(coast);
  // RightRun_2.stop(coast);
  // RightRun_3.stop(coast);
}

/**
 * @brief 停止所有底盘电机
 * @param brake_name 刹车模式(brake/coast/hold)
 */
void RunStop(brakeType brake_name)
{
  LeftRun_1.stop(brake_name);
  LeftRun_2.stop(brake_name);
  LeftRun_3.stop(brake_name);
  RightRun_1.stop(brake_name);
  RightRun_2.stop(brake_name);
  RightRun_3.stop(brake_name);
}

/**
 * @brief 手柄模式停止(混合刹车模式)
 * 中间电机使用brake模式,其他使用coast模式
 */
void JoyStop()
{
  LeftRun_1.stop(coast);
  LeftRun_2.stop(brake);
  LeftRun_3.stop(coast);
  RightRun_1.stop(coast);
  RightRun_2.stop(brake);
  RightRun_3.stop(coast);
}

void hold_stop(int time){
  RunStop(hold);
  wait(time);
  RunStop(coast);
}
// void Trans(int spd)
// {
// 	m(LeftRun_1,-spd);
// 	m(LeftRun_2,spd);
// 	m(RightRun_1,-spd);
// 	m(RightRun_2,spd);
// }

/**
 * @brief 直线行驶
 * @param spd 速度(-100到100, 正数前进,负数后退)
 */
void Run(int spd)
{
  Run_Ctrl(spd,spd);
}

/**
 * @brief 原地转弯
 * @param turnspd 转弯速度(正数右转,负数左转)
 */
void Turn(int turnspd)
{
 Run_Ctrl(turnspd,-turnspd);
}

/**
 * @brief 定时直线行驶
 * @param spd 速度
 * @param time 持续时间(毫秒)
 */
void Run_time(int spd,int time)
{
  Run(spd);
	task::sleep(time);
	RunStop(coast);
}
// void Claw(motor29 name,int status)
// {
//   name.setVelocity(100,percent);
//   if (status==1)
//   {name.spin(forward);}
//   if (status==-1)
//   {name.stop();}
// }

///////////////////////////////////////////////////////////////////////////////
// 机械结构控制函数
///////////////////////////////////////////////////////////////////////////////

////吸取/////
/**
 * @brief 吸球电机控制
 * @param spd 速度(-100到100)
 */
void Intake(int spd)
{
  //m(Intake_1,spd);
  motorctrl(Intake_1,100,spd);
}

/**
 * @brief 停止吸球电机
 * @param brake_name 刹车模式
 */
void IntakeStop(brakeType brake_name)
{
  Intake_1.stop(brake_name);
}

/**
 * @brief 定时吸球
 * @param spd 速度
 * @param time 持续时间(毫秒)
 */
void Intake_time(int spd,int time)
{
  Intake(spd);
	task::sleep(time);
  IntakeStop(brake);
}

/////射球///////
/**
 * @brief 射球电机控制
 * @param spd 速度(-100到100)
 */
void Shoot(int spd)
{
  //motorctrl(Shoot_1,100,spd);
  m(Shoot_1,spd);
}

/**
 * @brief 定时射球
 * @param spd 速度
 * @param time 持续时间(毫秒)
 */
void Shoot_time(int spd,int time)
{
  Shoot(spd);
	task::sleep(time);
  Shoot(0);
}

/////分球///////
/**
 * @brief 分球电机控制
 * @param spd 速度(-100到100)
 */
void Ball(int spd)
{
  //m(UpDown_1,spd);
  m(Ball_1,spd);
}

/**
 * @brief 定时分球
 * @param spd 速度
 * @param time 持续时间(毫秒)
 */
void Ball_time(int spd,int time)
{
  Ball(spd);
	task::sleep(time);
  Ball(0);
}

/**
 * @brief 翼板气动控制
 * @param LR 控制参数: 1-展开右翼, -1-展开左翼, 0-收回两翼
 */
void Wing(int LR)
{
 if (LR==1)
  {Wing_R.set(true);}
  else if (LR==-1)
  {Wing_L.set(true);}
  else 
  {
  Wing_R.set(false); 
  Wing_L.set(false);
  }
}

///////////////////////////////////////////////////////////////////////////////
// 颜色识别与自动分球系统
///////////////////////////////////////////////////////////////////////////////

/**颜色识别自动控制线程函数
 * 根据联盟颜色和传感器检测结果自动控制吸球和分球
 * Alliance: 0-手动, 1-红方, -1-蓝方
 */
void Color_Control(){
  while(true){
    if(auto_color_ctrl==1){ //自动颜色控制开启
      if (Alliance==0){ //手动模式:不判断颜色,直接吸球
        Intake(100);
        Ball(100);
      }
      else if(Alliance==1){ //红方:保留红球,排出蓝球
        if(Color_2.color()==blue){   //靠下传感器检测到蓝球
          auto_color_divide=1;
          Ball(-80);  //反转排出
          Intake(20);  //降低吸球速度
          wait(80, msec); // 防抖延时
        }
        else if(Color_2.color()==red){ //检测到红球
          auto_color_divide=0;
          Ball(100);   //正常吸入
          Intake(100);
        }
        else if(Color.color()==blue&&Color.isNearObject()){ //靠上传感器检测到蓝球
          auto_color_divide=1;
          Ball(-80);  //反转排出
          Intake(20);
          wait(80, msec); // 防抖延时
        }
        else if(Color.color()==red&&Color.isNearObject()){ //检测到红球
          auto_color_divide=0;
          Ball(100);
          Intake(100);
        }
        else{ //未检测到球
          auto_color_divide=0;
          Ball(100);
          Intake(80);
        }
      }
      else if(Alliance==-1){ //蓝方:保留蓝球,排出红球
        if(Color_2.color()==blue){ //下传感器检测到蓝球
          auto_color_divide=0;
          Ball(100);   //正常吸入
          Intake(100);
        }
        else if(Color_2.color()==red){ //检测到红球
          auto_color_divide=1;
          Ball(-80);  //反转排出
          Intake(20);  //降低吸球速度
          wait(80, msec); // 防抖延时
        }
        else if(Color.color()==blue&&Color.isNearObject()){ //上传感器检测到蓝球
          auto_color_divide=0;
          Ball(100);
          Intake(100);
        }
        else if(Color.color()==red&&Color.isNearObject()){ //检测到红球
          auto_color_divide=1;
          Ball(-80);  //反转排出
          Intake(20);
          wait(80, msec); // 防抖延时
        }
        else{ //未检测到球
          auto_color_divide=0;
          Ball(100);
          Intake(100);
        }
      }
    } else {
      auto_color_divide=0;
    }
    
    // 移除 driver_control==1 时的 break，让线程常驻用于手动阶段
    
  wait(10, msec);
  }
}
thread ColorThread=thread(Color_Control); //创建颜色控制线程

/**
 * 以下是备用的颜色分球函数(已弃用)
 * 功能:根据颜色传感器2检测结果控制射球机构
 */
/*void Color_divide(){
  while(true){
    if(Alliance==0){
      continue;
    }
    else if(Alliance==1){
      if(Color_2.color()==blue){
        Shoot(0);
        if(Up.value()){
          Up.set(false);
          wait(70,msec);
        }
        shoot_ctrl=1;
        if(intake_ctrl==0){
          Shoot(100);
          Intake(-100);
          shoot_ctrl=-1;
        }
      }
    }
    else if(Alliance==-1){
      if(Color_2.color()==red){
        Shoot(0);
        if(Up.value()){
          Up.set(false);
          wait(70,msec);
        }
        shoot_ctrl=1;
        if(intake_ctrl==0){
          Shoot(100);
          Intake(-100);
          shoot_ctrl=-1;
        }
      }
    }   
    wait(20,msec);
  }
}*/

/**
 * 以下是输出计数函数(已弃用)
 * 功能:通过距离传感器检测输出球的数量
 */
/*void Out_Count(){
  while(true){
    if(out_detect.isNearObject()){
      out_count++;
      while(out_detect.isNearObject()){
        continue;
      }
    }
    if(driver_control==1){   //在手动阶段退出节省资源
      break;
    }
    wait(10);
  }
}
thread OutThread=thread(Out_Count);*/

/**模式选择: 
 *            0-停止所有机构
 *            1-中高桥模式(全部正转)
 *            -1-低桥模式(吸球反转)
 *            2-自动分球吸球模式
 */
void Get_Ball(float spd)//0：停；1：中高桥；-1：低桥；2：吸球.     //在开启分球的情况下吐球时会导致Ball左右脑互搏（已修复）
{
  if (spd==0) //停止模式
  {
    auto_color_ctrl=0; //关闭颜色控制
    Intake(0);
    Ball(0);
    Shoot(0);
  }
  if (spd==1) //中高桥模式:全速正转
  {
    auto_color_ctrl=0;
    Intake(100);
    Ball(100);     
    Shoot(100); 
  }  
  if (spd==-1) //低桥模式:吸球反转
  {
    auto_color_ctrl=0;
    Intake(-100); 
    Ball(-100);
    Shoot(-100);
  }  
  if (spd==2) //自动吸球分球模式
  {
    auto_color_ctrl=1; //开启颜色识别
    Intake(100); 
    if(!auto_color_divide){
      Ball(100);
    }
    Shoot(-50);  //射球轮反转
  }
}

void FAuto_Get_Ball(float spd)//0：停；1：中高桥；-1：低桥；2：吸球
{
  if (spd==0) //停止模式
  {
    Intake(0);
    Ball(0);
    Shoot(0);
    auto_color_ctrl=0; //关闭颜色控制
    }
  if (spd==1) //中高桥模式:全速正转
  {
    Intake(100);
    Ball(100);
    Shoot(100); 
    auto_color_ctrl=0;
    }  
  if (spd==-1) //低桥模式:吸球反转
  {
    Intake(-100); 
    Ball(100);
    Shoot(100);
    auto_color_ctrl=0;
  }  
  if (spd==2) //吸球模式
  {
    Intake(100); 
    Ball(100);
    Shoot(-60);  //射球轮反转
    auto_color_ctrl=0; 
    }
}

///////////////////////////////////////////////////////////////////////////////
// 辅助功能函数
///////////////////////////////////////////////////////////////////////////////

/////复位//////
/**
 * @brief 复位函数(预留)
 */
void Refresh(void)
{
}

/**
 * @brief 单电机定时自动控制
 * @param motor_name 电机对象
 * @param spd 速度
 * @param times 持续时间(毫秒)
 */
void mAuto(motor motor_name,int spd,int times)
{
  m(motor_name,spd);
	task::sleep(times);
  m(motor_name,0);
}

/**
 * @brief 撞墙停止(检测电机堵转)
 * @param spd 行驶速度
 * @param timeout 超时时间(毫秒)
 * 当电机转速低于10rpm时判定为撞墙,提前停止
 */
void Wall_Stop(int spd,float timeout)
{
  float Time=Brain.timer(timeUnits::sec);
  Run(spd);
  wait(100);
  while ((Brain.timer(timeUnits::sec)-Time)<=timeout/1000)
   
  {
    if(fabs(RightRun_1.velocity(velocityUnits::rpm))<10||fabs(LeftRun_1.velocity(velocityUnits::rpm)<10))
    {break;}  //检测到堵转,退出循环
  }
  Run(0);
}
// Run_gyro内部状态,供测试日志任务读取
struct TelemetryData {
    int action;          // 1=Turn, 2=Run直线, 3=测速测试
    float target;
    float current;
    float error;
    float error_deriv;
    float dt;
    float p_out;
    float i_out;
    float d_out;
    float total_out;
    float aux_error;
    float aux_deriv;
    float aux_out;
};
TelemetryData current_telemetry = {0};
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief 陀螺仪辅助直线行驶(PD控制)
 * @param enc 目标编码器值(度)
 * @param power 基础功率(0-100)
 * @param g 目标角度
 * @param ramp 是否启用比例减速(默认为true)
 * 使用陀螺仪实时纠偏,保证直线行驶精度
 */
void Run_gyro(double enc , double power, float g, bool ramp=true)
{
  //enc=enc*3;
  g=Side*g+Start; //根据场地方向调整目标角度
  LeftRun_1.resetPosition();
  RightRun_1.resetPosition();
  
  //PID参数
  //float gyro_kp = 1;
  float gyro_kp = 7.0;   //角度比例系数
  float gyro_kd = 20;  //角度微分系数
  float ramp_kp = 0.01; //减速比例系数
  
  bool finish;//跳出标志
  float menc=0;//左右轮编码器平均值
  float vg = 0;//角速度差(微分项)
  float vm = 0;//线速度差(微分项)
  float turnpower;//转向补偿功率
  float movepower;//移动补偿功率
  float gyro_lasterror;//上一次角度误差
  float move_lasterror = 0;//上一次距离误差
  float move_err = fabs(enc) - fabs(menc);//编码器当前与目标差值
  float gyro_err = g - Gyro.rotation(degrees) ;//陀螺仪当前与目标差值

  double total_enc = fabs(enc); //总距离(度)

  gyro_lasterror = gyro_err;
  //int timeout =  enc < 300 ? 500 : enc * 1.5;
  float Timer=Brain.timer(timeUnits::sec);
  float timeout=fabs((0.1*enc)/power); //根据距离和速度计算超时时间
  
	while((Brain.timer(timeUnits::sec)-Timer)<=timeout+0.5)
  {
    //实时更新编码器和陀螺仪数据
    menc = (fabs(LeftRun_1.position(rotationUnits::deg))+ fabs(RightRun_1.position(rotationUnits::deg)))/2;
    move_err = fabs(enc) - fabs(menc);
    gyro_err = g - Gyro.rotation(degrees) ;
    vg = gyro_err - gyro_lasterror;  //计算角度变化率
    vm = move_err-move_lasterror;    //计算距离变化率
    gyro_lasterror = gyro_err;
    
    //PD控制计算转向补偿
    turnpower = gyro_kp*gyro_err + gyro_kd * vg;

    // 比例减速逻辑: 剩余1/3距离时开始减速
    double current_power = fabs(power);
    double decel_dist = total_enc / 3.0; // 减速阈值(总距离的1/3)
    
    if(ramp && fabs(move_err) < decel_dist) 
    {
       // 线性减速: current_power * (剩余距离/减速阈值)
       current_power = current_power * (fabs(move_err) / decel_dist) * ramp_kp;
    }
    if(current_power < motor_min_speed) current_power = motor_min_speed; // 最小速度限制
    
    // 恢复原有符号方向
    double final_power = sgn(power) * current_power;

    // 写入全局变量供测试日志读取
    current_telemetry.action = 2;
    current_telemetry.target = enc;
    current_telemetry.current = menc;
    current_telemetry.error = move_err;
    current_telemetry.error_deriv = vm;
    current_telemetry.dt = 0;
    current_telemetry.p_out = 0;
    current_telemetry.i_out = 0;
    current_telemetry.d_out = 0;
    current_telemetry.total_out = final_power;
    current_telemetry.aux_error = gyro_err;
    current_telemetry.aux_deriv = vg;
    current_telemetry.aux_out = turnpower;

    move_lasterror = move_err;
    //到达目标判断
    if (fabs(enc)-fabs(menc)<2 && fabs(vm) < 1)//距离误差<2度 且 速度变化<1
    {
      break;
    }
    else
    {
    //应用补偿后的功率到左右电机
    Run_Ctrl((sgn(enc)*final_power)+ turnpower,(sgn(enc)*final_power) -turnpower);
    }
  }
  RunStop(brake);
}

/**
 * @brief 陀螺仪辅助直线行驶(P控制)
 * @param enc 目标编码器值(度), 正值前进, 负值后退
 * @param g 目标角度
 * 使用陀螺仪实时纠偏,保证直线行驶精度
 * 全程使用P控制自动计算行驶功率
 */
void Run_gyro_new(double enc, float g=now)
{
  //enc=enc*3;
  g=Side*g+Start; //根据场地方向调整目标角度
  LeftRun_1.resetPosition();
  LeftRun_2.resetPosition();
  LeftRun_3.resetPosition();
  RightRun_1.resetPosition();
  RightRun_2.resetPosition();
  RightRun_3.resetPosition();
  
  //PD参数 — gyro自适应: kp = 基准值 × RPM × kp_scale, kd = 基准值 × RPM × kd_scale
  float gyro_kp_base = 7.0;     //基准kp (100 RPM下调优所得)
  float gyro_kd_base = 0.7;     //基准kd (100 RPM下调优所得)
  //float gyro_kp_scale = 0.009;  //kp的RPM自适应系数
  //float gyro_kd_scale = 0.005;  //kd的RPM自适应系数 (比kp更大,增强减速阶段阻尼)
  float move_kp = 0.35;   //距离比例系数 500:0.3 1000:0.3 1500:0.3
  float move_kd = fabs(enc) <= 500 ? 0.025 : (fabs(enc) <= 1000 ? 0.03 : 0.04); // 距离微分系数 (单位°/s, 接近目标时vm为负→减速) 500:0.03 1000:0.04 1500:0.05

  float menc=0;//左右轮编码器平均值
  float vm = 0;//线速度差
  float turnpower;//转向补偿功率
  float movepower;//移动补偿功率
  float move_err = fabs(enc) - fabs(menc);//编码器当前与目标差值
  float move_lasterror = move_err;//上一次距离误差(初始化为初始误差)
  float gyro_err = g - Gyro.rotation(degrees) ;//陀螺仪当前与目标差值
  float gyro_lasterror = gyro_err;//上一次角度误差(初始化为当前误差,避免首周期vg尖峰)

  //int timeout =  enc < 300 ? 500 : enc * 1.5;
  float Timer=Brain.timer(timeUnits::sec);
  float timeout=fabs(enc) / 200.0 + 1.0; //超时保护
  float last_time = Timer; //动态dt: 记录上一次循环时间
  
	while((Brain.timer(timeUnits::sec)-Timer)<=timeout+0.5)
  {
    //动态计算dt: 用实际经过时间而非固定值
    float current_time = Brain.timer(timeUnits::sec);
    float dt = current_time - last_time;
    if (dt < 0.002) {          //不足2ms就跳过, 防止dt过小导致vm噪声爆炸
      vex::task::sleep(1);     //让出CPU, 避免空转
      continue;
    }
    last_time = current_time;

    //实时更新编码器和陀螺仪数据 (6电机平均)
    menc = (fabs(LeftRun_1.position(rotationUnits::deg)) + fabs(LeftRun_2.position(rotationUnits::deg)) + fabs(LeftRun_3.position(rotationUnits::deg))
         + fabs(RightRun_1.position(rotationUnits::deg)) + fabs(RightRun_2.position(rotationUnits::deg)) + fabs(RightRun_3.position(rotationUnits::deg))) / 6.0;
    move_err = fabs(enc) - fabs(menc);
    gyro_err = g - Gyro.rotation(degrees) ;

    //归一化为每秒变化率(÷dt), 让kd的单位(°/s)与kp(°)量级匹配
    vm = (move_err - move_lasterror) / dt;    //距离变化率(°/s), 接近目标时为负
    
    
    //PD控制计算转向补偿 — 动态自适应kp/kd
    float avg_rpm = (fabs(LeftRun_1.velocity(rpm)) + fabs(LeftRun_2.velocity(rpm)) + fabs(LeftRun_3.velocity(rpm))
                   + fabs(RightRun_1.velocity(rpm)) + fabs(RightRun_2.velocity(rpm)) + fabs(RightRun_3.velocity(rpm))) / 6.0;
    float gyro_kp = gyro_kp_base * avg_rpm/100;
    float gyro_kd = gyro_kd_base * avg_rpm/100;
    float vg = (gyro_err - gyro_lasterror) / dt;  //角速度(°/s)
    turnpower = gyro_kp*gyro_err + gyro_kd*vg;
    gyro_lasterror = gyro_err;

    //PD控制计算行驶功率
    movepower = move_kp * move_err + move_kd * vm;
    if(movepower > 100) movepower = 100;              // 最大速度限制
    if(movepower < motor_min_speed && fabs(enc) > fabs(menc)) movepower = motor_min_speed; // 最小速度限制
    double final_power = movepower;

    // 写入全局变量供测试日志读取
    current_telemetry.action = 2;
    current_telemetry.target = enc;
    current_telemetry.current = menc;
    current_telemetry.error = move_err;
    current_telemetry.error_deriv = vm;
    current_telemetry.dt = dt;
    current_telemetry.p_out = move_kp * move_err;
    current_telemetry.i_out = 0;
    current_telemetry.d_out = move_kd * vm;
    current_telemetry.total_out = final_power;
    current_telemetry.aux_error = gyro_err;
    current_telemetry.aux_deriv = vg;
    current_telemetry.aux_out = turnpower;
    move_lasterror = move_err;
    //到达目标判断: 距离误差<2度 且 速度<100°/s (归一化后的vm单位是°/s)
    if (fabs(enc)-fabs(menc)<2 && fabs(vm) < 100)
    {
      break;
    }
    else
    {
    //应用补偿后的功率到左右电机
    Run_Ctrl((sgn(enc)*final_power)+ turnpower,(sgn(enc)*final_power) -turnpower);
    }
    vex::task::sleep(10); 
  }
  RunStop(brake);
}
/**
 * @brief 双距离传感器辅助直线行驶(陀螺仪+测距仪双重纠偏)
 * @param dis 目标距离(毫米), 当检测距离满足条件时停止
 * @param power 基础功率(0-100)
 * @param g 目标角度(陀螺仪目标值)
 * @param reverse 是否反向行驶(true:后退/远离障碍物, false:前进/靠近障碍物)
 * 
 * 功能说明:
 * 1. 停止控制: 使用两个距离传感器(Distance1, Distance2)的平均值判断是否到达目标距离.
 *    - 两个都有值: 取平均值判断
 *    - 仅一个有值: 取该有效值判断
 *    - 若反向(reverse=true): 当前距离 > dis 时停止
 *    - 若正向(reverse=false): 当前距离 < dis 时停止
 * 
 * 2. 姿态控制: 采用双重纠偏机制
 *    - 陀螺仪PD控制: 保持机器人朝向目标角度g
 *    - 测距仪差值纠偏: 当两个测距仪均有效且差值在200mm以内时,
 *      根据左右距离差(d1-d2)辅助修正航向,确保机器人平行于参照物(如墙壁)行驶
 */
void FAuto_Run_gyro(double dis , double power, float g, bool reverse=false){
  g=Side*g+Start; //根据场地方向调整目标角度
  
  //PID参数
  float gyro_kp = 2;   //角度比例系数
  float gyro_kd = 20;  //角度微分系数
  float dist_kp = 0.5; //测距仪纠偏系数
  float ramp_kp = 0.3; //减速比例系数

  float vg = 0;//角速度差(微分项)
  float turnpower;//转向补偿功率
  float gyro_lasterror;//上一次角度误差
  float gyro_err = g - Gyro.rotation(degrees) ;//陀螺仪当前与目标差值

  gyro_lasterror = gyro_err;
  float Timer=Brain.timer(timeUnits::sec);
  float timeout;
  
  // 计算初始距离用于超时判断
  double start_d1 = Distance1.objectDistance(distanceUnits::mm);
  double start_d2 = Distance2.objectDistance(distanceUnits::mm);
  double start_dist = 9999;
  
  if(start_d1!=9999 && start_d2!=9999) start_dist = (start_d1+start_d2)/2;
  else if(start_d1!=9999) start_dist = start_d1;
  else if(start_d2!=9999) start_dist = start_d2;

  double total_travel = 0; // 总行驶距离
  bool has_total_travel = false; // 是否已获取总距离

  if(start_dist != 9999){
      total_travel = fabs(start_dist - dis);
      has_total_travel = true;
  }

  if(start_dist==9999){
    timeout=fabs((0.1*2500)/power+2); //根据距离和速度计算超时时间
  }
  else{
    timeout=fabs((0.1*(start_dist-dis))/power); //根据距离和速度计算超时时间
  }

  while((Brain.timer(timeUnits::sec)-Timer)<=timeout+0.5){
    double d1 = Distance1.objectDistance(distanceUnits::mm);
    double d2 = Distance2.objectDistance(distanceUnits::mm);
    double current_dist = 9999;

    // 计算当前综合距离
    if(d1!=9999 && d2!=9999) current_dist = (d1+d2)/2;
    else if(d1!=9999) current_dist = d1;
    else if(d2!=9999) current_dist = d2;

    //检查距离传感器,若距离小于目标距离则停止
    if(current_dist != 9999){
        if(reverse){
          if(current_dist > dis){
            break;
          }
        }
        else{
          if(current_dist < dis){
            break;
          }
        }
    }
    //实时更新陀螺仪数据
    gyro_err = g - Gyro.rotation(degrees) ;
    vg = gyro_err - gyro_lasterror;  //计算角度变化率
    gyro_lasterror = gyro_err;
    
    //PD控制计算转向补偿
    turnpower = gyro_kp*gyro_err + gyro_kd * vg;

    //双测距仪辅助纠偏
    //当两个测距仪都有效且差值在一定范围内(例如200mm)
    if(d1!=9999 && d2!=9999 && fabs(d1-d2)<200){
       // 假设Distance1在左, Distance2在右
       // d1 > d2 说明左远右近(车头偏右), 需要左转(turnpower减小)
       turnpower -= (d1-d2)*dist_kp;
    }

    // 比例减速逻辑: 剩余1/3距离时开始减速
    double current_power = fabs(power);
    double dist_err = 0;
    
    if(current_dist != 9999 && total_travel > 0){
       dist_err = fabs(current_dist - dis);
       double decel_dist = total_travel / 3.0; // 减速阈值
       
       if(dist_err < decel_dist){ 
           // 线性减速
           current_power = current_power * (dist_err / decel_dist);
       }
       if(current_power < motor_min_speed) current_power = motor_min_speed; // 最小速度限制
    }
    
    // 恢复原有符号方向
    double final_power = sgn(power) * current_power;

    //应用补偿后的功率到左右电机
    Run_Ctrl(final_power+ turnpower,final_power -turnpower);
  }
  RunStop(brake);
}
/**
 * @brief 单距离传感器辅助直线行驶(陀螺仪PD控制,无测距仪纠偏)
 * @param dis 目标距离(毫米)
 * @param power 基础功率(0-100)
 * @param g 目标角度(陀螺仪目标值)
 * @param sensor_id 指定使用的测距仪(1:Distance1, 2:Distance2)
 * @param reverse 是否反向行驶(true:后退/远离, false:前进/靠近)
 */
void Dis_Run_gyro(double dis, double power, float g, int sensor_id, bool reverse=false){
  g=Side*g+Start; //根据场地方向调整目标角度
  
  //PID参数
  float gyro_kp = 2;   //角度比例系数
  float gyro_kd = 20;  //角度微分系数
  float ramp_kp = 0.3; //减速比例系数

  float vg = 0;//角速度差
  float turnpower;//转向补偿功率
  float gyro_lasterror;//上一次角度误差
  float gyro_err = g - Gyro.rotation(degrees) ;//陀螺仪当前与目标差值

  gyro_lasterror = gyro_err;
  float Timer=Brain.timer(timeUnits::sec);
  float timeout;
  
  // 计算初始距离用于超时判断
  double start_dist = 9999;
  
  if(sensor_id == 1) start_dist = Distance1.objectDistance(distanceUnits::mm);
  else if(sensor_id == 2) start_dist = Distance2.objectDistance(distanceUnits::mm);

  double total_travel = 0; // 总行驶距离
  bool has_total_travel = false; // 是否已获取总距离

  if(start_dist != 9999){
      total_travel = fabs(start_dist - dis);
      has_total_travel = true;
  }

  if(start_dist==9999){
    timeout=fabs((0.1*2500)/power+2);
  }
  else{
    timeout=fabs((0.1*(start_dist-dis))/power);
  }

  while((Brain.timer(timeUnits::sec)-Timer)<=timeout+0.5){
    double current_dist = 9999;
    
    if(sensor_id == 1) current_dist = Distance1.objectDistance(distanceUnits::mm);
    else if(sensor_id == 2) current_dist = Distance2.objectDistance(distanceUnits::mm);

    //检查距离
    if(current_dist != 9999){
        if(reverse){
          if(current_dist > dis){
            break;
          }
        }
        else{
          if(current_dist < dis){
            break;
          }
        }
    }
    //实时更新陀螺仪数据
    gyro_err = g - Gyro.rotation(degrees) ;
    vg = gyro_err - gyro_lasterror;
    gyro_lasterror = gyro_err;
    
    //PD控制计算转向补偿
    turnpower = gyro_kp*gyro_err + gyro_kd * vg;

    // 比例减速逻辑: 剩余1/3距离时开始减速
    double current_power = fabs(power);
    double dist_err = 0;
    
    if(current_dist != 9999){
       // 如果之前没获取到总距离，现在尝试获取一次
       if(!has_total_travel){
           total_travel = fabs(current_dist - dis);
           has_total_travel = true;
       }

       if(has_total_travel && total_travel > 0){
           dist_err = fabs(current_dist - dis);
           double decel_dist = total_travel / 3.0; // 减速阈值
           
           if(dist_err < decel_dist){ 
               // 线性减速
               current_power = current_power * (dist_err / decel_dist);
           }
           if(current_power < motor_min_speed) current_power = motor_min_speed; // 最小速度限制
       }
    }
    
    // 恢复原有符号方向
    double final_power = sgn(power) * current_power;

    //应用补偿后的功率到左右电机(无测距仪纠偏)
    Run_Ctrl(final_power+ turnpower,final_power -turnpower);
  }
  RunStop(brake);
}
///////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
/**
 * @brief 将角度归一化到[-180, 180]区间，用于最短路径计算
 * @param angle 输入角度
 * @return 归一化后的角度
 */
float reduce_negative_180_to_180(float angle) {
    while(angle > 180) angle -= 360;
    while(angle < -180) angle += 360;
    return angle;
}

/**
 * @brief 陀螺仪PID转向控制
 * @param target 目标角度
 * 使用自适应PID算法,根据误差大小动态调整参数
 * 大角度转向使用较小kp,小角度精调使用较大kp
 */
void Turn_Gyro(float target)
{
   now=target;
   target=Side*target+Start; //根据场地方向调整目标角度
   float error = reduce_negative_180_to_180(target - Gyro.rotation(degrees)); //最短路径误差计算
   
   //PD参数(kp/kd在循环内根据误差动态调整)
   float kp, kd;
   float dtol = 0.5;  //停止速度阈值 (根据计划调整为0.5配合稳定时间)
   float errortolerance = 2; //角度误差容忍度
   float lim =100;    //功率限幅
   
   float lasterror;   //上一次角度误差
   float V= 0;        //角速度(微分项)
   
   float Time=Brain.timer(timeUnits::sec);
   float timeout=fabs(error/50); //根据角度计算超时时间
   if (timeout>3 || timeout!=timeout) timeout=3; // 上限3秒,避免占满自动阶段; NaN时也用3
   float pow;         //实际输出功率
   
   float time_settled = 0; //新增：稳定计时器
   float settle_time_req = 200; //新增：稳定时间要求(ms)

   lasterror = error;
   
   while (true)
   {
    error = reduce_negative_180_to_180(target - Gyro.rotation(degrees)); //最短路径误差计算
    V = error - lasterror; //计算角速度(微分)
    lasterror = error;

  // 自适应kp/kd：调整以减少过冲和振荡
  // 小角度(<=30): 降低kp，增大kd以增加阻尼
  // 中角度(30-90): 保持适中
  // 大角度(>90): 稍微增加kp以保证速度，增大kd以提前减速
  kp = fabs(error) > 90 ? 3.0 : (fabs(error) > 30 ? 2.8 : 2.5);
  kd = fabs(error) > 90 ? 35.0 : (fabs(error) > 30 ? 28.0 : 25.0);
    
    //PD计算输出功率
   pow = kp * error + kd * V;
   pow = fabs(pow) > lim ? sgn(pow) * lim : pow; //功率限幅
   
   // 最低功率保底：只在误差较大时提供，误差很小时允许0功率，依靠动能和I/D项（如果有的话）自然停止，防止ping-pong
   if (fabs(pow) < 12 && fabs(error) > 1.5) {
       pow = sgn(pow) * 12; // 稍微降低最低功率从15到12
   } else if (fabs(error) <= 1.5 && fabs(pow) < 5) {
       pow = 0; // 误差极小时，如果计算功率很小，直接给0，依靠稳定时间退出
   }

    // 写入全局变量供测试日志读取
    current_telemetry.action = 1;
    current_telemetry.target = target;
    current_telemetry.current = Gyro.rotation(degrees);
    current_telemetry.error = error;
    current_telemetry.error_deriv = V;
    current_telemetry.dt = 0;
    current_telemetry.p_out = kp * error;
    current_telemetry.i_out = 0;
    current_telemetry.d_out = kd * V;
    current_telemetry.total_out = pow;
    current_telemetry.aux_error = 0;
    current_telemetry.aux_deriv = 0;
    current_telemetry.aux_out = 0;

    Turn(pow);
    
    // 稳定退出检测 (Settling Logic)
    if (fabs(error) <= errortolerance && fabs(V) <= dtol) {
        time_settled += 10;
    } else {
        time_settled = 0;
    }

    if (time_settled >= settle_time_req || (Brain.timer(timeUnits::sec) - Time) >= timeout + 1) {
        break;
    }
    
    wait(10,msec);
  }
   RunStop(brake);

}

/**
 * @brief 新版陀螺仪PID转向控制 (基于 JAR-Template 算法)
 * @param target 目标角度
 * 引入完整的PID控制，带抗积分饱和(start_i)、过零清空、稳定的Settle退出机制
 * 解决转向过冲、最后几度卡死、高频抖动等问题
 */
void Turn_Gyro_new(float target)
{
   now = target;
   target = Side * target + Start; //根据场地方向调整目标角度
   
   // ====== PID与退出条件参数 ======
   float kp = 4.2;            // 比例系数 
   float ki = 10.0;           // 积分系数 (继续加大，目前1.5还是不够快)
   float kd = 0.25;           // 微分系数 (稍微加大阻尼，压制过冲)
   float start_i = 15.0;      // 开始累加积分的误差阈值(度)
   float max_voltage = 100;   // 最大输出功率
   
   float settle_error = 2.0;  // 稳定误差容忍度(度)
   float settle_time = 150;   // 在容忍度内维持的时间(ms)才能退出
   float timeout = 3000;      // 总体超时保护(ms)
   // ===================================================
   
   float accumulated_error = 0;
   float previous_error = reduce_negative_180_to_180(target - Gyro.rotation(degrees));
   
   float time_spent_settled = 0;
   float time_spent_running = 0;
   
   // 引入真实的计时器来计算 dt，解决 D 项失效问题
   float last_time = Brain.timer(timeUnits::msec);
   
   while (true)
   {
       float current_time = Brain.timer(timeUnits::msec);
       float dt = (current_time - last_time) / 1000.0; // 转换为秒
       if (dt <= 0.001) dt = 0.01; // 防止除零错误
       last_time = current_time;

       float error = reduce_negative_180_to_180(target - Gyro.rotation(degrees));
       
       // 1. 积分分离 (Integral windup prevention)
       if (fabs(error) < start_i) {
           accumulated_error += error * dt; // 积分项也应该乘以 dt
       }
       
       // 2. 过零清空积分 (Zero-crossing integral reset)
       if ((error > 0 && previous_error < 0) || (error < 0 && previous_error > 0)) {
           accumulated_error = 0;
       }
       
       // 3. 计算误差变化率 (导数)
       float error_deriv = (error - previous_error) / dt;
       
       // 4. 计算PID输出
       float p_out = kp * error;
       float i_out = ki * accumulated_error;
       float d_out = kd * error_deriv;
       float output = p_out + i_out + d_out;
       
       previous_error = error;
       
       // 5. 功率限幅
       if (fabs(output) > max_voltage) {
           output = sgn(output) * max_voltage;
       }
       
       // 写入全局变量供测试日志读取
       current_telemetry.action = 1;
       current_telemetry.target = target;
       current_telemetry.current = Gyro.rotation(degrees);
       current_telemetry.error = error;
       current_telemetry.error_deriv = error_deriv;
       current_telemetry.dt = dt;
       current_telemetry.p_out = p_out;
       current_telemetry.i_out = i_out;
       current_telemetry.d_out = d_out;
       current_telemetry.total_out = output;
       current_telemetry.aux_error = 0;
       current_telemetry.aux_deriv = 0;
       current_telemetry.aux_out = 0;

       // 6. 应用到底盘电机
       Turn(output);
       
       // 7. 稳定退出逻辑 (Settle Logic)
       if (fabs(error) < settle_error) {
           time_spent_settled += dt * 1000.0; // 换算回毫秒
       } else {
           time_spent_settled = 0;
       }
       
       time_spent_running += dt * 1000.0;
       
       // 8. 退出判断
       if (time_spent_settled >= settle_time) {
           break; 
       }
       if (time_spent_running >= timeout && timeout != 0) {
           break; 
       }
       
       wait(10, msec);
   }
   
   RunStop(brake);
}

///////////////////////////////////////////////////////////////////////////////
// Anchor 航向锁定控制（基于陀螺仪）
///////////////////////////////////////////////////////////////////////////////

// Anchor 锁定目标航向（陀螺仪角度）
extern float anchor_target_heading;
float anchor_target_heading = 0;

/**
 * @brief 航向锁定控制（基于陀螺仪的P控制）
 * @param enable 是否激活锁定
 * @description 使用P控制保持机器人航向不变，被撞转后自动补偿回原航向
 *              类似电子罗盘锁定效果
 */
void Anchor_Lock(bool enable)
{
    // P控制参数（参考Turn_Gyro和Run_gyro的参数）
    const float kp = 3.0;       // 比例系数（越大锁定越紧，但可能振荡）
    const float lim = 60;       // 功率限幅（防止电机过热）
    const float deadband = 1.0; // 死区（误差小于此值时不输出，减少抖动）
    const float min_power = 14; // 最低功率（克服摩擦，参考Turn_Gyro）
    
    if (!enable) {
        return;
    }
    
    // 计算航向误差
    float current_heading = Gyro.rotation(degrees);
    float error = anchor_target_heading - current_heading;
    
    // P控制计算输出（只控制转向，不控制平移）
    float turn_power = kp * error;
    
    // 限幅
    if (fabs(turn_power) > lim) {
        turn_power = sgn(turn_power) * lim;
    }
    
    // 死区处理
    if (fabs(error) < deadband) {
        turn_power = 0;
    }
    // 最低功率保底（误差较大时保证有足够力矩克服摩擦）
    else if (fabs(turn_power) < min_power && fabs(error) > 2) {
        turn_power = sgn(turn_power) * min_power;
    }
    
    // 输出到左右电机（差速转向）
    // 左转: 左负右正, 右转: 左正右负
    m(LeftRun_1, (int)turn_power);
    m(LeftRun_2, (int)turn_power);
    m(LeftRun_3, (int)turn_power);
    m(RightRun_1, (int)(-turn_power));
    m(RightRun_2, (int)(-turn_power));
    m(RightRun_3, (int)(-turn_power));
}

/**
 * @brief 设置Anchor锁定目标航向（记录当前陀螺仪角度）
 * @description 将当前陀螺仪角度记录为目标锁定航向
 */
void Anchor_SetTarget()
{
    anchor_target_heading = Gyro.rotation(degrees);
}

//////////////////////////////////////////////////////////////////////////
/**
 * @brief 单侧转向PD控制
 * @param target 目标角度
 * 通过控制单侧电机实现转向,转弯半径较大
 * target>0时控制左侧, target<0时控制右侧
 */
void Turn_Side(float target)
{
   now=target;
   target=Side*target+Start; //根据场地方向调整
   float error = target - Gyro.rotation(degrees) ;//与目标角度距离
   
   //PD参数
   float kp =5;    //比例系数
   float kd =40;   //微分系数
   float dtol = 0.2;  //停止速度阈值
   float errortolerance = 2; //角度误差容忍度
   float lim =100;    //功率限幅
   
   float lasterror;   //上一次角度误差
   float V= 0;        //角速度
   bool arrived;      //到达标志
   float Time=Brain.timer(timeUnits::sec);
   float timeout=fabs(error/50);
   float pow;         //输出功率
   
   lasterror = error;
   arrived = error == 0;
   
   while (!arrived)
   {
    error = target - Gyro.rotation(degrees) ;
    V = error - lasterror; //计算角速度
    lasterror = error;
    
    //提前退出判断
    if (fabs(error)<3)
    {
      if(RightRun_1.velocity(velocityUnits::rpm)<3||LeftRun_1.velocity(velocityUnits::rpm)<3){break;}
    }
    
    //到达判断
    if ((fabs(error) <= errortolerance && fabs(V) <= dtol) || (Brain.timer(timeUnits::sec)-Time)>=timeout+1)
    {arrived = true;}
    
    //PD计算
    pow = kp * error + kd * V;
    pow = fabs(pow) > lim ? sgn(pow) * lim : pow; //限幅
    
    //根据目标方向选择控制侧
    if (target>0)
    {Left_Ctrl(pow);}  //左转(控制右侧电机)
    else
    {Right_Ctrl(pow);} //右转(控制左侧电机)
    lasterror = error;
    wait(10,msec);
  }
   RunStop(brake);

}
///////////////////////////////////////
/**
 * @brief 撞墙行驶(带陀螺仪偏离检测)
 * @param spd 行驶速度
 * @param timeout 超时时间(毫秒)
 * @param err 允许的角度偏离误差
 * 当角度偏离超过err度时提前停止,防止卡死
 */
void Run_wall(int spd,float timeout,int err)
{
  float Time_1=Brain.timer(timeUnits::sec);
  float ref = Gyro.rotation(degrees);  // 以进入时的当前朝向为直线参考,不依赖上一动是否到位
  while((Brain.timer(timeUnits::sec)-Time_1<=timeout/1000))
  {
    //检测角度偏离(相对进入时的朝向)
    if (fabs(Gyro.rotation(degrees) - ref) > err)
    {
      break; //偏离过大,退出
    }
    else
    {
    Run(spd);
    //wait(10,msec);
    }
  }
  RunStop(brake);
}

/////////////////////////////////////////////////////////////////////////
/**
 * @brief 编码器控制转弯
 * @param turnspd 转弯速度
 * @param encode 目标编码器值(度)
 * 使用编码器反馈实现固定角度转弯
 */
void Turnencode(int turnspd,int encode)
{
   // Turn(3,10*Side*abs(encode)/encode);
    //task::sleep(100);
    RunStop(coast);
    LeftRun_1.resetPosition();
    RightRun_1.resetPosition();
    while(1)
	{
    //检查是否到达目标编码器值
    if (((abs(LeftRun_1.position(rotationUnits::deg))+abs(RightRun_1.position(rotationUnits::deg)))/2)<abs(encode))
    {
     Turn((Side*abs(encode)/encode)*turnspd); //根据方向和编码器正负号确定转向
    }
    else
    {
     RunStop(brake);
     break;
    }

    }
    RunStop(brake);
    // task::sleep(100);
    // RunStop(coast);
    //  task::sleep(100);
}

///////////////////////////////////////////////////////////////////////////////
/**
 * @brief 编码器控制直线行驶(速度模式)
 * @param speed 行驶速度(-100到100)
 * @param encode 目标编码器值(度)
 * 使用编码器反馈实现固定距离行驶,带超时保护
 */
void Runencode(int speed,int encode)
{
    //Brain.resetTimer();
    LeftRun_1.resetPosition();
    RightRun_1.resetPosition();

    float Timer=Brain.timer(timeUnits::sec);
    float timeout=fabs((0.1*encode)/speed); //根据距离和速度计算超时时间
    
	while((Brain.timer(timeUnits::sec)-Timer)<=timeout+1)
	{
    //检查是否到达目标编码器值
    if (((abs(LeftRun_1.position(rotationUnits::deg))+abs(RightRun_1.position(rotationUnits::deg)))/2)<abs(encode))
		{
			Run_V5(speed,speed); //继续前进
		}

	  else
		{
			break; //到达目标,退出循环
		}

	}
    RunStop(brake);

}
///////////////////////////////////////////////////////////////////////////////
// 视觉传感器跟踪函数
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief 视觉跟踪 - 将最大物体保持在屏幕中央
 * @param tolerance 中心容差(像素), 默认15像素
 * 
 * 功能说明:
 * - Vision Sensor 分辨率为 316x212 像素，中心点为 (158, 106)
 * - 使用PD控制算法根据物体X坐标偏差控制底盘转向
 * - 当物体在中心容差范围内时停止
 * - 实时在主控屏幕第1行显示转向功率
 */
void Vision_Center_Track(int tolerance=15)
{
  const int SCREEN_CENTER_X = 158; // 视觉传感器X轴中心
  const int SCREEN_CENTER_Y = 106; // 视觉传感器Y轴中心
  
  // PD控制参数
  float kp = 0.3;  // 比例系数(根据实际情况调整)
  float kd = 0.1;  // 微分系数
  float min_power = 10; // 最小转向功率
  float max_power = 50; // 最大转向功率
  
  float last_error = 0;
  
  while(true)
  {
    // 拍摄快照
    Vision.takeSnapshot(Vision__HighGoal);
    
    // 检查是否检测到物体
    if(Vision.objectCount > 0)
    {
      // 获取最大物体的X坐标
      int object_x = Vision.largestObject.centerX;
      
      // 计算偏差(正值表示物体在右边，需要右转)
      float error = object_x - SCREEN_CENTER_X;
      
      // 计算微分项
      float derivative = error - last_error;
      last_error = error;
      
      // 如果物体已经在中心容差范围内，停止
      if(fabs(error) <= tolerance)
      {
        RunStop(coast);
        
        // 在主控上显示状态
        
        // 在Brain屏幕上显示状态
        Brain.Screen.setCursor(5, 1);
        Brain.Screen.clearLine(5);
        Brain.Screen.print("Vision: Centered! X=%d", object_x);
        
        wait(100, msec);
        continue; // 继续监控，保持居中
      }
      
      // PD控制计算转向功率
      float turn_power = kp * error + kd * derivative;
      
      // 限制功率范围
      if(fabs(turn_power) < min_power)
      {
        turn_power = sgn(turn_power) * min_power;
      }
      if(fabs(turn_power) > max_power)
      {
        turn_power = sgn(turn_power) * max_power;
      }
      
      // 控制底盘转向
      // error > 0: 物体在右边，需要右转(左正右正)
      // error < 0: 物体在左边，需要左转(左负右负)
      Turn(turn_power);
      
      
      // 在Brain屏幕上显示跟踪状态
      Brain.Screen.setCursor(5, 1);
      Brain.Screen.clearLine(5);
      Brain.Screen.print("Vision: Tracking X=%d Err=%.1f Pwr=%.1f", 
                         object_x, error, turn_power);
    }
    else
    {
      // 未检测到物体，停止并等待
      RunStop(coast);
      
      // 在Brain屏幕上显示
      Brain.Screen.setCursor(5, 1);
      Brain.Screen.clearLine(5);
      Brain.Screen.print("Vision: No object detected    ");
    }
    
    wait(100, msec); // 控制循环频率
  }
}

///////////////////////////////////////////////////////////////////////////////
// 屏幕显示和用户交互
///////////////////////////////////////////////////////////////////////////////
//event Print = event();
//event AutoScreen = event();
///////////////////////////////////////////////////////////////////////////////
int Auto_temp=0; //临时自动程序编号
task  PrintTask;

/**
 * @brief 手柄屏幕信息显示任务
 * @return 0
 * 实时显示:
 *   第1行: 联盟颜色(Red/Blue/Manual)
 *   第2行: 自动程序编号
 *   第3行: 陀螺仪当前角度
 */
int Print(){
 Controller1.Screen.clearLine(1);
 Controller1.Screen.clearLine(2);
 Controller1.Screen.clearLine(3);
	//while (button==0)
  while(true){
      //显示联盟颜色
      if(Alliance==1)
      {
        Controller1.Screen.clearLine(1);
        Controller1.Screen.setCursor(1,8);
        Controller1.Screen.print ("Red"); //红方
      }
      if(Alliance==-1)
      {
        Controller1.Screen.clearLine(1);
        Controller1.Screen.setCursor(1,8);
        Controller1.Screen.print ("Blue"); //蓝方
      }
      if(Alliance==0)
      {
        Controller1.Screen.clearLine(1);
        Controller1.Screen.setCursor(1,8);
        Controller1.Screen.print ("Manual"); //手动模式
      }
      
      //显示自动程序编号
      //Controller1.Screen.clearLine(2);
      Controller1.Screen.setCursor(2,8);
      Controller1.Screen.print ("Auto=%d",Auto);
      Controller1.Screen.setCursor(2,1);
      Controller1.Screen.print ("hold=%d",auto_stop);
      // if(Auto==1)
      // {
      //     Controller1.Screen.setCursor(2,1);
      //     Controller1.Screen.print ("中桥加长桥");
      // }
      // if(Auto==2)
      // {
      //     Controller1.Screen.setCursor(2,1);
      //     Controller1.Screen.print ("低桥加长桥");//减分边冲环后联盟杆
      // }
      // if(Auto==3)
      // {
      //     Controller1.Screen.setCursor(2,1);
      //     Controller1.Screen.print ("单车Awp");
      // }
      // if(Auto==4)
      // {
      //     Controller1.Screen.setCursor(2,1);
      //     Controller1.Screen.print ("技能赛!");
      // }
      // Controller1.Screen.setCursor(2,1);
      // Controller1.Screen.print ("Roll=%5.2f",Gyro.orientation(roll, degrees));//Gyro.rotation(degrees) 
      
      //显示陀螺仪角度
      Controller1.Screen.setCursor(3,8);
      Controller1.Screen.print ("Gyro=%5.2f",Gyro.rotation(degrees));

    wait(50, msec); //刷新间隔50ms
  }
}

/**
 * @brief 自动程序和联盟颜色选择控制
 * 通过Brain屏幕触摸实现选择:
 *   第一步: 选择联盟(左侧红方,右侧蓝方)
 *   第二步: 选择自动程序(1-4)
 */
void Auto_Ctrl(){
  /*手柄按键选择方式(已注释)
  if(Controller1.ButtonLeft.pressing()){
    Side=1;
    Alliance=1;
    AutoChoose=0;
    while(Controller1.ButtonLeft.pressing()){
      wait(20,msec);
    }
  }
  else if(Controller1.ButtonRight.pressing()){
    Side=1;
    Alliance=-1;
    AutoChoose=0;
    while(Controller1.ButtonRight.pressing()){
      wait(20,msec);
    }
  }
  else if(Controller1.ButtonUp.pressing()){
    Auto++;
    if(Auto>4){
      Auto=1;
    }
    while(Controller1.ButtonUp.pressing()){
      wait(20,msec);
    }
  }
  else if(Controller1.ButtonY.pressing()){
    button=0;
    Choose=0;
    while(Controller1.ButtonY.pressing()){
      wait(20,msec);
    }
    PrintTask=task(Print);
  }*/
  
  /*显示信息到手柄(已注释)
  if(Alliance==0){
    Controller1.Screen.clearLine(1);
    Controller1.Screen.setCursor(1,1);
    Controller1.Screen.print("Not chosen");
  }
  Controller1.Screen.clearLine(2);
  Controller1.Screen.setCursor(2,1);
  Controller1.Screen.print("Auto=%d",Auto);
  Controller1.Screen.clearLine(3);
  Controller1.Screen.setCursor(3,1);
  Controller1.Screen.print("Gyro=%5.2f",Gyro.rotation(degrees));*/
  //wait(20, msec);
  
  //Brain屏幕触摸选择
  int x,y;
  
  //第一步:选择联盟颜色(仅在未选择联盟时)
  if (Alliance==0&&Brain.Screen.pressing())
  {
  x = Brain.Screen.xPosition();
  y = Brain.Screen.yPosition();
  Brain.Screen.drawCircle(x,y,50); //绘制触摸点
  while(Brain.Screen.pressing()); //等待松开
  
  if (x>260) //点击右侧 - 蓝方
    {
    Brain.Screen.clearScreen();
    Controller1.Screen.clearScreen();
    //绘制8个蓝色矩形按钮供选择自动程序(上下两排，每排4个)
    Brain.Screen.drawRectangle(10,20,100,90,color::blue);
    Brain.Screen.drawRectangle(130,20,100,90,color::blue);
    Brain.Screen.drawRectangle(250,20,100,90,color::blue);
    Brain.Screen.drawRectangle(370,20,100,90,color::blue);
    Brain.Screen.drawRectangle(10,130,100,90,color::blue);
    Brain.Screen.drawRectangle(130,130,100,90,color::blue);
    Brain.Screen.drawRectangle(250,130,100,90,color::blue);
    Brain.Screen.drawRectangle(370,130,100,90,color::blue);
    Side=1;
    Alliance=-1; //设置为蓝方
    AutoChoose=0;

   }
    else if (x>20) //点击左侧 - 红方
   {
     Brain.Screen.clearScreen();
     Controller1.Screen.clearScreen();
     //绘制8个红色矩形按钮供选择自动程序(上下两排，每排4个)
     Brain.Screen.drawRectangle(10,20,100,90,color::red);
     Brain.Screen.drawRectangle(130,20,100,90,color::red);
     Brain.Screen.drawRectangle(250,20,100,90,color::red);
     Brain.Screen.drawRectangle(370,20,100,90,color::red);
     Brain.Screen.drawRectangle(10,130,100,90,color::red);
     Brain.Screen.drawRectangle(130,130,100,90,color::red);
     Brain.Screen.drawRectangle(250,130,100,90,color::red);
     Brain.Screen.drawRectangle(370,130,100,90,color::red);
     
     // 添加文字标签
     Brain.Screen.setPenColor(color::white);
     Brain.Screen.printAt(40, 70, "L1");
     Brain.Screen.printAt(160, 70, "L2");
     Brain.Screen.printAt(280, 70, "L3");
     Brain.Screen.printAt(400, 70, "L4");
     Brain.Screen.printAt(40, 180, "R1");
     Brain.Screen.printAt(160, 180, "R2");
     Brain.Screen.printAt(280, 180, "R3");
     Brain.Screen.printAt(400, 180, "R4");
     
     Side=1;
     Alliance=1; //设置为红方
     AutoChoose=0;
      // while(Brain.Screen.pressing()) ;

    }
        }
    //第二步:选择自动程序(已选择联盟但未选择程序时)
    else if (Brain.Screen.pressing()&&Auto==0)
    {
      x = Brain.Screen.xPosition();
      y = Brain.Screen.yPosition();
      Choose=0;
      Brain.Screen.drawCircle(x,y,50); //绘制触摸点
      while(Brain.Screen.pressing()); //等待松开
      
     if (y < 120) { // 上排
        if (x > 370) //最右侧 - 自动程序4
       {
         Brain.Screen.clearScreen();
         Controller1.Screen.clearLine(1);
         Controller1.Screen.clearLine(2);
         Controller1.Screen.clearLine(3);
         Auto=4;
         Brain.Screen.printAt(200,100,"L4");
         button=0;
         PrintTask=task(Print); //启动显示任务
 
       }
        else  if ((x>250)) //第三个 - 自动程序3
       {
         Brain.Screen.clearScreen();
         Controller1.Screen.clearLine(1);
         Controller1.Screen.clearLine(2);
         Controller1.Screen.clearLine(3);
         Auto=3;
         Brain.Screen.printAt(200,100,"L3");
         button=0;
         PrintTask=task(Print);
       }
        else  if ((x>130)) //第二个 - 自动程序2
       {
         Brain.Screen.clearScreen();
         Auto=2; 
         Controller1.Screen.clearLine(1);
         Controller1.Screen.clearLine(2);
         Controller1.Screen.clearLine(3);
         Brain.Screen.printAt(200,100,"L2");
         button=0;
         PrintTask=task(Print);
       }
         else if ((x>10)) //最左侧 - 自动程序1
       {
         Brain.Screen.clearScreen();
         Auto=1;
         Controller1.Screen.clearLine(1);
         Controller1.Screen.clearLine(2);
         Controller1.Screen.clearLine(3);
         Brain.Screen.printAt(200,100,"L1");
         button=0;
         PrintTask=task(Print);
       }
      } else { // 下排
        if (x > 370) //最右侧 - 自动程序8
       {
         Brain.Screen.clearScreen();
         Controller1.Screen.clearLine(1);
         Controller1.Screen.clearLine(2);
         Controller1.Screen.clearLine(3);
         Auto=8;
         Brain.Screen.printAt(200,100,"R4");
         button=0;
         PrintTask=task(Print); //启动显示任务
 
       }
        else  if ((x>250)) //第三个 - 自动程序7
       {
         Brain.Screen.clearScreen();
         Controller1.Screen.clearLine(1);
         Controller1.Screen.clearLine(2);
         Controller1.Screen.clearLine(3);
         Auto=7;
         Brain.Screen.printAt(200,100,"R3");
         button=0;
         PrintTask=task(Print);
       }
        else  if ((x>130)) //第二个 - 自动程序6
       {
         Brain.Screen.clearScreen();
         Auto=6; 
         Controller1.Screen.clearLine(1);
         Controller1.Screen.clearLine(2);
         Controller1.Screen.clearLine(3);
         Brain.Screen.printAt(200,100,"R2");
         button=0;
         PrintTask=task(Print);
       }
         else if ((x>10)) //最左侧 - 自动程序5
       {
         Brain.Screen.clearScreen();
         Auto=5;
         Controller1.Screen.clearLine(1);
         Controller1.Screen.clearLine(2);
         Controller1.Screen.clearLine(3);
         Brain.Screen.printAt(200,100,"R1");
         button=0;
         PrintTask=task(Print);
       }
      }
    
            // Brain.Screen.clearScreen();
  }
  wait(20, msec);
}

/**
 * 以下是手柄按键选择函数(已弃用)
 * 功能:通过手柄按键选择联盟和自动程序
 * 现已改用Brain屏幕触摸选择
 */
/*void Auto_Controller_Choose(){
  if(Controller1.ButtonLeft.pressing()||Controller1.ButtonRight.pressing()||Controller1.ButtonUp.pressing()||Controller1.ButtonY.pressing()){
    if(Controller1.ButtonLeft.pressing()){
      Brain.Screen.clearScreen();
      Controller1.Screen.clearScreen();
      Brain.Screen.drawRectangle(10,20,100,200,color::red);
      Brain.Screen.drawRectangle(130,20,100,200,color::red);
      Brain.Screen.drawRectangle(250,20,100,200,color::red);
      Brain.Screen.drawRectangle(370,20,100,200,color::red);
      Side=1;
      Alliance=1;
      AutoChoose=0;
      while(Controller1.ButtonLeft.pressing()){
        continue;
      }
    }
    else if(Controller1.ButtonRight.pressing()){
      Brain.Screen.clearScreen();
      Controller1.Screen.clearScreen();
      Brain.Screen.drawRectangle(10,20,100,200,color::blue);
      Brain.Screen.drawRectangle(130,20,100,200,color::blue);
      Brain.Screen.drawRectangle(250,20,100,200,color::blue);
      Brain.Screen.drawRectangle(370,20,100,200,color::blue);
      Side=1;
      Alliance=-1; 
      AutoChoose=0;
      while(Controller1.ButtonRight.pressing()){
        continue;
      }
    }
    if(Controller1.ButtonUp.pressing()){
      Auto_temp++;
      if(Auto_temp>4){
        Auto_temp=1;
      }
      while(Controller1.ButtonUp.pressing()){
        continue;
      }
    }
    if(Controller1.ButtonY.pressing()){
      button=0;
      Choose=0;
      Auto=Auto_temp;
      PrintTask=task(Print);
      while(Controller1.ButtonY.pressing()){
        continue;
      }
    }
  }
}*/
task  AutoTask;

/**
 * @brief 自动程序选择界面主函数
 * @return 0
 * 初始化选择界面:
 *   1. 在Brain屏幕上显示红蓝两个大矩形供选择联盟
 *   2. 选择联盟后显示4个小矩形供选择自动程序(1-4)
 *   3. 选择完成后启动屏幕显示任务
 */
int AutoScreen()
{
  //初始化变量
  Auto=0;       //自动程序编号
  button=1;     //按钮状态
  Side=0;       //场地方向
  Alliance=0;   //联盟:0-未选择,1-红方,-1-蓝方
  /*Auto=1;
  button=0;
  Side=1;
  Alliance=-1;*/

  Brain.Screen.clearScreen();
  //  Controller1.Screen.clearLine(1);
  //  Controller1.Screen.clearLine(2);
  //  Controller1.Screen.clearLine(3);
  
  //绘制初始选择界面:左红右蓝
  Brain.Screen.drawRectangle(20,20,200,200,color::red);   //红方选择区
  Brain.Screen.drawRectangle(260,20,200,200,color::blue); //蓝方选择区
  Choose=1;
  
  if (button==1){
  while(button==1){ //循环等待用户选择
    Auto_Ctrl(); //处理选择逻辑
    //Auto_Controller_Choose();
    /*Controller1.Screen.clearLine(3);
    Controller1.Screen.setCursor(3,1);
    Controller1.Screen.print(Gyro.rotation(degrees));*/
    wait(10,msec);
  }
  
  /*以下是备用的手柄按键选择方式(已注释)
  Choose=1;
  Controller1.Screen.clearScreen();
  while(button==1){
    if(Controller1.ButtonLeft.pressing()){
      Side=1;
      Alliance=1;
      while(Controller1.ButtonLeft.pressing()){
        continue;
      }
    }
    /*else if(Controller1.ButtonRight.pressing()){
      Side=1;
      Alliance=-1;
      while(Controller1.ButtonRight.pressing()){
        continue;
      }
    }
    else if(Controller1.ButtonUp.pressing()){
      Auto++;
      if(Auto>4){
        Auto=1;
      }
      while(Controller1.ButtonUp.pressing()){
        continue;
      }
    }
    else if(Controller1.ButtonY.pressing()){
      button=0;
      while(Controller1.ButtonY.pressing()){
        continue;
      }
      break;
    }*/

    /*if(driver_control==1){
      break;
    }*/

    /*显示选择信息到手柄(已注释)
    if(Alliance==1){
      Controller1.Screen.clearLine(1);
      Controller1.Screen.setCursor(1,1);
      Controller1.Screen.print("Red");
    }
    else if(Alliance==-1){
      Controller1.Screen.clearLine(1);
      Controller1.Screen.setCursor(1,1);
      Controller1.Screen.print("Blue");
    }
    else{
      Controller1.Screen.clearLine(1);
      Controller1.Screen.setCursor(1,1);
      Controller1.Screen.print("Not chosen");
    }
    Controller1.Screen.clearLine(2);
    Controller1.Screen.setCursor(2,1);
    Controller1.Screen.print("Auto=%d",Auto);
    Controller1.Screen.clearLine(3);
    Controller1.Screen.setCursor(3,1);
    Controller1.Screen.print("Gyro=%5.2f",Gyro.rotation(degrees));
    wait(20, msec);*/
  }
  PrintTask=task(Print); //启动屏幕显示任务
  double base_motor_temperature=0;
  double Intake_temperature=0;
  double Shoot_temperature=0;
  double Ball_temperature=0;
  while(1){
    Brain.Screen.clearScreen();
    base_motor_temperature=(LeftRun_1.temperature(temperatureUnits::celsius)+LeftRun_2.temperature(temperatureUnits::celsius)+LeftRun_3.temperature(temperatureUnits::celsius)+RightRun_1.temperature(temperatureUnits::celsius)+RightRun_2.temperature(temperatureUnits::celsius)+RightRun_3.temperature(temperatureUnits::celsius))/6;
    Intake_temperature=Intake_1.temperature(temperatureUnits::celsius);
    Shoot_temperature=Shoot_1.temperature(temperatureUnits::celsius);
    Ball_temperature=Ball_1.temperature(temperatureUnits::celsius);
    Brain.Screen.setCursor(1,1);
    Brain.Screen.print("Base_motor_temperature=%.2f",base_motor_temperature);
    Brain.Screen.setCursor(2,1);
    Brain.Screen.print("Intake_temperature=%.2f",Intake_temperature);
    Brain.Screen.setCursor(3,1);
    Brain.Screen.print("Shoot_temperature=%.2f",Shoot_temperature);
    Brain.Screen.setCursor(4,1);
    Brain.Screen.print("Ball_temperature=%.2f",Ball_temperature);
    wait(20, msec);
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////////
// 测试函数(含电机速度日志)
///////////////////////////////////////////////////////////////////////////////////
bool test_log_active = false; //日志任务运行标志

// 日志缓冲区结构与存储(先采集到内存,跑完后一次性输出)
struct TestLogEntry {
    float time_s;
    float left_avg;
    float right_avg;
    TelemetryData t; // 嵌套统一日志数据
};

const int TEST_LOG_MAX = 2000; // 最大日志条目数
TestLogEntry test_log_buf[TEST_LOG_MAX];
int test_log_count = 0; // 当前已存条目数

/**
 * @brief 统一测试日志任务 — 仅采集数据到缓冲区,不做串口输出
 * @return 0
 */
int test_log_task_fn()
{
  float start_time = Brain.timer(timeUnits::msec);
  test_log_count = 0; // 清零计数器

  while(test_log_active && test_log_count < TEST_LOG_MAX)
  {
    // 从电机实时采样左右侧均速(取绝对值)
    float l1 = fabs(LeftRun_1.velocity(velocityUnits::rpm));
    float l2 = fabs(LeftRun_2.velocity(velocityUnits::rpm));
    float l3 = fabs(LeftRun_3.velocity(velocityUnits::rpm));
    float r1 = fabs(RightRun_1.velocity(velocityUnits::rpm));
    float r2 = fabs(RightRun_2.velocity(velocityUnits::rpm));
    float r3 = fabs(RightRun_3.velocity(velocityUnits::rpm));

    float t = (Brain.timer(timeUnits::msec) - start_time) / 1000.0;
    TestLogEntry &e = test_log_buf[test_log_count];
    e.time_s = t;
    e.left_avg  = (l1 + l2 + l3) / 3.0;
    e.right_avg = (r1 + r2 + r3) / 3.0;
    
    // 如果是测速模式，自动计算 diff 并填入 telemetry
    if (current_telemetry.action == 3) {
      float avg_abs_rpm = (l1 + l2 + l3 + r1 + r2 + r3) / 6.0;
      float theoretical_rpm = 200.0 * current_telemetry.target / 100.0;
      current_telemetry.current = avg_abs_rpm;
      current_telemetry.error = avg_abs_rpm - theoretical_rpm;
    }

    // 直接复制当前所有数据
    e.t = current_telemetry;

    test_log_count++;
    vex::task::sleep(20); // 统一20ms高频采样
  }
  return 0;
}

task test_log_task_handle;

/**
 * @brief 将缓冲区中的日志数据通过串口一次性输出(跑完后调用)
 */
void test_log_dump()
{
  // USB CDC 防丢策略: 60ms行间延迟 + 每8行300ms排空暂停
  const int LINE_DELAY  = 60;   
  const int CHUNK_SIZE  = 8;    
  const int CHUNK_PAUSE = 300;  

  // 输出统一CSV表头
  vex::task::sleep(100); 
  printf("telemetry_v1\n");
  vex::task::sleep(LINE_DELAY);
  printf("time_s,left_avg,right_avg,action,target,current,error,error_deriv,dt,p_out,i_out,d_out,total_out,aux_error,aux_deriv,aux_out\n");
  vex::task::sleep(LINE_DELAY);

  // 逐行输出所有缓冲数据, 分块限速
  for(int i = 0; i < test_log_count; i++)
  {
    TestLogEntry &e = test_log_buf[i];
    
    printf("%.3f,%.1f,%.1f,%d,%.2f,%.2f,%.2f,%.3f,%.4f,%.2f,%.2f,%.2f,%.2f,%.2f,%.3f,%.2f\n",
            e.time_s, e.left_avg, e.right_avg,
            e.t.action, e.t.target, e.t.current, e.t.error, e.t.error_deriv, e.t.dt,
            e.t.p_out, e.t.i_out, e.t.d_out, e.t.total_out,
            e.t.aux_error, e.t.aux_deriv, e.t.aux_out);

    // 分块限速: 每CHUNK_SIZE行做一次长暂停让USB buffer排空
    if((i + 1) % CHUNK_SIZE == 0)
      vex::task::sleep(CHUNK_PAUSE);
    else
      vex::task::sleep(LINE_DELAY);
  }

  // 尾部: 等USB buffer完全排空后再输出footer
  vex::task::sleep(500);
  printf("\n");
  vex::task::sleep(LINE_DELAY);
  printf("--- log end (total %d samples) ---\n", test_log_count);
  vex::task::sleep(200);
}

/**
 * @brief 纯直线距离PID测试函数（禁用航向纠偏）
 * 
 * 从100度开始，每次增加100度，直到1200度
 * 每次行驶方向与上次相反（前进/后退交替）
 * 用于独立校准 move_kp / move_kd 参数
 */
/**
 * @brief 直线行驶测试函数(含统一10列日志)
 * @param enc 目标编码器值(度), 正值前进, 负值后退
 * @param g 目标角度(默认0, 即保持当前朝向)
 * 
 * 调用Run_gyro行驶,同时在后台每50ms输出:
 *   time_s, menc, move_err, last_move_error, delta_move_err, vm, dt, current_power, gyro_err, vg, turnpower, left_avg, right_avg
 */
void test_straight(double enc, float g=0)
{
  //初始化坐标系:当前朝向为0°
  now = 0;
  Start = Gyro.rotation(degrees);

  //清零全局日志变量
  current_telemetry = {0};

  //打印测试类型标识
  printf("test_straight_v2\n"); //版本标记: v2=归一化dt版

  //启动日志任务@
  test_log_active = true;
  test_log_task_handle = task(test_log_task_fn);

  //执行直线行驶(传入目标角度g, 实现弧线行驶)
  Run_gyro_new(enc, g);

  //Run_gyro结束后继续记录1秒,观察完整减速过程
  //vex::task::sleep(1000);

  //停止采集任务
  test_log_active = false;
  vex::task::sleep(100); //等待采集任务退出

  //一次性输出所有缓冲数据
  test_log_dump();
  printf("--- test_straight complete, enc=%.1f, g=%.1f ---\n", enc, g);
  vex::task::sleep(500); // 块间间隔: 确保complete信息发完且USB buffer排空后再进入下一个test
}

/**
 * @brief 转向测试函数(含转向日志)
 * 
 * 自动从10度开始,每次增加10度角度差,直到180度
 * 每次基于当前角度旋转相应角度差,调用Turn_Gyro转向
 * 日志仅保留转向相关列: time, gyro_err, vg, turnpower, left_avg, right_avg
 */
void test_turn()
{
  Side=1; 
  //记录起始角度作为参考
  float start_angle = Gyro.rotation(degrees);
  
  //从20度开始,每次增加20度,直到180度
  for(int delta = 20; delta <= 180; delta += 20)
  {
    //计算目标角度 = 当前角度 + 角度差
    float target_angle = Gyro.rotation(degrees) + delta;
    
    //清零全局日志变量
    current_telemetry = {0};

    //打印测试类型标识
    printf("test_turn (delta=%d deg)\n", delta);

    //启动日志任务
    test_log_active = true;
    test_log_task_handle = task(test_log_task_fn);
    vex::task::sleep(50); // 等待日志任务启动并开始记录
    
    //执行PID转向
    Turn_Gyro_new(target_angle);

    //停止采集任务
    test_log_active = false;
    vex::task::sleep(100); //等待采集任务退出

    //一次性输出所有缓冲数据
    test_log_dump();
    printf("--- test_turn complete, delta=%d, target=%.1f ---\n", delta, target_angle);
    
    //每次测试间隔1秒,等待稳定
    vex::task::sleep(100);
  }
  
  printf("=== All turn tests complete (10-180 deg) ===\n");
}


/**
 * @brief 最小驱动功率测试函数
 * 
 * 从0%到30%功率,每2%递增,交替前后方向驱动机器人
 * 每次驱动5秒,停止2秒,同时后台日志任务每50ms采集数据
 * 以二维数组格式输出: {{time_s,power,diff},...}
 */
void test_minspeed()
{
  //打印测试类型标识
  printf("test_minspeed\n");

  //启动日志任务
  current_telemetry = {0};
  current_telemetry.action = 3;
  test_log_active = true;
  test_log_task_handle = task(test_log_task_fn);

  bool forward = true; //第一次往前走

  for(float power = 0; power <= 30; power += 2)
  {
    current_telemetry.target = power; //更新当前功率,供日志任务读取

    //驱动5秒
    if(forward)
      Run_Ctrl(power, power);   //前进
    else
      Run_Ctrl(-power, -power);   //后退

    vex::task::sleep(5000);

    //停止2秒
    RunStop(brake);
    vex::task::sleep(2000);

    //下一次反方向
    forward = !forward;
  }

  //停止采集任务
  test_log_active = false;
  vex::task::sleep(100); //等待采集任务退出

  //一次性输出所有缓冲数据
  test_log_dump();
  printf("--- test_minspeed complete ---\n");
  vex::task::sleep(500); // 块间间隔: 确保complete信息发完且USB buffer排空后再进入下一个test
}

/**
 * @brief 恒速 gyro PD 调参测试函数
 * @param gyro_kp 航向比例系数
 * @param gyro_kd 航向微分系数(单位°/s)
 * 
 * 底盘恒定电压输出(~100 RPM)跑5秒,
 * gyro PD算法与Run_gyro_new一致(含dt归一化),
 * 日志复用 test_log_type=0 (test_straight_v2 格式)
 */
void test_gyro_pd(float gyro_kp, float gyro_kd)
{
  now = 0;
  Start = Gyro.rotation(degrees);
  float g = Side * 0 + Start;        // 目标角度 = 当前朝向

  LeftRun_1.resetPosition();
  RightRun_1.resetPosition();

  // 清零全局日志变量
  current_telemetry = {0};

  printf("test_gyro_pd\n");   // 标识行

  // 启动日志采集
  test_log_active = true;
  test_log_task_handle = task(test_log_task_fn);

  // --- gyro PD 变量（与 Run_gyro_new 一致）---
  float gyro_err = g - Gyro.rotation(degrees);
  float gyro_lasterror = gyro_err;
  float Timer = Brain.timer(timeUnits::sec);
  float last_time = Timer;
  float base_power = 50;              // ~100 RPM（V5 自由转速 200 RPM 的 50%）

  while ((Brain.timer(timeUnits::sec) - Timer) <= 3.0)
  {
    float current_time = Brain.timer(timeUnits::sec);
    float dt = current_time - last_time;
    if (dt < 0.002) { vex::task::sleep(1); continue; }
    last_time = current_time;

    float menc = (fabs(LeftRun_1.position(rotationUnits::deg))
                + fabs(RightRun_1.position(rotationUnits::deg))) / 2;
    gyro_err = g - Gyro.rotation(degrees);

    float vg = (gyro_err - gyro_lasterror) / dt;   // °/s
    float turnpower = gyro_kp * gyro_err + gyro_kd * vg;
    gyro_lasterror = gyro_err;

    // 写入全局变量供日志任务读取
    current_telemetry.action = 2; // 用 action 2 表示带有 gyro_err 的直线运动日志格式
    current_telemetry.target = 0;
    current_telemetry.current = menc;
    current_telemetry.error = 0;
    current_telemetry.error_deriv = 0;
    current_telemetry.dt = dt;
    current_telemetry.p_out = 0;
    current_telemetry.i_out = 0;
    current_telemetry.d_out = 0;
    current_telemetry.total_out = base_power;
    current_telemetry.aux_error = gyro_err;
    current_telemetry.aux_deriv = vg;
    current_telemetry.aux_out = turnpower;

    // 恒压驱动 + gyro 转向补偿
    Run_Ctrl(base_power + turnpower, base_power - turnpower);
    vex::task::sleep(10);
  }
  RunStop(brake);

  test_log_active = false;
  vex::task::sleep(100);
  test_log_dump();
  printf("--- test_gyro_pd complete, kp=%.2f, kd=%.3f ---\n", gyro_kp, gyro_kd);
  vex::task::sleep(500); // 块间间隔: 确保complete信息发完且USB buffer排空后再进入下一个test
}
///////////////////////////////////////////////////////////////////////////////
