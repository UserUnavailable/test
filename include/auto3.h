  ////X1_A_B_C_E(-1);
  //C_A();
  //E_F_E();
  ////E_D();

  
//左侧先打中杆
now=-14;
Side=1;
Start=Gyro.rotation(degrees)-now*Side;
if(Alliance==-1){ 
  Get_Ball(2);
  Run_gyro(350,50,now);
  RunStop(coast);
  wait(50);
  Load.set(true);
  Up.set(false);
  wait(150);
  Turn_Gyro(-130);
  Run_gyro(70,-40,now);
  Get_Ball(1);
  Run_gyro(100,-30,now);
  wait(1300);
  Load.set(false);
  Get_Ball(0);
  wait(200);
  Get_Ball(2);
  Run_gyro(730,80,now);
  Load.set(true);
  Turn_Gyro(-180);
  Get_Ball(2);
  //wait(20000);
  Run_gyro(50,50,now);
  Run_time(50,300);
  wait(100);
  Wing_L.set(true);
  Run_time(50,200);
  Wing_L.set(false);
  wait(100);
  Run_time(60,200);
  wait(500);
  Up.set(true);
  Turn_Gyro(-182);
  Run_gyro(410,-80,now,false);
  Run_time(-20,300);
  Load.set(false);
  Get_Ball(1);
  auto_color_ctrl=1;
  wait(500);
  Run_time(20,50);
  wait(1550);
  Get_Ball(0);
  Run_gyro(70,30,now);
  //Up.set(false);
  Turn_Gyro(-135);
  Run_gyro(190,-30,now); //210
  Turn_Gyro(-180);
  JoyStop();
  wait(100);
  Run_gyro(100,-60,now,false);
  Run_gyro(100,-60,now+5,false);
  Run_gyro(100,-60,now-5,false);
  Turn_Gyro(-170);
  RunStop(hold);
}
else{
  if(Alliance==1){
    Get_Ball(2);
    Run_gyro(350,50,now);
    RunStop(coast);
    wait(50);
    Load.set(true);
    Up.set(false);
    wait(150);
    Turn_Gyro(-130);
    Run_gyro(70,-40,now);
    Get_Ball(1);
    Run_gyro(100,-30,now);
    wait(1300);
    Load.set(false);
    Get_Ball(0);
    wait(200);
    Get_Ball(2);
    Run_gyro(730,80,now);
    Load.set(true);
    Turn_Gyro(-180);
    Get_Ball(2);
    //wait(20000);
    Run_gyro(50,50,now);
    Run_time(50,300);
    wait(100);
    Wing_L.set(true);
    Run_time(50,200);
    Wing_L.set(false);
    wait(100);
    Run_time(60,200);
    wait(500);
    Up.set(true);
    Turn_Gyro(-182);
    Run_gyro(410,-80,now,false);
    Run_time(-20,300);
    Load.set(false);
    Get_Ball(1);
    auto_color_ctrl=1;
    wait(500);
    Run_time(20,50);
    wait(1550);
    Get_Ball(0);
    Run_gyro(70,30,now);
    //Up.set(false);
    Turn_Gyro(-135);
    Run_gyro(190,-30,now); //210
    Turn_Gyro(-180);
    JoyStop();
    wait(100);
    Run_gyro(100,-60,now,false);
    Run_gyro(100,-60,now+5,false);
    Run_gyro(100,-60,now-5,false);
    Turn_Gyro(-170);
    RunStop(hold);
  }
}


/*
Get_Ball(0);
Run_gyro(600,60,now);
Run_gyro(150,30,now);
Turn_Gyro(-20);
Load.set(true);
Get_Ball(2);
Run_time(50,700);
wait(400);
Run_gyro(450,-30,now);
Load.set(false);
Up.set(true);
Get_Ball(1);
wait(2000);
Get_Ball(0);
Up.set(false);
Run_gyro(70,30,now);
Turn_Gyro(25);
Run_gyro(125,-30,now);
Turn_Gyro(-20);
Run_gyro(400,-60,now);*/




/*//与auto1镜像
#include <cfloat>
now=-10; //初始摆位角度
Start=Gyro.rotation(degrees)-now*Side;
if(Alliance==-1){
  Intake(100);
  Ball(100);
  Run_gyro(200,40,now);
  Run_gyro(110,30,now);
  JoyStop();
  Load.set(true);
  Wing_L.set(true);
  wait(200);
  Wing_L.set(false);
  wait(300);


  //Intake(-100);
  //wait(200);
  //Intake(100);


  //wait(100);
  //Load.set(true);
  Run_gyro(120,30,now);
  JoyStop();
  wait(100);
  //Load.set(true);
  Turn_Gyro(40);//正对桥下
  JoyStop();
  Load.set(false);
  //Load.set(false);
  wait(200);
  //wait(20000);

  Run_gyro(200,60,now);
  Run_gyro(120,30,now);
  Load.set(true);
  wait(300);
  Run_gyro(125,-30,now);
  Turn_Gyro(36);
  JoyStop();
  wait(100);
  Run_gyro(70,-60,now);
  Run_gyro(50,-30,now);
  Load.set(false);
  Turn_Gyro(107);
  JoyStop();
  wait(200);
  Run_gyro(60, -30, now);
  Run_gyro(60, -30, now);
  Intake(-100);
  JoyStop();
  wait(500);
  //wait(20000);
  //wait(20000);
  Run_gyro(610,60,now);
  Run_gyro(100,30,now);
  JoyStop();
  Load.set(true);
  wait(100);
  Turn_Gyro(150);
  JoyStop();
  wait(100);
  //Run_gyro(30, -30, now);
  //JoyStop();
  //wait(100);
  Run_gyro(100,40,now);
  Run_gyro(140,50,now);
  Run(10);
  wait(1200);
  Run_gyro(440,-60,now);
  Load.set(false);
  Up.set(true);
  Get_Ball(1);
  auto_color_ctrl=1;
  while (Brain.timer(timeUnits::sec)<=13.8){
  }
  Get_Ball(2);
  Run_gyro(50,40,now);
  JoyStop();
  wait(100);
  Up.set(false);
  Get_Ball(0);
  Run_time(-70,600);
}
else{
  if(Alliance==1){
    Intake(100);
    Ball(100);
    Run_gyro(200,40,now);
    Run_gyro(110,30,now);
    JoyStop();
    Load.set(true);
    Wing_L.set(true);
    wait(200);
    Wing_L.set(false);
    wait(300);


    //Intake(-100);
    //wait(200);
    //Intake(100);


    //wait(100);
    //Load.set(true);
    Run_gyro(120,30,now);
    JoyStop();
    wait(100);
    //Load.set(true);
    Turn_Gyro(40);//正对桥下
    JoyStop();
    Load.set(false);
    //Load.set(false);
    wait(200);
    //wait(20000);

    Run_gyro(200,60,now);
    Run_gyro(120,30,now);
    Load.set(true);
    wait(300);
    Run_gyro(125,-30,now);
    Turn_Gyro(36);
    JoyStop();
    wait(100);
    Run_gyro(70,-60,now);
    Run_gyro(50,-30,now);
    Load.set(false);
    Turn_Gyro(107);
    JoyStop();
    wait(200);
    Run_gyro(60, -30, now);
    Run_gyro(60, -30, now);
    Intake(-100);
    JoyStop();
    wait(500);
    //wait(20000);
    //wait(20000);
    Run_gyro(610,60,now);
    Run_gyro(100,30,now);
    JoyStop();
    Load.set(true);
    wait(100);
    Turn_Gyro(150);
    JoyStop();
    wait(100);
    //Run_gyro(30, -30, now);
    //JoyStop();
    //wait(100);
    Run_gyro(100,40,now);
    Run_gyro(140,50,now);
    Run(10);
    wait(1200);
    Run_gyro(440,-60,now);
    Load.set(false);
    Up.set(true);
    Get_Ball(1);
    auto_color_ctrl=1;
    while (Brain.timer(timeUnits::sec)<=13.8){
    }
    Get_Ball(2);
    Run_gyro(50,40,now);
    JoyStop();
    wait(100);
    Up.set(false);
    Get_Ball(0);
    Run_time(-70,600);
  }
}*/
/*
  Up.set(false);
  Turn_Gyro(18);//转向面对中
  Run_gyro(500,-70,now);//后退到中桥
  Intake(100);
  Run_time(-30,200);
  wait(100);
  Get_Ball(1);//0：停；1：中高桥；-1：低桥；2：吸球
  Run_gyro(80,40,now);//后退到中桥
  Turn_Gyro(92);//转向面对中桥
  Get_Ball(2);//0：停；1：中高桥；-1：低桥；2：吸球
  Run_gyro(250,60,now);//后退到中桥
  //Load.set(true);//放下填装杆
  Run_gyro(50,50,now);//后退到中桥
  Turn_Gyro(128);//转向面对桥下球
  Load.set(false);//放下填装杆
  Run_gyro(450,60,now);//前进到球堆
  Load.set(true);//放下填装杆
  //Wall_Stop(40,200);
  wait(100);
  Turn_Gyro(117);//转向面对桥下球
  Run_gyro(220,-60,now);// 
  Load.set(false);
  Turn_Gyro(45);//转向面对长桥
  Get_Ball(1);//0：停；1：中高桥；-1：低桥；2：吸球
  Run_gyro(150,-60,now);//前进到长桥附近
  Run_time(-30,200);
  Turn_Gyro(45);//转向面对中桥
  Run_time(-30,100);
  Turn_Gyro(45);//转向面对中桥
  Ball(0);
  Run_time(-30,100);
  Get_Ball(2);//0：停；1：中高桥；-1：低桥；2：吸球
  wait(100);
  Turn_Gyro(45);//转向面对中桥
  Load.set(false); 
  Run_gyro(100,60,now);//前进到长桥附近
  Run_gyro(500,80,now);//前进到长桥附近
  Run_gyro(230,60,now);//前进到长桥附近
   wait(100);
  Turn_Gyro(0);//转向面对长桥
  Run_gyro(200,-60,now);//后退到长桥
  //task::stop (BallTask);
  Run_time(-40,200);
  Up.set(true);
  Shoot(100);
  for (int i=0;i<=2;i++)
  {
  Turn_Gyro(now);
  Run(-30);
  wait(350);
  Run(0);
  wait(50);
  }
  Up.set(false);
  Get_Ball(2);//0：停；1：中高桥；-1：低桥；2：吸球
  Run_time(-30,100);
  Turn_Gyro(now);  //
  Run_gyro(100,50,now);

  Load.set(true);
  Run_gyro(250,60,now);
  Run_time(50,200); 
  for (int i=0;i<=3;i++)
  {  
     Run_time(30,100);
     wait(100);
      }
  Turn_Gyro(now);
  Run_gyro(350,-60,now);
  Up.set(true);
  Run_time(-50,200);
  //
  Shoot(100);
    for (int i=0;i<=3;i++)
  {
  Turn_Gyro(now);
  Run(-30);
  wait(350);
  Run(0);
  wait(50);
  }
  Load.set(false);
  Run(-30);
  task::stop (BallTask);
  Get_Ball(1);//0：停；1：中高桥；-1：低桥；2：吸球
  while (Brain.timer(timeUnits::sec)<=15){}
  // Up.set(false);
  // Load.set(false);
  // wait(200);*/