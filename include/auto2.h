  ////X1_A_B_C_E(-1);
  //C_A();
  //E_F_E();
  ////E_D();
  now=180;
  Start=Gyro.rotation(degrees)-now*Side;
  /*
  now=14;
  
  Wing_L.set(true);
  Get_Ball(2);
  Run_gyro_new(330);
  wait(50);
  Load.set(true);
  wait(150);
  Turn_Gyro(130);
  Run_gyro_new(680);
  Turn_Gyro(175); 
  wait(100,msec);
  Run_wall(40,1200,10);
  Run_gyro_new(-520);
  Load.set(false);
  Get_Ball(1);
  wait(3000);
  Get_Ball(0);
  */
  
  Run_gyro_new(100);
  Turn_Gyro(225);
  Wing_L.set(false);
  Run_gyro_new(-150);
  Run_gyro_new(-450,180);
  Turn_Gyro(200);
  RunStop(hold);

  
//右侧
/*

Load.set(true);
Turn_Gyro(180);
Get_Ball(2);
Run_gyro(50,50,now,false);
Run_time(50,300);
wait(100);

Run_time(50,200);
Wing_L.set(false);
wait(100);
Run_time(60,200);
wait(500);
Turn_Gyro(182);
Run_gyro(410,-80,now,false);
Run_time(-40,300);
RunStop(hold);
Load.set(false);
Up.set(true);
Get_Ball(1);
auto_color_ctrl=1;
wait(500);
Run_time(20,50);
wait(2050);
Get_Ball(0);
RunStop(coast);
Run_gyro(70,30,now);
//Up.set(false);
Turn_Gyro(225);
Run_gyro(200,-30,now);
Turn_Gyro(180);
JoyStop();
wait(100);
Run_gyro(100,-60,now,false);
Run_gyro(100,-60,now+5,false);
Run_gyro(100,-60,now-5,false);
Turn_Gyro(190);
RunStop(hold);

*/
