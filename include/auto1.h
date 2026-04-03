
//左9球中杆
/*now=-14;
Start=Gyro.rotation(degrees)-now*Side;
Up.set(true);
Wing_L.set(true);
Get_Ball(2);
Run_gyro_new(330);
wait(100);
Run_gyro_new(457, -38);
Run_gyro_new(140, -90);
Load.set(true);
Run_wall(30,200,5);
Run_gyro_new(-460, -45);
Get_Ball(0);
Run_gyro_new(-230, -135);
Get_Ball(1);
wait(2000);
Get_Ball(2);
Run_gyro_new(950, -140);
Turn_Gyro_new(-180);
Up.set(false);
Run_wall(30,1000,5);
Run_gyro_new(-520, -180);
Load.set(false);
Get_Ball(1);    
Run_wall(-5,1000,5);
wait(200);
Get_Ball(0);
Run_gyro_new(250, -221);
Wing_L.set(false);
Turn_Gyro_new(-178);
Run_gyro(-520, 100, -178, false);
Turn_Gyro_new(-133);
*/

now=-16;
Start=Gyro.rotation(degrees)-now*Side;
Up.set(true);
Get_Ball(2);
run_gyro_JAR(330);
wait(100);
run_gyro_JAR(495, -90);
Run_wall(30,600,5);
Load.set(true);
wait(200);
run_gyro_JAR(-328, -50);
turn_side_JAR(-135, left);
Get_Ball(1);
wait(1200);
Get_Ball(2);
run_gyro_JAR(900, -140);
Up.set(false);
Turn_Gyro_new(-180);
Run_wall(30,1200,5);
run_gyro_JAR(-520);
Get_Ball(1);    
Run_wall(-20,1000,5);
wait(200);
Get_Ball(0);
Load.set(false);
hook();




