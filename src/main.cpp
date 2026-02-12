
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       VEX                                                       */
/*    Created:      Thu Sep 26 2019                                           */
/*    Description:  Clawbot Competition Template                              */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Controller1          controller                    
// LeftRun_1            motor         1               
// LeftRun_2            motor         2               
// LeftRun_3            motor         3               
// RightRun_1           motor         4               
// RightRun_2           motor         5               
// RightRun_3           motor         6               
// Intake_1             motor         11              
// Shoot_1              motor         13              
// Ball_1               motor         12              
// Load                 digital_out   A               
// Gyro                 inertial      20              
// Color                optical       14              
// Color_2              optical       15              
// Wing_R               digital_out   B               
// Wing_L               digital_out   D               
// Up                   digital_out   C               
// ---- END VEXCODE CONFIGURED DEVICES ----

//////////////////////////////////////////////////////////
#include <random>
int Load_Dis=240;//撞板后退到填装桶距离

int Load_Deg=0;//取填装后转向长桥角度
//////////////////////////////////////////////////////////
int Side=1,Alliance=1;//红蓝边转向标定量
int Auto=1;//默认自动第几套
//////////////////////////////////////////////////////////
int Choose=1;//进入选自动界面标定参数
int AutoChoose=0;//主控器屏幕选红蓝时的标定参数
int testmotor=0;//开机测试马达是不是都转(未启用)
int button=0;//遥控器释放自动防误触
float Auto_time=0;//自动阶段总计时器
float Start,now;//陀螺仪标定初始位置自动开始时重置
float turn_slow=0.99;//转向速度控制 //0.6
float speedctrl=1;//前进速度控制1.2
int ball_ctrl=0,shoot_ctrl=0,intake_ctrl=0;
//int intake_ctrl=0,color_ctrl=0;//change_ctrl=1,shoot_ctrl=0,
int flag_start;
//int Flager_Ball=1;//吸球标定
int mode=0;//分球线程模式
//int joy_print=1;//默认自动第几套
float start_roll;
//int auto_color_ctrl=0;//自动颜色控制
int driver_control=0;
int auto_control=1;
bool auto_stop=false;
extern bool auto_stop;
//int auto_start=0;
#include "vex.h"
#include "void.h"
#include "auto_All.h"
//#include "auto_backup.h"
#include "mainauto.h"


void test()
{
//button=1;
Brain.resetTimer();
///////////////////////////////测试内容///////////////////////////////
//#include "auto_skill.h" 
  // safe=2;
  // BallTask=task(Intake_Color);
//Turn_Gyro(90);
AutoPro();//手动放自动的点
//////////////////////////////////////////////////////////////////////
//button=2;
}
#include "joystick.h"
// A global instance of competition
competition Competition;

// define your global instances of motors and other devices here

/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.      */
/*  Do them in the following function.  You must return from this function   */
/*  or the autonomous and usercontrol tasks will not be started.  This       */
/*  function is only called once after the V5 has been powered on and        */
/*  not every time that the robot is disabled.                               */
/*---------------------------------------------------------------------------*/

void pre_auton(void) {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();
  Up.set(true);
  Basket.set(false);
  Gyro.startCalibration();
  while (Gyro.isCalibrating()) { task::sleep(50); }
  Gyro.setRotation(0.0, degrees);
  start_roll=Gyro.orientation(roll, degrees);
  wait(100);
  if (Gyro.rotation(degrees)!=0)
  { Controller1.rumble("-");
    PrintTask=task(Print);
  }
  /*else
  {  while (Gyro.isCalibrating()) { task::sleep(50); }
  Gyro.setRotation(0.0, degrees);
  start_roll=Gyro.orientation(roll, degrees);
    wait(100);}*/
  //PrintTask=task(Print);
  //AutoTask=task(AutoScreen);

  AutoScreen();

  // All activities that occur before the competition starts
  // Example: clearing encoders, setting servo positions, ...
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              Autonomous Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void autonomous(void) {
  Up.set(true);
  Basket.set(true);
  auto_control=1;
  driver_control=0;
  ColorThread=thread(Color_Control);
// ..........................................................................
  AutoPro();
  // Insert autonomous user code here.
  // ..........................................................................
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase of */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void usercontrol(void) {
  // User control code here, inside the loop
  Up.set(true);
  Basket.set(true);
  RunStop(coast);
  task::stop (BallTask);
  task::stop (AutoTask);
  driver_control=1;
  auto_control=0;
  auto_color_ctrl=0;
  PrintTask=task(Print);
  //driver_control=1;

  Wing_R.set(true);
  //Vision_Center_Track();
  Joystick();
  while (1) {
    // This is the main execution loop for the user control program.
    // Each time through the loop your program should update motor + servo
    // values based on feedback from the joysticks.

    // ........................................................................
    // Insert user code here. This is where you use the joystick values to
    // update your motors, etc.
    // ........................................................................
    wait(20, msec); // Sleep the task for a short amount of time to
                    // prevent wasted resources.
  }
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
  /*PrintTask=task(Print);
  AutoScreen();*/
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);


  // Run the pre-autonomous function.
  pre_auton();
  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}


// // "when started" hat block
// int whenStarted1() {

//   Gyro.startCalibration();
//   while (Gyro.isCalibrating()) { task::sleep(50); }
//   Gyro.setRotation(0.0, degrees);
//   start_roll=Gyro.orientation(roll, degrees);
//   Controller1.rumble("-");
//   //AutoTask=task(AutoScreen);
//   //Print.broadcast();

//   return 0;
// }


// // "when autonomous" hat block
// int onauton_autonomous_0() {
//   AutoPro();
//   return 0;
// }

// // "when driver control" hat block
// // int ondriver_drivercontrol_0() {
// //   while (true) {
// //   wait(5, msec);
// //   }
// //   return 0;
// // }

// int ondriver_drivercontrol_0() {
//   while (true) {
//   Joystick();
//   wait(5, msec);
//   }
//   return 0;
// }


// void VEXcode_driver_task() {
//   // Start the driver control tasks....
//   vex::task drive0(ondriver_drivercontrol_0);
//   while(Competition.isDriverControl() && Competition.isEnabled()) {this_thread::sleep_for(10);}
//   drive0.stop();
//   return;
// }

// void VEXcode_auton_task() {
//   // Start the auton control tasks....
//   vex::task auto0(onauton_autonomous_0);
//   while(Competition.isAutonomous() && Competition.isEnabled()) {this_thread::sleep_for(10);}
//   auto0.stop();
//   return;
// }



// int main() {
//   vex::competition::bStopTasksBetweenModes = false;
//   Competition.autonomous(VEXcode_auton_task);
//   Competition.drivercontrol(VEXcode_driver_task);

//   // Initializing Robot Configuration. DO NOT REMOVE!
//   vexcodeInit();

//   // post event registration

//   // set default print color to black
//   Print(onevent_Print_0);
//   //AutoScreen(onevent_AutoScreen_0);

//   // wait for rotation sensor to fully initialize
//   wait(30, msec);//30

//   whenStarted1();
// }