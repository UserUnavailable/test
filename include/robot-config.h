using namespace vex;

extern brain Brain;

// VEXcode devices
extern controller Controller1;
extern motor LeftRun_1;
extern motor LeftRun_2;
extern motor LeftRun_3;
extern motor RightRun_1;
extern motor RightRun_2;
extern motor RightRun_3;
extern motor Intake_1;
extern motor Shoot_1;
extern motor Ball_1;
extern digital_out Load;
extern inertial Gyro;
extern optical Color;
extern optical Color_2;
extern optical Color_3;
//extern optical out_detect;
extern distance Distance1;
extern distance Distance2;
extern aivision::colordesc Vision__HighGoal;
extern aivision Vision;
extern digital_out Wing_R;
extern digital_out Wing_L;
extern digital_out Up;
extern digital_out Basket;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void  vexcodeInit( void );