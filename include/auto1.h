
/*
//左9球中杆
now=-14;
Start=Gyro.rotation(degrees)-now*Side;
Wing_L.set(true);
Get_Ball(2);
Run_gyro_new(330);
wait(100);
Run_gyro_new(440, -38);
Run_gyro_new(140, -90);
Load.set(true);
Run_wall(30,200,5);
Run_gyro_new(-440, -45);
Get_Ball(0);
Run_gyro_new(-230, -135);
Up.set(true);
Get_Ball(1);
wait(2000);
Get_Ball(2);
Run_gyro_new(920, -143);
Turn_Gyro(-178);
Up.set(false);
Run_wall(30,1000,5);
Run_gyro_new(-520, -180);
Load.set(false);
Get_Ball(1);    
Run_wall(-5,1000,5);
wait(200);
Get_Ball(0);
Run_gyro_new(250, -221);
//Run_gyro(250, 80, -221);
Wing_L.set(false);
Turn_Gyro(-180);
Run_gyro(-520, 100, -180);
Turn_Gyro(-133);
*/

/*
//左9球高杆
now=-14;
Start=Gyro.rotation(degrees)-now*Side;
Wing_L.set(true);
Get_Ball(2);
Run_gyro_new(330);
wait(100);
Load.set(false);
Run_gyro_new(440, -38);
Run_gyro_new(140, -90);
Load.set(true);
Run_wall(30,200,5);
Run_gyro_new(-650, 0);
Run_gyro_new(-310, 45);
Turn_Gyro(178);
wait(50);
Run_wall(30,1000,5);
Run_gyro_new(-520, 180);
Get_Ball(1);    
Run_wall(-5,2400,5);
Load.set(false);
wait(200);
Get_Ball(0);
Run_gyro_new(250, 139);
Wing_L.set(false);
Turn_Gyro(180);
Run_gyro(-520, 100, 180);
Turn_Gyro(227);
*/
//先高再中
now=-90;
Start=Gyro.rotation(degrees)-now*Side;
Wing_L.set(true);
Get_Ball(2);
Load.set(true);
Run_gyro_new(500);
Turn_Gyro(-178);
wait(50);
Run_wall(30,1100,5);
Run_gyro_new(-520, -180);
Get_Ball(1);    
Run_wall(-5,1000,5);
Load.set(false);
wait(200);
Get_Ball(2);
Run_gyro_new(80);
Turn_Gyro(-298);
Run_gyro_new(550);
Load.set(true);
Turn_Gyro(-135);
Run_gyro_new(-200);
Up.set(true);
Get_Ball(1);
wait(1200);
Get_Ball(0);
Run_gyro_new(470, -120);
Turn_Gyro(-180);
Wing_L.set(false);
Run_gyro(-250, 100, -180);
Turn_Gyro(-150);