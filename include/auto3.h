

//9球低杆
now=14;
Start=Gyro.rotation(degrees)-now*Side;
Wing_L.set(true);
Get_Ball(2);
Run_gyro_new(350);
wait(100);
Run_gyro_new(452, 38);
Get_Ball(0);
Run_gyro_new(130, 90);
Get_Ball(2);
Load.set(true);
Run_wall(30,200,10);
Run_gyro_new(-430, 45);
Get_Ball(0);
Turn_Gyro(-45);
Load.set(false);
Run_gyro_new(260, -45);
Get_Ball(-1);
wait(2000);
Get_Ball(2);
Load.set(true);
Run_gyro_new(-940, -40);
Turn_Gyro(-178);
wait(50);
Run_wall(30,1000,10);
Run_gyro_new(-520, -180);
Load.set(false);
Get_Ball(1);    
Run_wall(-5,1000,10);
wait(200);
Get_Ball(0);
Run_gyro_new(250, -221);
Wing_L.set(false);
Turn_Gyro(-178);
Run_gyro(-520, 100, -178);
Turn_Gyro(-133);
/*
now=14;
Start=Gyro.rotation(degrees)-now*Side;
Wing_L.set(true);
Get_Ball(2);
Run_gyro_new(350);
wait(100);
Run_gyro_new(452, 38);
Run_gyro_new(130, 90);
Load.set(true);
Run_wall(30,200,10);
Run_gyro_new(-650, 0);
Run_gyro_new(-290, -45);
Turn_Gyro(-178);
wait(50);
Run_wall(30,1000,5);
Run_gyro_new(-520, -180);
Load.set(false);
Get_Ball(1);    
Run_wall(-5,2400,5);
Load.set(false);
wait(200);
Get_Ball(0);
Run_gyro_new(250, -221);
Wing_L.set(false);
Turn_Gyro(-178);
Run_gyro(-520, 100, -178);
Turn_Gyro(-133);
*/

