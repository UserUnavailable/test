////后退打低杆，向前吸三球，转向吸高杆下球，后退转向前进打高杆
//Run_gyro(距离、功率、角度)
//thread ColorThread=thread(Color_Control);

//左侧后打中杆
now=0;
Start=Gyro.rotation(degrees)-now*Side;

if(Alliance==-1){
  //Run_gyro(350,60,now);
  Run_gyro(350);
  JoyStop();
  wait(200);
  Turn_Gyro(-90);
  hold_stop(100);
  Load.set(true);
  Get_Ball(2);
  Run_time(50,300);
  wait(100);
  Wing_L.set(true);
  Run_time(50,200);
  Wing_L.set(false);
  wait(100);
  Run_time(60,200);
  Wing_L.set(true);
  wait(200);
  Wing_L.set(false);
  wait(350);
  Turn_Gyro(-92);
  //Run_gyro(380,-80,now,false);
  Run_gyro(-380);
  Run_time(-20,300);
  Load.set(false);
  Up.set(true);
  Get_Ball(1);
  auto_color_ctrl=1;
  //wait(1000);
  wait(300);
  Turn_Gyro(now);
  Run_time(20,50);
  wait(1330);  //wait(1530);
  Get_Ball(0);
  //Run_gyro(190,80,now); //
  Run_gyro(190); //
  JoyStop();
  wait(100);
  Turn_Gyro(-225);
  hold_stop(100);
  Up.set(false);
  //wait(10000);
  Get_Ball(2);
  //Run_gyro(550,60,now);
  Run_gyro(550);


  
  Load.set(true);
  //Run_gyro(100,30,now);
  Run_gyro(100);
  JoyStop();
  wait(200);
  Turn_Gyro(-40);
  //Run_gyro(100,-50,now,false);
  Run_gyro(-100);
  Intake(100);
  Ball(100);
  Shoot(70);

  //Run_gyro(120,-50,now,false);
  Run_gyro(-120);
  JoyStop();
  wait(100);
  Run_time(10,200);
  Shoot(100);
  hold_stop(1350);         //wait(1950);
  Get_Ball(0);
  //Run_gyro(80,20,now);
  Run_gyro(80);

  Up.set(true);
  //wait(100);

  Load.set(false);



  //Get_Ball(0);
  //wait(200);
  //Run_gyro(80,-60,now,false);
  Run_gyro(-80);
  JoyStop();
  wait(100);


  Get_Ball(2);
  //Run_gyro(470,80,now);
  Run_gyro(470);
  Turn_Gyro(-90);
  hold_stop(100);
  //Run_gyro(70,-60,now,false);
  Run_gyro(-70);
  //Run_gyro(100,-60,now+5,false);
  Run_gyro(-100, now+5);
  //Run_gyro(110,-60,now-5,false);
  Run_gyro(-110, now-5);
  JoyStop();
  Turn_Gyro(-80);
  RunStop(hold);
}
else{
  if(Alliance==1){
    //Run_gyro(350,60,now);
    Run_gyro(350);
    JoyStop();
    wait(200);
    Turn_Gyro(-90);
    hold_stop(100);
    Load.set(true);
    Get_Ball(2);
    Run_time(50,300);
    wait(100);
    Wing_L.set(true);
    Run_time(50,200);
    Wing_L.set(false);
    wait(100);
    Run_time(60,200);
    Wing_L.set(true);
    wait(200);
    Wing_L.set(false);
    wait(350);
    Turn_Gyro(-92);
    //Run_gyro(380,-80,now,false);
    Run_gyro(-380);
    Run_time(-20,300);
    Load.set(false);
    Up.set(true);
    Get_Ball(1);
    auto_color_ctrl=1;
    //wait(1000);
    wait(300);
    Turn_Gyro(now);
    Run_time(20,50);
    wait(1330); //wait(1530);
    Get_Ball(0);
    //Run_gyro(190,80,now); //
    Run_gyro(190); //
    JoyStop();
    wait(100);
    Turn_Gyro(-225);
    hold_stop(100);
    Up.set(false);
    //wait(10000);
    Get_Ball(2);
    //Run_gyro(550,60,now);
    Run_gyro(550);
      

    
    Load.set(true);
    //Run_gyro(100,30,now);
    Run_gyro(100);
    JoyStop();
    wait(200);
    Turn_Gyro(-40);
    //Run_gyro(100,-50,now,false);
    Run_gyro(-100);
    Intake(100);
    Ball(100);
    Shoot(70);

    //Run_gyro(120,-50,now,false);
    Run_gyro(-120);
    JoyStop();
    wait(100);
    Run_time(10,200);
    Shoot(100);
    hold_stop(1350);         //wait(1950);
    Get_Ball(0);
    //Run_gyro(80,20,now);
    Run_gyro(80);

    Up.set(true);
    //wait(100);

    Load.set(false);



    //Get_Ball(0);
    //wait(200);
    //Run_gyro(80,-60,now,false);
    Run_gyro(-80);
    JoyStop();
    wait(100);


    Get_Ball(2);
    //Run_gyro(470,80,now);
    Run_gyro(470);
    Turn_Gyro(-90);
    hold_stop(100);
    //Run_gyro(70,-60,now,false);
    Run_gyro(-70);
    //Run_gyro(100,-60,now+5,false);
    Run_gyro(-100, now+5);
    //Run_gyro(110,-60,now-5,false);
    Run_gyro(-110, now-5);
    JoyStop();
    Turn_Gyro(-80);
    RunStop(hold);
  }
}

/*  
    Load.set(true);
    Up.set(false);
    Run_gyro(100,30,now);
    JoyStop();
    wait(200);
    Turn_Gyro(-40);
    Run_gyro(120,-50,now,false);
    Intake(100);
    Ball(100);
    Shoot(70);
    Run_gyro(100,-50,now,false);
    wait(1950);
    Load.set(false);
    Get_Ball(0);
    wait(200);
    Get_Ball(2);
    Run_gyro(440,80,now);
    Turn_Gyro(-90);
    hold_stop(100);
    Run_gyro(100,-60,now,false);
    Run_gyro(100,-60,now+5,false);
    Run_gyro(110,-60,now-5,false);
    JoyStop();
    Turn_Gyro(-80);
    RunStop(hold);*/




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

