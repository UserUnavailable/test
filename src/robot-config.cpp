#include "vex.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

// A global instance of brain used for printing to the V5 Brain screen
brain  Brain;

// VEXcode device constructors
controller Controller1 = controller(primary);
motor LeftRun_1 = motor(PORT9, ratio18_1, true);
motor LeftRun_2 = motor(PORT13, ratio18_1, false);
motor LeftRun_3 = motor(PORT3, ratio18_1, true);
motor RightRun_1 = motor(PORT11, ratio18_1, true);
motor RightRun_2 = motor(PORT12, ratio18_1, false);
motor RightRun_3 = motor(PORT14, ratio18_1, true);
motor Intake_1 = motor(PORT6, ratio6_1, true);
motor Shoot_1 = motor(PORT2, ratio6_1, false);
motor Ball_1 = motor(PORT1, ratio6_1, false);
digital_out Load = digital_out(Brain.ThreeWirePort.B);
inertial Gyro = inertial(PORT4);
optical Color = optical(PORT17);    //进球口上方传感器
optical Color_3 = optical(PORT14);  //吐球口传感器
optical Color_2 = optical(PORT16);  //进球口下方传感器
//optical out_detect = optical(PORT20)
digital_out load = digital_out(Brain.ThreeWirePort.A);
digital_out Wing_R = digital_out(Brain.ThreeWirePort.H);
digital_out Wing_L = digital_out(Brain.ThreeWirePort.F);
digital_out Up = digital_out(Brain.ThreeWirePort.D);
digital_out Basket = digital_out(Brain.ThreeWirePort.C);
digital_out Anchor = digital_out(Brain.ThreeWirePort.E);
distance Distance1 = distance(PORT5); //距离传感器，端口待定
distance Distance2 = distance(PORT4);
aivision::colordesc Vision__HighGoal(1, 255, 255, 0, 10, 0.20);
aivision Vision(PORT20, Vision__HighGoal);
// VEXcode generated functions
// define variable for remote controller enable/disable
bool RemoteControlCodeEnabled = true;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void vexcodeInit( void ) {
  // nothing to initialize
}