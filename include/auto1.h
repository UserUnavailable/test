now=-14;
Start=Gyro.rotation(degrees)-now*Side;
Wing_L.set(true);
Get_Ball(2);
Run_gyro_new(470);
wait(50);
Load.set(true);
Up.set(true);
Turn_Gyro(-135);
Run_gyro_new(-180);
Get_Ball(1);
wait(1000);
Load.set(false);
Get_Ball(2);
Run_gyro_new(50);
Run_gyro_new(348, -45);
Run_gyro_new(160, -90);
wait(50);
Load.set(true);
Run_wall(10,300,10);  

Run_gyro_new(-500, 1);
/*
Load.set(false);
Run_wall(20,500,10);
*/

/*
Get_Ball(2);
Run_gyro_new(870);
Up.set(true);
Turn_Gyro(-180);
//wait(20,msec);
Run_wall(30,1300,10);
Run_gyro_new(-520);
Load.set(false);
Get_Ball(1);
Run_wall(-5,1000,10);
wait(400);
Get_Ball(0);
Hook();
*/
