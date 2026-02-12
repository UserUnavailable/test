#include <cmath>
#include <thread>
//bool auto_stop=false;
int first_auto_stop=1;
int mode_ctrl=2;
/*void auto_Intake(){
  while(true){
    if(Intake_1.torque()>=0.35){
      Intake(-100);
    }
    wait(20, msec);
  }
}
thread AutoIntakeThread=thread(auto_Intake);*/
void base_control(){
  while(true){
    if(auto_stop){
      wait(20,msec);
      continue;
    }
    else{
      int left;
      int right;
      int a,b;
      int transverse;
      a=Controller1.Axis3.value();
      b=Controller1.Axis1.value();
      left=a*speedctrl+b*turn_slow;
      right=-a*speedctrl+b*turn_slow;
      Run_Ctrl(left,right);
    }
    if(auto_control==1){
      break;
    }
    wait(5, msec);
  }
}
void hook_control(){
  while(true){
    if(Controller1.ButtonL1.pressing()){
      Wing_L.set(false);
    }
    else{
      Wing_L.set(true);
    }
    if(auto_control==1){
      break;
    }
    wait(20, msec);
  }
}
void hold_ctrl(){
  while(true){
    if(Controller1.ButtonY.pressing()){
      if(auto_stop){
        RunStop(coast);
        auto_stop=false;
      }
      else{
        RunStop(hold);
        auto_stop=true;
      }
      while(Controller1.ButtonY.pressing()){
        continue;
      }
    }
    if(auto_control==1){
      break;
    }
    wait(20, msec);
  }
}
void load_ctrl(){
  while(true){
    if(Controller1.ButtonL2.pressing()){
      Load.set(!Load.value());
      while(Controller1.ButtonL2.pressing()){
        continue;
      }
    }
    if(auto_control==1){
      break;
    }
    wait(20, msec);
  }
}
void Intake_Shoot_ctrl(){
  while(true){
    if(Controller1.ButtonR1.pressing()){
      Ball_Intake();
      ball_ctrl=0;            //可能会导致一边吸球一边吐球时由于筛选导致吐球不顺
      intake_ctrl=1;
      if(shoot_ctrl==1){
        Shoot(-50);
      }
    }
    /*else if(Controller1.ButtonR2.pressing()){
      ball_ctrl=0;
      Intake(-100);
      Ball(-100);
      //ball_ctrl=-1;
    }*/
    else{
      if(shoot_ctrl==1){
        Shoot(-40);
        Intake(0);
      }
      if(!Controller1.ButtonR2.pressing()&&!Controller1.ButtonB.pressing()&&!Controller1.ButtonDown.pressing()){
        ball_ctrl=1;
      }
      intake_ctrl=0;
    }
    if(Controller1.ButtonX.pressing()){
      Up.set(true);
      //Brain.Timer.reset();
      mode_ctrl=2;
    }
    else if(Controller1.ButtonB.pressing()&&(!Controller1.ButtonR2.pressing())){
      ball_ctrl=0;
      shoot_ctrl=-1;
      Intake(-100);
      //Intake(-50); //技能赛
      Ball(-100);
      Shoot(-100); //未测试，目的：使球更快下去
    }
    else if(Controller1.ButtonDown.pressing()){
      Up.set(false);
      shoot_ctrl=-1;
      ball_ctrl=0;
      Ball_Intake();
      Shoot(50);
      Brain.Timer.clear();
      mode_ctrl=1;
    }
    if(Controller1.ButtonR2.pressing()){
        shoot_ctrl=-1;
        ball_ctrl=0;
        Ball_Intake();
        Shoot(100);          
    }
    else{
      if((!Controller1.ButtonR1.pressing())&&(!Controller1.ButtonB.pressing())&&(!Controller1.ButtonDown.pressing())){
        ball_ctrl=1;
      }
      shoot_ctrl=1;
    }
    if(auto_control==1){
      break;
    }
    wait(5, msec);
  }
}
void Up_ctrl(){
  while(true){
    if(Brain.timer(timeUnits::msec)>500&&mode_ctrl==1){
      Up.set(true);
    }
    if(auto_control==1){
      break;
    }
    wait(5, msec);
  }
}
void Ball_ctrl(){
  while(true){
    if(ball_ctrl==1){
      Ball(0);
    }
    else if(ball_ctrl==-1){
      Ball(0);
    }
    if(auto_control==1){
      break;
    }
    wait(5, msec);
  }
}
void choose_ctrl(){
  while(true){
    if (Controller1.ButtonUp.pressing()){
      /*Auto=0;
      button=0;
      //Side=0;
      Alliance=0; 
      Brain.Screen.clearScreen();

      Controller1.Screen.clearLine(1);
      Controller1.Screen.clearLine(2);
      Controller1.Screen.clearLine(3);
      Brain.Screen.drawRectangle(20,20,200,200,color::red);
      Brain.Screen.drawRectangle(260,20,200,200,color::blue);
      Choose=1;*/

      //if(button==0){
        Alliance=0;
      //}

      //Auto_Ctrl();
      /*if (Choose==1)
      {
      Auto_Ctrl();
      }  */
    }
    if(Controller1.ButtonLeft.pressing()){
      Alliance=1; 
      // Controller1.Screen.setCursor(2,1);
      // Controller1.Screen.print ("红球");
    }
    if(Controller1.ButtonRight.pressing()){
      Alliance=-1; 
      // Controller1.Screen.setCursor(2,1);
      // Controller1.Screen.print ("蓝球");
    }
    if(auto_control==1){
      break;
    }
    wait(20, msec);
  }
}
void hook_auto_ctrl(){
  while(true){
    if(Controller1.ButtonA.pressing()){
      // 暂停底盘控制
      auto_stop = true;
      RunStop(brake);
      // 初始化陀螺仪角度基准
      now = 0;  // 相对角度基准
      Start = Gyro.rotation(degrees);  // 当前绝对角度作为起点
      
      // 执行一键钩子动作
      Run_gyro(70,30,now);
      Turn(50);
      wait(400);
      now=0;
      Start=Gyro.rotation(degrees);
      Run_gyro(200,-30,now);
      Turn(-50);
      wait(300);
      Turn(0);
      RunStop(coast);
      // 恢复底盘控制
      auto_stop = false;
      
      // 等待按键释放
      while(Controller1.ButtonA.pressing()){
        wait(20, msec);
      }
    }
    if(auto_control==1){
      break;
    }
    wait(20, msec);
  }
}
void Joystick(void){
  thread base_control_thread=thread(base_control);
  thread hook_control_thread=thread(hook_control);
  thread hold_control_thread=thread(hold_ctrl);
  thread choose_control_thread=thread(choose_ctrl);
  thread load_control_thread=thread(load_ctrl);
  thread Intake_Shoot_control_thread=thread(Intake_Shoot_ctrl);
  thread ball_control_thread=thread(Ball_ctrl);
  thread up_control_thread=thread(Up_ctrl);
  thread hook_auto_control_thread=thread(hook_auto_ctrl);
  Color.setLightPower(100, percent); 
  Color_2.setLightPower(100, percent);
  Color_3.setLightPower(100, percent);
  /*while(true){
    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(1,1);
    Brain.Screen.print(Distance1.objectDistance(distanceUnits::mm));
    Brain.Screen.print(" ");
    Brain.Screen.print(Distance2.objectDistance(distanceUnits::mm));
    wait(500, msec);
  }*/
}


/*if (Controller1.ButtonR1.pressing())
{
  Up.set(false);
  //task::stop (BallTask);
  //Intake(100);//开启吸球
  ball_ctrl=2;//(分球电机自由控制)
  color_ctrl=1;//(启用传感器)

}
else if (Controller1.ButtonR2.pressing())
{
 Intake(-100);//开启吐球
 ball_ctrl=-1;//分球电机停转
 color_ctrl=0;//禁用传感器
}
else if (intake_ctrl!=0)
{
Intake(100*intake_ctrl);//开启吐球
}
else 
{
  Intake(0);
  ball_ctrl=0;//分球停转
  color_ctrl=0;//不启用传感器
}


if(Controller1.ButtonX.pressing())//吐球打高桥
{
  Up.set(true);
  Shoot(100);
  if (color_ctrl!=1)
  { 
    ball_ctrl=1;//(分球电机自由控制)}
  }
  intake_ctrl=1;
}
else if(Controller1.ButtonA.pressing())//吐球打高桥
{
  Up.set(false);
  Shoot(100);
  if (color_ctrl!=1)
  { 
    ball_ctrl=1;//(分球电机自由控制)}
  }
  intake_ctrl=1;
}  
else if(Controller1.ButtonB.pressing())//吐球打高桥  
{
Shoot(100);
Up.set(false);
if (color_ctrl!=1)
  { 
    ball_ctrl=1;//(分球电机自由控制)}
  }
intake_ctrl=-1;
//Intake(-100);//开启吐球
}
else if (color_ctrl==1)
{
  Shoot(-100);
}
else 
{
  intake_ctrl=0;
  Shoot(0);
}
if (color_ctrl==1)
{
  Color.setLightPower(100, percent); 
  Color_2.setLightPower(100, percent);
  Ball_Intake();
}
  else
{Color.setLightPower(0, percent); 
  Color_2.setLightPower(0, percent);}*/
///////////////////////////////////




////自动遥控器控制////
//task::stop (AutoTask);
/*if (Controller1.ButtonDown.pressing()&&button!=1)
{    test();}*/

 //////遥控器按键/////////////////////


//////////////////////////////////






  /*
  if(abs(Controller1.Axis1.position())>60)
  {
   Ball(Controller1.Axis1.value());//分球电机摇杆控制
   ball_ctrl=2;
   color_ctrl=-1;//禁用传感器
  }
else if(ball_ctrl==1)
  {Ball(100);}
else if(ball_ctrl==-1)
  {Ball(-100);}
// else if(ball_ctrl==5)
//   {Ball(50);}
  else if (color_ctrl!=1)//(启用传感器)
  { 
    Ball(0);
  }



  */

///////////////////////////////////


  

 /*
 if(Controller1.Axis2.position()>60)
{
  Wing(0);
}
else if(Controller1.Axis2.position()<-60)
{
  Wing(1);
}
 */

// else 
//  {
//   Wing(0);
//  }

// ///遥控器选择自动程序///




