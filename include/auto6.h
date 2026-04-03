//右先高再低
now=90;
Start=Gyro.rotation(degrees)-now*Side;
Wing_L.set(true);
Get_Ball(2);
Load.set(true);
run_gyro_JAR(540);
Turn_Gyro_new(180);
Run_wall(30,1200,5);
run_gyro_JAR(-520, 180);
Get_Ball(1);    
Run_wall(-20,1200,5);
Load.set(false);
wait(200);
Get_Ball(2);
turn_side_JAR(-40, left);
run_gyro_JAR(440, -40);
Load.set(true);
wait(300);
Load.set(false);
Get_Ball(-1);
run_gyro_JAR(280, -45); 
wait(800);
Get_Ball(0);
run_gyro_JAR(-690, -60);
// //turn_gyro_new(180);
turn_side_JAR(0, left);
Wing_L.set(false);
run_gyro_JAR(400, 0);
Turn_Gyro_new(-20);
RunStop(hold);


