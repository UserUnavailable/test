now=-16;
Start=Gyro.rotation(degrees)-now*Side;
Wing_L.set(true);
Get_Ball(2);
Run_gyro_new(330);
wait(100);
Load.set(true);
wait(150);
Run_gyro_new(640, -125);
Turn_Gyro(-178); 
wait(20,msec);
Run_wall(30,1300,10);
Run_gyro_new(-520, -180);
Load.set(false);
Get_Ball(1);    
Run_wall(-5,2000,10);
wait(400);
Get_Ball(0);
Run_gyro_new(250, -221);
Wing_L.set(false);
Turn_Gyro(-178);
Run_gyro(-520, 100, -178);
Turn_Gyro(-133);


