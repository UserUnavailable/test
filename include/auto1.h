// ////后退打低杆，向前吸三球，转向吸高杆下球，后退转向前进打高杆
// //Run_gyro(距离、功率、角度)
// //thread ColorThread=thread(Color_Control);

now=-14;
Start=Gyro.rotation(degrees)-now*Side;
Wing_L.set(true);
Get_Ball(2);
Run_gyro_new(490);
wait(100);
Load.set(true);
wait(150);
Up.set(false);
Turn_Gyro(-135);
Run_gyro_new(-180);
Get_Ball(1);
wait(1000);
Get_Ball(2);
Run_gyro_new(870);
Up.set(true);
Turn_Gyro(-180);
wait(20,msec);
Run_wall(30,1300,10);
Run_gyro_new(-520);
Load.set(false);
Get_Ball(1);
Run_wall(-5,1000,10);
wait(400);
Get_Ball(0);
Hook();


// now=0;
// Start=Gyro.rotation(degrees)-now*Side;

// Run_gyro(350,60,now);
// JoyStop();
// wait(200);
// Turn_Gyro(-90);
// hold_stop(100);
// Load.set(true);
// Get_Ball(2);
// Run_time(50,300);
// wait(100);
// Wing_L.set(true);
// Run_time(50,200);
// Wing_L.set(false);
// wait(100);
// Run_time(60,200);
// Wing_L.set(true);
// wait(200);
// Wing_L.set(false);
// wait(350);
// Turn_Gyro(-92);
// Run_gyro(380,-80,now,false);
// Run_time(-20,300);
// Load.set(false);
// Up.set(true);
// Get_Ball(1);
// auto_color_ctrl=1;
// wait(300);
// Turn_Gyro(now);
// Run_time(20,50);
// wait(1330);
// Get_Ball(0);
// Run_gyro(190,80,now);
// JoyStop();
// wait(100);
// Turn_Gyro(-225);
// hold_stop(100);
// Up.set(false);
// Get_Ball(2);
// Run_gyro(550,60,now);


  
// Load.set(true);
// Run_gyro(100,30,now);
// JoyStop();
// wait(200);
// Turn_Gyro(-40);
// Run_gyro(100,-50,now,false);
// Intake(100);
// Ball(100);
// Shoot(70);

// Run_gyro(120,-50,now,false);
// JoyStop();
// wait(100);
// Run_time(10,200);
// Shoot(100);
// hold_stop(1350);
// Get_Ball(0);
// Run_gyro(80,20,now);

// Up.set(true);

// Load.set(false);



// Run_gyro(80,-60,now,false);
// JoyStop();
// wait(100);


// Get_Ball(2);
// Run_gyro(470,80,now);
// Turn_Gyro(-90);
// hold_stop(100);
// Run_gyro(70,-60,now,false);
// Run_gyro(100,-60,now+5,false);
// Run_gyro(110,-60,now-5,false);
// JoyStop();
// Turn_Gyro(-80);
// RunStop(hold);


