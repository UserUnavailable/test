void Auto_1()
{
  #include "auto1.h"
} 

void Auto_2()
{ 
 #include "auto2.h"
}
void Auto_3()
{
 #include "auto3.h"
}
void Auto_4()
{
 //#include "auto_skill.h"
 #include "auto4.h"  
}

/////////////////////////////////////////////
void AutoPro ()
{
button=1; 
Color.setLightPower(100, percent); 
Color_2.setLightPower(100, percent);
Color_3.setLightPower(100, percent);
//auto_start=1;
task::stop (AutoTask);
task::stop (Print);
Brain.resetTimer();
Side=1;
 if (Auto==1)
 {Auto_1();}
 if (Auto==2)
 {Auto_2();}
 if (Auto==3)
 {Auto_3();}
 if (Auto==4)
 {Auto_4();}
 Auto_time=Brain.timer(timeUnits::sec);
 Brain.Screen.clearScreen();
 Brain.Screen.print(Brain.timer(timeUnits::sec));
 Controller1.Screen.clearLine(3);
 Controller1.Screen.setCursor(3,1);
 Controller1.Screen.print(Auto_time);
 driver_control=1;
 task::stop (BallTask);
}

