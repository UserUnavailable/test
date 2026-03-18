//右先高再低
now=90;
Start=Gyro.rotation(degrees)-now*Side;
Wing_L.set(true);
Get_Ball(2);
Load.set(true);
Run_gyro_new(470);
Turn_Gyro(178);
Run_wall(30,1200,5);
Run_gyro_new(-520, 180);
Get_Ball(1);    
Run_wall(-5,1000,5);
Load.set(false);
wait(200);
Get_Ball(2);
Run_gyro_new(80);
Turn_Gyro(302);
Run_gyro_new(550);
wait(50);
Run_gyro_new(340, 315);
Get_Ball(-1);
wait(1200);
Get_Ball(2);
Run_gyro_new(-510, 305);
Turn_Gyro(360);
Wing_L.set(false);
Run_gyro(280, 100, 360);
Turn_Gyro(-130);

