void X1_A_B_C_E(int dir)//出发吸面前3球和中线球，退到中桥前
{
  Side=dir;
  now=0; //初始摆位角度
  Start=Gyro.rotation(degrees)-now*Side;
  //  Run_gyro(50,50,now);//前进到球堆
  Run_gyro(50);//前进到球堆
  Turn_Gyro(-18);//转向面对堆
  Get_Ball(2);///0：停；1：中高桥；-1：低桥；2：吸球
  //  Run_gyro(250,60,now);//前进到球堆
  Run_gyro(250);//前进到球堆
  Load.set(true);
  //  Run_gyro(100,60,now);//前进到球堆
  Run_gyro(100);//前进到球堆
  Turn_Gyro(-65);//转向面对堆
  Load.set(false);
  //  Run_gyro(200,60,now);//前进到球堆
  Run_gyro(200);//前进到球堆
  //  Run_gyro(100,40,now);//
  Run_gyro(100);//
  Load.set(true);
  Run_time(30,100);
  wait(100);
  //  Run_gyro(260,-60,now);// 
  Run_gyro(-260);// 
  Turn_Gyro(-135);//转向面对长桥
  Get_Ball(1);//0：停；1：中高桥；-1：低桥；2：吸球
  if (Auto==2)
  {Get_Ball(0);
  Intake(-100);}
  //  Run_gyro(100,-60,now);//前进到长桥附近
  Run_gyro(-100);//前进到长桥附近
  Run_time(-30,200);
  Turn_Gyro(-135);//转向面对中桥
  Run_time(-30,100);
  Ball(0);
  Turn_Gyro(-135);//转向面对中桥
  Run_time(-30,100);
  Get_Ball(2);//0：停；1：中高桥；-1：低桥；2：吸球
  // Turn_Gyro(45);//转向面对中桥
  // Run_time(-30,100);
  wait(100);
  Turn_Gyro(-135);//转向面对中桥
  Load.set(false); 
  //  Run_gyro(100,60,now);//前进到长桥附近
  Run_gyro(100);//前进到长桥附近
  //  Run_gyro(500,80,now);//前进到长桥附近
  Run_gyro(500);//前进到长桥附近
  //  Run_gyro(200,60,now);//前进到长桥附近
  Run_gyro(200);//前进到长桥附近
}
void X1_C(int dir)//出发吸面前3球和中线球，退到中桥前
{
  Side=dir;
  now=0; //初始摆位角度
  Start=Gyro.rotation(degrees)-now*Side;
  Turn_Gyro(-3);//转向面对中桥-7+Auto*2
  //  Run_gyro(470,-70,now);//后退到中桥
  Run_gyro(-470);//后退到中桥
  //Run_time(-30,100);
  Up.set(false);
  Get_Ball(Side);//0：停；1：中高桥；-1：低桥；2：吸球
  Turn_Gyro(45);//转向面对中桥
  Run_time(-30,100);
  Turn_Gyro(45);//转向面对中桥
  Run_time(-30,100);
  wait(100);
  Get_Ball(2);///0：停；1：中高桥；-1：低桥；2：吸球
}
void C_A()//出发吸面前3球和中线球，退到中桥前
{
  //  Run_gyro(50,50,now);//前进到球堆
  Run_gyro(50);//前进到球堆
  Load.set(true);//收起填装杆
  //  Run_gyro(150,60,now);//前进到球堆
  Run_gyro(150);//前进到球堆
  wait(100);
  //  Run_gyro(40,-30,now);//
  Run_gyro(-40);//
  Load.set(false);//收起填装杆
  Turn_Gyro(110);//转向面对桥下球
  //  Run_gyro(450,60,now);//前进到球堆
  Run_gyro(450);//前进到球堆
  Load.set(true);//放下填装杆
  Run_wall(50,300,2);
  wait(100);
  //  Run_gyro(300,-60,now);//
  Run_gyro(-300);//
  Turn_Gyro(50);//转向面对长桥
  Load.set(false);
  //  Run_gyro(600,60,now);//前进到长桥附近
  Run_gyro(600);//前进到长桥附近
}
///////////////////////////////////////////////////////////////
void A_E()//从中桥到填装区到长桥
{
  Load.set(false);//收起填装杆
  Turn_Gyro(115);//转向面对桥下球
  //  Run_gyro(450,60,now);//前进到球堆
  Run_gyro(450);//前进到球堆
  Load.set(true);//放下填装杆
  Run_wall(30,200,2);
  wait(100);
  //  Run_gyro(250,-60,now);//
  Run_gyro(-250);//
  Turn_Gyro(50);//转向面对长桥
  Load.set(false);
  //  Run_gyro(450,60,now);//前进到长桥附近
  Run_gyro(450);//前进到长桥附近

}
void E_F_E()//从中桥到填装区到长桥
{
  //Run_gyro(50,60,now);//前进到长桥附近
  wait(100);
  Turn_Gyro(-90);//转向面对长桥
  //  Run_gyro(250,60,now);
  Run_gyro(250);
  mode=1;
  BallTask=task(Intake_Color);
  Run_time(30,200); 
  wait(100);
  //  Run_gyro(170,-60,now);
  Run_gyro(-170);


}
void E_D()//从中桥到填装区到长桥
{
  wait(100);
  Turn_Gyro(0);//转向面对长桥
  //  Run_gyro(200,-60,now);//后退到长桥
  Run_gyro(-200);//后退到长桥
  //task::stop (BallTask);
  Run_time(-40,200);
  Up.set(true);
  Shoot(100);
  for (int i=0;i<=3;i++)
  {
  Turn_Gyro(0);
  Run(-30);
  wait(300);
  Run(0);
  wait(100);
  }
  Up.set(false);
  Get_Ball(2);//0：停；1：中高桥；-1：低桥；2：吸球
  Run_time(-30,100);
  Turn_Gyro(0);
  mode=1;
  BallTask=task(Intake_Color);
  //
  //  Run_gyro(100,50,now);
  Run_gyro(100);

  Load.set(true);
  //  Run_gyro(250,60,now);
  Run_gyro(250);
  Run_time(50,200); 
  for (int i=0;i<=3;i++)
  {  
     Run_time(30,100);
     wait(100);
      }
  wait(100);
  //  Run_gyro(350,-60,now);
  Run_gyro(-350);
  Up.set(true);
  Run_time(-50,200);
  Load.set(false);
  //
  Shoot(100);
    for (int i=0;i<=4;i++)
  {
  Turn_Gyro(0);
  Run(-30);
  wait(350);
  Run(0);
  wait(50);
  }
  Run(-30);
  task::stop (BallTask);
  Get_Ball(1);//0：停；1：中高桥；-1：低桥；2：吸球
  while (Brain.timer(timeUnits::sec)<=14.8){}
  Up.set(false);
  wait(200);
}
/////////////////////////////////////////////////////////////////