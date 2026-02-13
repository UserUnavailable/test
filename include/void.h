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
float motor_min_speed = 26; //电机最小驱动功率(自动阶段统一使用)

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
  Run_Ctrl(spd,-spd);
}

/**
 * @brief 原地转弯
 * @param turnspd 转弯速度(正数右转,负数左转)
 */
void Turn(int turnspd)
{
 Run_Ctrl(turnspd,turnspd);
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
          Ball(-100);  //反转排出
          Intake(30);  //降低吸球速度
          auto_color_divide=1;
        }
        else if(Color_2.color()==red){ //检测到红球
          Ball(100);   //正常吸入
          Intake(100);
          auto_color_divide=0;
        }
        else if(Color.color()==blue&&Color.isNearObject()){ //靠上传感器检测到蓝球
          Ball(-100);  //反转排出
          Intake(30);
          auto_color_divide=1;
        }
        else if(Color.color()==red&&Color.isNearObject()){ //检测到红球
          Ball(100);
          Intake(100);
          auto_color_divide=0;
        }
        else{ //未检测到球
          Ball(100);
          Intake(100);
          auto_color_divide=0;
        }
      }
      else if(Alliance==-1){ //蓝方:保留蓝球,排出红球
        if(Color_2.color()==blue){ //下传感器检测到蓝球
          Ball(100);   //正常吸入
          Intake(100);
          auto_color_divide=0;
        }
        else if(Color_2.color()==red){ //检测到红球
          Ball(-100);  //反转排出
          Intake(30);  //降低吸球速度
          auto_color_divide=1;
        }
        else if(Color.color()==blue&&Color.isNearObject()){ //上传感器检测到蓝球
          Ball(100);
          Intake(100);
          auto_color_divide=0;
        }
        else if(Color.color()==red&&Color.isNearObject()){ //检测到红球
          Ball(-100);  //反转排出
          Intake(30);
          auto_color_divide=1;
        }
        else{ //未检测到球
          Ball(100);
          Intake(100);
          auto_color_divide=0;
        }
      }
    }
    auto_color_divide=0;
    if(driver_control==1){   //在手动阶段退出防止常驻筛球
      break;
    }
  wait(10);
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
    Intake(0);
    Ball(0);
    Shoot(0);
    auto_color_ctrl=0; //关闭颜色控制
    }
  if (spd==1) //中高桥模式:全速正转
  {
    Intake(100);
    if(!auto_color_divide){
      Ball(100);     
    }
    Shoot(100); 
    auto_color_ctrl=0;
    }  
  if (spd==-1) //低桥模式:吸球反转
  {
    Intake(-100); 
    if(!auto_color_divide){
      Ball(100);
    }
    Shoot(100);
    auto_color_ctrl=0;
  }  
  if (spd==2) //自动吸球分球模式
  {
    Intake(100); 
    if(!auto_color_divide){
      Ball(100);
    }
    Shoot(-60);  //射球轮反转
    auto_color_ctrl=1; //开启颜色识别
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
/**
 * @brief 根据颜色传感器智能吸球
 * 检测球的颜色并根据联盟判断是否吸入或排出
 */
void Ball_Intake()
{
  if (Alliance==0){ //手动模式:不判断颜色,直接吸球
      Intake(100);
      Ball(100);
    }
  else if(Alliance==1){ //红方:保留红球,排出蓝球
    if(Color_2.color()==blue){   //靠下传感器检测到蓝球
      Ball(-100);  //反转排出
      Intake(30);  //降低吸球速度
    }
    else if(Color_2.color()==red){ //检测到红球
      Ball(100);   //正常吸入
      Intake(100);
    }
    else if(Color.color()==blue&&Color.isNearObject()){ //靠上传感器检测到蓝球
      Ball(-100);  //反转排出
      Intake(30);
    }
    else if(Color.color()==red&&Color.isNearObject()){ //检测到红球
      Ball(100);
      Intake(100);
    }
    else{ //未检测到球
      Ball(100);
      Intake(100);
    }
  }
  else if(Alliance==-1){ //蓝方:保留蓝球,排出红球
    if(Color_2.color()==blue){ //下传感器检测到蓝球
      Ball(100);   //正常吸入
      Intake(100);
    }
    else if(Color_2.color()==red){ //检测到红球
      Ball(-100);  //反转排出
      Intake(30);  //降低吸球速度
    }
    else if(Color.color()==blue&&Color.isNearObject()){ //上传感器检测到蓝球
      Ball(100);
      Intake(100);
    }
    else if(Color.color()==red&&Color.isNearObject()){ //检测到红球
      Ball(-100);  //反转排出
      Intake(30);
    }
    else{ //未检测到球
      Ball(100);
      Intake(100);
    }
  }

  /*以下是旧版本的吸球逻辑(已注释)
  Intake(100);
  Ball(100);*/
  
  /*旧版本的颜色判断逻辑(已注释):
  根据传感器检测到的颜色调整吸球和分球速度
  else if(Color_2.color()==blue)//下
  {
    //Ball(-Alliance*100);
    Ball(100);
    Intake(30);
    //Intake(-Alliance*40+60);
    //Intake(-Alliance*25);
  }
  else if (Color_2.color()==red)
    {
    //Ball(Alliance*100);
    Ball(100);
    Intake(30);
    //Intake(Alliance*40+60);
    //Intake(Alliance*25);
    }
  else if(Color.color()==blue&&Color.isNearObject())//上
  {
    Ball(-Alliance*100);
    Intake(Alliance*100);
    //Intake(100);
    //Intake(-Alliance*10);
  }
  else if (Color.color()==red&&Color.isNearObject())//
    {
    Ball(Alliance*100);
    Intake(-Alliance*100);
    //Intake(100);
    //Intake(Alliance*10);
    }
    else
    {
    Ball(100);
    Intake(100);
    }*/
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
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief 陀螺仪辅助直线行驶(P控制)
 * @param enc 目标编码器值(度), 正值前进, 负值后退
 * @param g 目标角度
 * 使用陀螺仪实时纠偏,保证直线行驶精度
 * 全程使用P控制自动计算行驶功率
 */
void Run_gyro(double enc, float g=now)
{
  //enc=enc*3;
  g=Side*g+Start; //根据场地方向调整目标角度
  LeftRun_1.resetPosition();
  RightRun_1.resetPosition();
  
  //PD参数
  //float gyro_kp = 1;
  float gyro_kp = 0;   //角度比例系数
  float gyro_kd = 0;  //角度微分系数
  float move_kp = 0.13;  //距离比例系数
  float move_kd = 0.2;  //距离微分系数(阻尼,防过冲)

  float menc=0;//左右轮编码器平均值
  float vm = 0;//线速度差
  float turnpower;//转向补偿功率
  float movepower;//移动补偿功率
  float move_lasterror;//上一次距离误差
  float gyro_lasterror = 0;//上一次角度误差
  float move_err = fabs(enc) - fabs(menc);//编码器当前与目标差值
  float gyro_err = g - Gyro.rotation(degrees) ;//陀螺仪当前与目标差值

  //int timeout =  enc < 300 ? 500 : enc * 1.5;
  float Timer=Brain.timer(timeUnits::sec);
  float timeout=fabs(enc) / 200.0 + 1.0; //超时保护
  
	while((Brain.timer(timeUnits::sec)-Timer)<=timeout+0.5)
  {
    //实时更新编码器和陀螺仪数据
    menc = (fabs(LeftRun_1.position(rotationUnits::deg))+ fabs(RightRun_1.position(rotationUnits::deg)))/2;
    move_err = fabs(enc) - fabs(menc);
    gyro_err = g - Gyro.rotation(degrees) ;
    vm = move_err-move_lasterror;    //计算距离变化率
    move_lasterror = move_err;
    
    //PD控制计算转向补偿
    turnpower = gyro_kp*gyro_err + gyro_kd*(gyro_err - gyro_lasterror);
    gyro_lasterror = gyro_err;

    //PD控制计算行驶功率
    movepower = move_kp * move_err + move_kd * vm;
    if(movepower < motor_min_speed) movepower = motor_min_speed; // 最小速度限制
    double final_power = movepower;

    //到达目标判断
    if (fabs(enc)-fabs(menc)<2 && fabs(vm) < 1)//距离误差<2度 且 速度变化<1
    {
      break;
    }
    else
    {
    //应用补偿后的功率到左右电机
    Run_Ctrl((sgn(enc)*final_power)+ turnpower,-(sgn(enc)*final_power) +turnpower);
    }
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
    Run_Ctrl(final_power+ turnpower,-final_power +turnpower);
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
    Run_Ctrl(final_power+ turnpower,-final_power +turnpower);
  }
  RunStop(brake);
}
///////////////////////////////////////////////////////////////////////////////////
task  BallTask;

/**
 * @brief 颜色识别吸球任务函数
 * @return 0
 * 模式说明: 
 *   mode==0: 停止吸球和分球
 *   mode==1: 启动智能分球吸球
 * 可作为独立任务运行
 */
int Intake_Color()//0：中断/1：分球/2：吸球停球/3：异色标记/4：打高桥/打
{
 Color.setLightPower(100, percent);    //设置颜色传感器1亮度
 Color_2.setLightPower(100, percent);  //设置颜色传感器2亮度
 Intake(100);
 wait(100);
  Ball(100);
  while(1)
  {
      if (mode==0) //停止模式
      {
        Intake(0);
        Ball(0);
        //Shoot(0); //开启吸球
      }
      if (mode==1) //吸球模式
      {
        Ball_Intake(); //调用智能吸球函数
        // if (abs(Intake_1.velocity(velocityUnits::rpm))<5)
        //   {
        //     Intake(-100);
        //     wait(100);
        //     Intake(100);
        //     wait(100);
        //   }
      }

  }
  return 0;
}
//////////////////////////////////////////////////////////////////////////////////
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
   float error = target - Gyro.rotation(degrees) ;//与目标角度距离
   
   //自适应PID参数
   float kp =fabs(error)>30&&fabs(error)<120?3.5:(fabs(error)<30?6:3);//根据误差动态调整kp
   float ki =35;   //积分系数
   float kd =fabs(error)>30&&fabs(error)<120?35:(fabs(error)<30?50:30); //自适应kd
   float irange=3.0;  //积分限幅
   float istart=60;   //积分启动阈值
   float dtol = 0.2;  //停止速度阈值
   float errortolerance = 2; //角度误差容忍度
   float lim =100;    //功率限幅
   
   float lasterror;   //上一次角度误差
   float V= 0;        //角速度(微分项)
   float integral = 0;//积分累加值
   bool arrived;      //到达目标标志
   float Time=Brain.timer(timeUnits::sec);
   float timeout=fabs(error/50); //根据角度计算超时时间
   float pow;         //实际输出功率
   
   lasterror = error;
   arrived = error == 0;
   
   while (!arrived)
   {
    error = target - Gyro.rotation(degrees) ;
    V = error - lasterror; //计算角速度(微分)
    lasterror = error;
    
    //提前退出判断:角度误差小且电机基本停止
    if (fabs(error)<=3)
    {
      if(fabs(RightRun_1.velocity(velocityUnits::rpm))<5||fabs(LeftRun_1.velocity(velocityUnits::rpm)<5)){break;}
    }
    
    //到达判断:误差和速度都在容忍范围内,或超时
    if ((fabs(error) <= errortolerance && fabs(V) <= dtol) || (Brain.timer(timeUnits::sec)-Time)>=timeout+1)
    {arrived = true;}
    
    //积分计算(仅在误差较小时启用,防止积分饱和)
    if (fabs(integral) < irange && fabs(error) < istart)
      integral += sgn(error)*0.01 ;
    
    //积分清零(误差过零点时)
    if (error * lasterror <= 0)
    integral = 0;
    
    //PID计算输出功率
    pow = kp * error + kd * V + ki*integral;
    pow = fabs(pow) > lim ? sgn(pow) * lim : pow; //功率限幅
    Turn(pow);
    //lasterror = error;
    wait(10,msec);
  }
   RunStop(brake);

}
//////////////////////////////////////////////////////////////////////////
/**
 * @brief 单侧转向PID控制
 * @param target 目标角度
 * 通过控制单侧电机实现转向,转弯半径较大
 * target>0时控制左侧, target<0时控制右侧
 */
void Turn_Side(float target)
{
   now=target;
   target=Side*target+Start; //根据场地方向调整
   float error = target - Gyro.rotation(degrees) ;//与目标角度距离
   
   //PID参数
   float kp =5;    //比例系数
   float ki =35;   //积分系数
   float kd =40;   //微分系数
   float irange=2;    //积分限幅
   float istart=80;   //积分启动阈值
   float dtol = 0.2;  //停止速度阈值
   float errortolerance = 2; //角度误差容忍度
   float lim =100;    //功率限幅
   
   float lasterror;   //上一次角度误差
   float V= 0;        //角速度
   float integral = 0;//积分累加
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
    
    //积分计算
    if (fabs(integral) < irange && fabs(error) < istart)
      integral += sgn(error)*0.01 ;
    
    //积分清零
    if (error * lasterror <= 0)
    integral = 0;
    
    //PID计算
    pow = kp * error + kd * V + ki*integral;
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
	while((Brain.timer(timeUnits::sec)-Time_1<=timeout/1000))
  {
    //检测角度偏离
    if (fabs(Gyro.rotation(degrees) -now+Start)>err)
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
			Run_V5(speed,-speed); //继续前进
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
    //绘制4个蓝色矩形按钮供选择自动程序
    Brain.Screen.drawRectangle(10,20,100,200,color::blue);
    Brain.Screen.drawRectangle(130,20,100,200,color::blue);
    Brain.Screen.drawRectangle(250,20,100,200,color::blue);
    Brain.Screen.drawRectangle(370,20,100,200,color::blue);
    Side=1;
    Alliance=-1; //设置为蓝方
    AutoChoose=0;

   }
    else if (x>20) //点击左侧 - 红方
   {
     Brain.Screen.clearScreen();
     Controller1.Screen.clearScreen();
     //绘制4个红色矩形按钮供选择自动程序
     Brain.Screen.drawRectangle(10,20,100,200,color::red);
     Brain.Screen.drawRectangle(130,20,100,200,color::red);
     Brain.Screen.drawRectangle(250,20,100,200,color::red);
     Brain.Screen.drawRectangle(370,20,100,200,color::red);
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
      
     if (Brain.Screen.xPosition()>370) //最右侧 - 自动程序4
    {
      Brain.Screen.clearScreen();
      Controller1.Screen.clearLine(1);
      Controller1.Screen.clearLine(2);
      Controller1.Screen.clearLine(3);
      Auto=4;
      Brain.Screen.printAt(200,100,"4");
      button=0;
      PrintTask=task(Print); //启动显示任务

    }
     else  if ((Brain.Screen.xPosition()>250)) //第三个 - 自动程序3
    {
      Brain.Screen.clearScreen();
      Controller1.Screen.clearLine(1);
      Controller1.Screen.clearLine(2);
      Controller1.Screen.clearLine(3);
      Auto=3;
      Brain.Screen.printAt(200,100,"3");
      button=0;
      PrintTask=task(Print);
    }
     else  if ((Brain.Screen.xPosition()>130)) //第二个 - 自动程序2
    {
      Brain.Screen.clearScreen();
      Auto=2; 
      Controller1.Screen.clearLine(1);
      Controller1.Screen.clearLine(2);
      Controller1.Screen.clearLine(3);
      Brain.Screen.printAt(200,100,"2");
      button=0;
      PrintTask=task(Print);
    }
      else if ((Brain.Screen.xPosition()>10)) //最左侧 - 自动程序1
    {
      Brain.Screen.clearScreen();
      Auto=1;
      Controller1.Screen.clearLine(1);
      Controller1.Screen.clearLine(2);
      Controller1.Screen.clearLine(3);
      Brain.Screen.printAt(200,100,"1");
      button=0;
      PrintTask=task(Print);
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

/**
 * @brief 电机速度日志任务
 * @return 0
 * 每50ms输出一次6个底盘电机速度绝对值的均值(RPM)到电脑终端
 */
int test_log_task_fn()
{
  float start_time = Brain.timer(timeUnits::msec);
  printf("time_ms, avg_abs_rpm\n");
  while(test_log_active)
  {
    float t = Brain.timer(timeUnits::msec) - start_time;
    float l1 = fabs(LeftRun_1.velocity(velocityUnits::rpm));
    float l2 = fabs(LeftRun_2.velocity(velocityUnits::rpm));
    float l3 = fabs(LeftRun_3.velocity(velocityUnits::rpm));
    float r1 = fabs(RightRun_1.velocity(velocityUnits::rpm));
    float r2 = fabs(RightRun_2.velocity(velocityUnits::rpm));
    float r3 = fabs(RightRun_3.velocity(velocityUnits::rpm));
    float avg_abs_rpm = (l1 + l2 + l3 + r1 + r2 + r3) / 6.0;
    printf("%.0f, %.1f\n", t, avg_abs_rpm);
    vex::task::sleep(50);
  }
  return 0;
}

task test_log_task_handle;

/**
 * @brief 直线行驶测试函数(含实时速度日志)
 * @param enc 目标编码器值(度), 正值前进, 负值后退
 * 
 * 调用Run_gyro行驶,同时在后台任务中每50ms
 * 将6个底盘电机速度绝对值的均值(RPM)通过printf输出到电脑终端
 * 格式: CSV (time_ms, avg_abs_rpm)
 */
void test_straight(double enc)
{
  //启动速度日志任务
  test_log_active = true;
  test_log_task_handle = task(test_log_task_fn);
  
  //执行直线行驶(g使用默认值now)
  Run_gyro(enc);
  
  //Run_gyro结束后继续记录1秒,观察完整减速过程
  //vex::task::sleep(1000);
  
  //停止日志任务
  test_log_active = false;
  vex::task::sleep(100); //等待最后一次日志输出完成
  printf("--- test_straight complete, enc=%.1f ---\n", enc);
}

/**
 * @brief 转向测试函数(含实时速度日志)
 * @param angle 目标角度
 * 
 * 调用Turn_Gyro转向,同时在后台任务中每50ms
 * 将6个底盘电机速度绝对值的均值(RPM)通过printf输出到电脑终端
 * 格式: CSV (time_ms, avg_abs_rpm)
 */
void test_turn(float angle)
{
  //启动速度日志任务
  test_log_active = true;
  test_log_task_handle = task(test_log_task_fn);
  
  //执行PID转向
  Turn_Gyro(angle);
  
  //停止日志任务
  test_log_active = false;
  vex::task::sleep(100); //等待最后一次日志输出完成
  printf("--- test_turn complete, angle=%.1f ---\n", angle);
}

float test_gyro_target_heading = 0; //目标航向角(经Side/Start变换后),供日志任务读取

/**
 * @brief 陀螺仪纠偏测试 - 日志任务
 * @return 0
 * 每50ms输出: time_ms, heading_err, left_avg, right_avg, lr_diff
 * heading_err = 目标航向 - 当前航向 (正=偏左需右修, 负=偏右需左修)
 * lr_diff = 左侧均速 - 右侧均速 (反映turnpower纠偏效果)
 */
int test_gyro_log_fn()
{
  float start_time = Brain.timer(timeUnits::msec);
  printf("time_ms, heading_err, left_avg, right_avg, lr_diff\n");
  while(test_log_active)
  {
    float t = Brain.timer(timeUnits::msec) - start_time;
    float heading_err = test_gyro_target_heading - Gyro.rotation(degrees);

    float l1 = LeftRun_1.velocity(velocityUnits::rpm);
    float l2 = LeftRun_2.velocity(velocityUnits::rpm);
    float l3 = LeftRun_3.velocity(velocityUnits::rpm);
    float r1 = RightRun_1.velocity(velocityUnits::rpm);
    float r2 = RightRun_2.velocity(velocityUnits::rpm);
    float r3 = RightRun_3.velocity(velocityUnits::rpm);

    float left_avg  = (l1 + l2 + l3) / 3.0;
    float right_avg = (r1 + r2 + r3) / 3.0;
    float lr_diff   = left_avg - right_avg;

    printf("%.0f, %.2f, %.1f, %.1f, %.1f\n", t, heading_err, left_avg, right_avg, lr_diff);
    vex::task::sleep(50);
  }
  return 0;
}

/**
 * @brief 陀螺仪纠偏测试函数(用于调试gyro_kp/gyro_kd)
 * @param enc 目标编码器值(度), 正值前进, 负值后退
 * @param g 目标角度(默认now)
 * 
 * 调用Run_gyro行驶,同时在后台任务中每50ms输出:
 *   heading_err(航向误差), left_avg/right_avg(左右均速), lr_diff(左右速差)
 * 用于观察陀螺仪PD纠偏的实时效果
 */
void test_gyro(double enc, float g=now)
{
  //计算变换后的目标航向,供日志任务读取
  test_gyro_target_heading = Side * g + Start;

  //启动陀螺仪日志任务
  test_log_active = true;
  test_log_task_handle = task(test_gyro_log_fn);

  //执行直线行驶
  Run_gyro(enc, g);

  //Run_gyro结束后继续记录1秒,观察完整减速过程
  vex::task::sleep(1000);

  //停止日志任务
  test_log_active = false;
  vex::task::sleep(100); //等待最后一次日志输出完成
  printf("--- test_gyro complete, enc=%.1f, g=%.1f ---\n", enc, g);
}

float test_minspeed_power = 0; //当前测试功率(0-100),供日志任务读取

/**
 * @brief 最小驱动功率测试 - 日志任务
 * @return 0
 * 每50ms采集{min, power, diff},不换行,以二维数组格式连续输出:
 * {{min,power,diff},{min,power,diff},...}
 */
int test_minspeed_log_fn()
{
  float start_time = Brain.timer(timeUnits::msec);
  bool first = true;
  printf("{");
  while(test_log_active)
  {
    float t = (Brain.timer(timeUnits::msec) - start_time) / 1000.0; // 单位：分钟
    float l1 = fabs(LeftRun_1.velocity(velocityUnits::rpm));
    float l2 = fabs(LeftRun_2.velocity(velocityUnits::rpm));
    float l3 = fabs(LeftRun_3.velocity(velocityUnits::rpm));
    float r1 = fabs(RightRun_1.velocity(velocityUnits::rpm));
    float r2 = fabs(RightRun_2.velocity(velocityUnits::rpm));
    float r3 = fabs(RightRun_3.velocity(velocityUnits::rpm));
    float avg_abs_rpm = (l1 + l2 + l3 + r1 + r2 + r3) / 6.0;
    //float avg_abs_rpm = r3;
    float theoretical_rpm = 200.0 * test_minspeed_power / 100.0;
    float diff = avg_abs_rpm - theoretical_rpm;

    if(!first) printf(",");
    printf("{%.2f,%.0f,%.1f}", t, test_minspeed_power, diff);
    first = false;

    vex::task::sleep(50);
  }
  printf("}\n");
  return 0;
}

/**
 * @brief 最小驱动功率测试函数
 * 
 * 从0%到30%功率,每2%递增,交替前后方向驱动机器人
 * 每次驱动5秒,停止2秒,同时后台日志任务每50ms采集数据
 * 以二维数组格式输出: {{min,diff},{min,diff},...}
 */
void test_minspeed()
{
  //启动日志任务
  test_log_active = true;
  test_minspeed_power = 0;
  test_log_task_handle = task(test_minspeed_log_fn);

  bool forward = true; //第一次往前走

  for(float power = 0; power <= 30; power += 2)
  {
    test_minspeed_power = power; //更新当前功率,供日志任务读取

    //驱动5秒
    if(forward)
      Run_Ctrl(power, -power);   //前进
    else
      Run_Ctrl(-power, power);   //后退

    vex::task::sleep(5000);

    //停止2秒
    RunStop(brake);
    vex::task::sleep(2000);

    //下一次反方向
    forward = !forward;
  }

  //停止日志任务
  test_log_active = false;
  vex::task::sleep(100);
  printf("--- test_minspeed complete ---\n");
}
///////////////////////////////////////////////////////////////////////////////
