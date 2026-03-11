// ////后退打低杆，向前吸三球，转向吸高杆下球，后退转向前进打高杆
// //Run_gyro(距离、功率、角度)
// //thread ColorThread=thread(Color_Control);

now=-14;
Start=Gyro.rotation(degrees)-now*Side;
Wing_L.set(true);
Get_Ball(2);
Run_gyro_new(490);
wait(50);
//Load.set(true);
wait(50);
Up.set(false);
Turn_Gyro(-135);
Load.set(true);
Run_gyro_new(-180);
Get_Ball(1);
wait(1000);
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