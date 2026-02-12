/*X1_A_B_C_E(1);
E_D();*/
////后退打低杆，向前吸三球，转向吸高杆下球，后退转向前进打高杆
//Run_gyro(距离、功率、角度)
//thread ColorThread=thread(Color_Control);

//技能赛自动   面向左上角贴停车区远侧边出发
now=0;
Start=Gyro.rotation(degrees)-now*Side;
FAuto_Get_Ball(2);
Up.set(true);
Run_time(60,500);       //己方停车区
Load.set(true);
wait(500);
Run_time(60,800);
Load.set(false);
Turn_Gyro(90);
//Run_gyro(150,40,now);
Run_gyro(150);
FAuto_Get_Ball(0);
Turn_Gyro(0);
FAuto_Run_gyro(Load_Dis,60,now);
JoyStop();
wait(100);
//hold_stop(100);
Turn_Gyro(-90);
Load.set(true);
FAuto_Get_Ball(2);
//Run_gyro(50,50,now);
Run_gyro(50);
Run_time(50,300);
wait(100);
Run_time(50,200);
wait(1500);
Turn_Gyro(-92);
//Run_gyro(440,-50,now);        //第一个高杆
Run_gyro(-440);        //第一个高杆
Run_time(-20,300);
Load.set(false);
FAuto_Get_Ball(1);
wait(2300);
FAuto_Get_Ball(0);
//Run_gyro(80,30,now);
Run_gyro(80);
//Up.set(false);
Turn_Gyro(0);
Wall_Stop(40,1000);
//Run_gyro(50,-30,now);
Run_gyro(-50);
JoyStop();
wait(100);
Turn_Gyro(90);
hold_stop(100);
FAuto_Run_gyro(Load_Dis,60,now);
JoyStop();
wait(100);
Turn_Gyro(0);
hold_stop(100);
FAuto_Run_gyro(Load_Dis,-40,now,true);
JoyStop();
wait(100);
Turn_Gyro(90);
hold_stop(100);
Load.set(true);
FAuto_Get_Ball(2);
//Run_gyro(50,50,now);
Run_gyro(50);
Run_time(50,300);
wait(100);
Run_time(60,200);
wait(1500);
Turn_Gyro(92);
//Run_gyro(440,-50,now);      //第二个高杆
Run_gyro(-440);      //第二个高杆
Run_time(-20,300);
Load.set(false);
FAuto_Get_Ball(1);
wait(2300);
FAuto_Get_Ball(0);
//Run_gyro(80,30,now);
Run_gyro(80);
//Up.set(false);
Turn_Gyro(180);
hold_stop(100);
//Run_gyro(150,50,now);
Run_gyro(150);
JoyStop();
wait(100);
Turn_Gyro(90);
hold_stop(100);
Wall_Stop(50,1000);
//Run_gyro(50,-30,now);
Run_gyro(-50);
JoyStop();
wait(100);
Turn_Gyro(180);
hold_stop(100);
FAuto_Get_Ball(2);
Run_time(60,800);   //对面停车区
Load.set(true);
wait(500);
Run_time(60,800);
Load.set(false);
Turn_Gyro(270);
hold_stop(100);
//Run_gyro(150,50,now);
Run_gyro(150);
JoyStop();
wait(100);
Turn_Gyro(180);
hold_stop(100);
FAuto_Run_gyro(Load_Dis,60,now);
JoyStop();
wait(100);
Turn_Gyro(90);
hold_stop(100);
Load.set(true);
FAuto_Get_Ball(2);
//Run_gyro(50,50,now);        
Run_gyro(50);
Run_time(50,300);
wait(100);
Run_time(60,200);
wait(1500);
Turn_Gyro(88);
//Run_gyro(440,-50,now);      //第三个高杆
Run_gyro(-440);      //第三个高杆
Run_time(-20,300);
Load.set(false);
FAuto_Get_Ball(1);
wait(2300);
FAuto_Get_Ball(0);
//Run_gyro(80,30,now);
Run_gyro(80);
//Up.set(false);
Turn_Gyro(180);
Wall_Stop(40,1000);
//Run_gyro(50,-30,now);
Run_gyro(-50);
JoyStop();
wait(100);
Turn_Gyro(270);
hold_stop(100);
FAuto_Run_gyro(Load_Dis,60,now);
JoyStop();
wait(100);
Turn_Gyro(180);
hold_stop(100);
FAuto_Run_gyro(Load_Dis,-40,now,true);
JoyStop();
wait(100);
Turn_Gyro(270);
hold_stop(100);
Load.set(true);
FAuto_Get_Ball(2);
//Run_gyro(50,50,now);          
Run_gyro(50);
Run_time(50,300);
wait(100);
Run_time(60,200);
wait(1500);
Turn_Gyro(272);
//Run_gyro(440,-50,now);        //第四个高杆
Run_gyro(-440);        //第四个高杆
Run_time(-20,300);
Load.set(false);
FAuto_Get_Ball(1);
wait(2300);
FAuto_Get_Ball(0);
//Run_gyro(80,40,now);
Run_gyro(80);
//Up.set(false);
Turn_Gyro(360);
hold_stop(100);
//Run_gyro(400,60,now);     //停车
Run_gyro(400);     //停车
JoyStop();
wait(100);
Turn_Gyro(270);
hold_stop(100);
Run_time(80,1000);
RunStop(hold);



//auto1大运备份
/*now=0;
Start=Gyro.rotation(degrees)-now*Side;

if(Alliance==-1){

  
  Run_gyro(240,-80,now);
  Run_gyro(180,-40,now);
  JoyStop();
  wait(300);
  Intake(100);
  Shoot(100);
  Wing_L.set(true);
  wait(200);
  Wing_L.set(false);
  wait(300);
  Intake(0);//吐完低杆
  Shoot(0);
  Run_gyro(40,20,now);
  
  Turn_Gyro(70);//正对三球
  
  


  //wait(20000);
  Get_Ball(2);//0：停；1：中高桥；-1：低桥；2：吸球
  Run_gyro(180,60,now);
  Run_gyro(50,30,now);
  JoyStop();
  wait(100);
  Load.set(true);
  wait(200);
  Turn_Gyro(25);
  // Run_time(-30,400);
  Get_Ball(1);
  Run_gyro(150,-30,now);
  wait(800);
  Get_Ball(0);
  wait(200);
  Get_Ball(2);
  Load.set(false);
  //wait(20000);
  Run_gyro(590,60,now);
  Run_gyro(150,30,now);
  Turn_Gyro(-23);
  Load.set(true);
  Get_Ball(2);
  //wait(20000);
  Run_time(50,300);
  wait(100);
  Run_time(50,200);
  wait(100);
  Run_time(50,200);
  wait(270);
  wait(150);
  Run_gyro(440,-50,now);
  Load.set(false);
  //wait(20000);
  Up.set(true);
  Get_Ball(1);
  wait(2830);
  
  Get_Ball(0);
  Run_gyro(70,30,now);
  Up.set(false);
  Turn_Gyro(25);
  Run_gyro(140,-30,now);
  Turn_Gyro(-20);
  JoyStop();
  wait(100);
  Run_gyro(100,-60,now);
  Run_gyro(100,-60,now+5);
  Run_gyro(110,-60,now-5);
  JoyStop();
  Turn_Gyro(0);
}
else{
  if(Alliance==1){
    Run_gyro(240,-80,now);
    Run_gyro(180,-40,now);
    JoyStop();
    wait(300);
    Intake(10);
    Wing_L.set(true);
    wait(200);
    Wing_L.set(false);
    wait(300);
    Intake(0);//吐完低杆
    //wait(20000);
    Run_gyro(40,20,now);
    Turn_Gyro(70);//正对三球
    //wait(20000);
    Get_Ball(2);//0：停；1：中高桥；-1：低桥；2：吸球
    Run_gyro(180,60,now);
    Run_gyro(50,30,now);
    JoyStop();
    wait(100);
    Load.set(true);
    wait(200);
    Turn_Gyro(25);
    // Run_time(-30,400);
    Get_Ball(1);
    Run_gyro(150,-30,now);
    wait(850);
    Get_Ball(0);
    wait(200);
    Get_Ball(2);
    Load.set(false);
    //wait(20000);
    Run_gyro(570,60,now);
    Run_gyro(170,30,now);
    Turn_Gyro(-23);
    Load.set(true);
    Get_Ball(2);
    Run_time(50,300);
    wait(100);
    Run_time(50,200);
    wait(100);
    Run_time(50,200);
    wait(400);
    wait(150);
    Turn_Gyro(-23);
    Run_gyro(450,-50,now);
    Load.set(false);
    //wait(20000);
    Up.set(true);
    Get_Ball(1);
    auto_color_ctrl=1;
    wait(2700);
    Get_Ball(0);
    Up.set(false);
    Run_gyro(70,30,now);
    Turn_Gyro(25);
    Run_gyro(140,-30,now);
    Turn_Gyro(-20);
    JoyStop();
    wait(100);
    Run_gyro(100,-60,now);
    Run_gyro(100,-60,now+5);
    Run_gyro(160,-60,now-5);
  }
}*/


// Ball(10);
// Intake(100);
// Shoot(100);
// Run_time(-20,2000);
// Get_Ball(0);


/*
Turn_Gyro(120);//正对高杆下球
Run_gyro(300,80,now);
Run_gyro(400,-80,now);

Turn_Gyro(75);
Run_gyro(250,80,now);

Turn_Gyro(-15);//正对高杆
Run_time(30,100);
Up.set(true);
Ball(100);
Shoot(100);
Intake(100);
Run_time(20,1000);


  
   
  
  
  
  
  
  
  
  
  
  
  
  
  
  /*X1_C(1);
  C_A();
              //X1_A_B_C_E(1);
  E_F_E();
  E_D();*/

  // now=0; //初始摆位角度
  // Start=Gyro.rotation(degrees)-now*Side;
  // Run_gyro(50,50,now);//前进到球堆
  // mode=1;
  // BallTask=task(Intake_Color);
  // Turn_Gyro(18);//转向面对堆
  // Run_gyro(150,40,now);//前进到球堆
  // Run_gyro(80,40,now);//前进到球堆
  // Load.set(true);
  // Run_gyro(100,40,now);//前进到球堆
  // Turn_Gyro(60);//转向面对堆
  // Load.set(false);
  // Run_gyro(250,40,now);//前进到球堆
  // Load.set(true);
  // Run_gyro(50,30,now);//前进到球堆
  // wait(100);
  // Run_gyro(200,-40,now);//前进到球堆
  // Turn_Gyro(135);//转向面对堆
  // Load.set(false);
  // Run_gyro(400,60,now);//前进到球堆
  // wait(100);
  // Turn_Gyro(180);//转向面对球堆
  // wait(100);
  // Run_gyro(-100,50,now);//吸球前进到中桥
  // Run_time(-50,200);
  //  task::stop (BallTask);
  // Intake(100);
  // Shoot(100);
  // //wait(100);
  // for (int i=0;i<=4;i++)
  // {
  // //Intake(100);
  // Shoot(100);
  // Turn_Gyro(180);
  // Run(-20);
  // wait(350);
  // Shoot(0);//分球桶反转吐球
  // //Intake(0);
  // Run(0);
  // wait(50);
  // }
  // Shoot(-30);
  // wait(100);
  // mode=1;
  // BallTask=task(Intake_Color);
  // Load.set(true);
  // Run_gyro(350,50,now);
  // Run_time(40,200); 
  // //wait(500);
  // for (int i=0;i<=3;i++)
  // {  
  //    Run_time(40,150);
  //    wait(50);
  //     }
  // wait(100);
  // Run_gyro(300,-60,now);
  // Run_time(-50,200);
  //  task::stop (BallTask);
  // Intake(100);
  // Shoot(100);
  //   for (int i=0;i<=4;i++)
  // {
  // Shoot(100);
  // Turn_Gyro(180);
  // Run(-20);
  // wait(350);
  // Shoot(0);//分球桶反转吐球
  // Run(0);
  // wait(50);
  // }
  // Load.set(false);
  // while (Brain.timer(timeUnits::sec)<=11.9){}
  // Shoot(-30);
  // Run_gyro(100,40,now);//前进到长桥
  // Get_Ball();
  // wait(100);
  // Run_time(-100,200);
  // wait(100);
  // Run_time(-30,200);
  // wait(100);
  // Run_gyro(100,40,now);//前进到长桥
  // Get_Ball();
  // Side=1;
  // Turn_Gyro(230);
  // Run_gyro(100,-50,now);//前进到长桥
  // Turn_Gyro(190);
  // Run_gyro(100,-50,185);//前进到长桥
  // Run_gyro(150,-60,180);//前进到长桥
  // RunStop(hold);

