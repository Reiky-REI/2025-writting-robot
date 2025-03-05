// #include <HAL_S05-MD.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <math.h>
// /*******************************************************************************************
//   @ 大然科技
// 	天津市大然科技有限公司-总线舵机库

// 产品型号：大然电机驱动板S05-MD
// 适用平台：stm32单片机
// 库版本号：v1.2
// 测试主控版本：stm32f103c8t6

// *******************************************************************************************/

// uint8_t servo_sdata[10]={0x7b,0x79,0,0,0,0,0x10,0x10,0x10,0x7d};    //发送数组  10bit
// uint8_t servo_rdata[16]={0x7b,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x7d};    //舵机返回值存放数组 16bit

// unsigned char receive_flag = 0;                                 //读取数据状态标志位，接收前清0，收到协议头置1，收到协议尾且总长度为16置2
// uint8_t READ_FLAG=0;                                                //读取状态标志位，每一次读取前自动清零，读取成功置为1，读取失败置为-1

// /**********舵机状态及参数全局变量***************/
// uint8_t state_data[15];                                             //state状态存放数组
// uint8_t state2_data[15];                                            //state2状态存放数组
// uint8_t e2_data[32];                                                //e2参数存放数组


// /*串口数据发送函数
// */
// int write_data(int r_n)                        
// {
// 	// HAL_StatusTypeDef status;
// 	if(r_n!=0)
// 	{
// 		receive_flag = 0;
// 		USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);   //使能串口接收中断。如果需要接收数据，在发送读取指令前打开串口接收中断
// 	}
// 	servo_sdata[0] = 123;  //0x7B,协议头
// 	servo_sdata[9] = 125;  //0x7D，协议尾
// 	servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;  //校验位
// 	usart_senddata(10);
// 	delay_ms(2);           //用来分隔多条指令发送
// }

// /*串口数据接收函数
// */
// void read_data()
// {
// 	int i=0;
// 	READ_FLAG=0;
// 	while(receive_flag!=2&&i<30)
// 	{
// 		delay_ms(20);
// 		i++;
// 	}
// 	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);//关闭串口中断  防干扰
// 	if(receive_flag == 2)
// 	{
// 		if(servo_rdata[0] == 123&&servo_rdata[15] == 125)
// 		{
// 			READ_FLAG=1;
// 		}
// 		else
// 		{
// 			READ_FLAG=-1;
// 		}
// 	}
// 	else
// 	{
// 		READ_FLAG=-1;
// 	}
// 	receive_flag = 0;
// }

// /*单个电机控制函数

//   控制指定电机编号的电机按照指定的速度转动到指定的角度。这里是相对于已经设置了电机的0度初始角度而言的控制电机转动的角度，是为控制电机转动的绝对角度。
    
// 注：设置完毕后，请留出足够运行的时间
// */
// void set_angle(int id_num, float  angle, int speed)  
// {
// 	unsigned char dir;
// 	if(speed<=0)    speed=100;
// 	if(angle<-360)  angle=-360;
// 	if(angle>360)   angle=360;
// 	if (angle<0)
// 	{
// 		 dir = 1;
// 		 angle *= -1;
// 	}
// 	else
// 	{
// 		 dir = 0;
// 	}
// 	servo_sdata[0] = 123;  //0x7B
// 	servo_sdata[1] = id_num;
// 	servo_sdata[2] = (int)((int)(angle)/10);
// 	if (dir == 1)  
// 	{
// 		servo_sdata[2] = servo_sdata[2]| 0x80;
// 	}
// 	servo_sdata[3] = ((int)(angle * 10)) % 100;
// 	servo_sdata[4] = (int)(speed/100);
// 	servo_sdata[5] = (int)(speed% 100);
// 	servo_sdata[7] = 16;  //0x10
// 	servo_sdata[8] = 1;   //0x01
// 	servo_sdata[9] = 125; //0x7D
// 	write_data(0);   //发送控制指令，不返回数据
// 	//启动指令
// 	servo_sdata[7] = 17;
// 	servo_sdata[8] = 16;
// 	write_data(0);   //发送控制指令，不返回数据
// }

// /*单个电机控制函数。

//   控制指定电机编号的电机按照指定的速度相对转动指定角度。这里的角度是相对于电机当前位置而言,对应的是控制电机转动的相对角度。

// 注：设置完毕后，请留出足够运行的时间
// */
// void step_angle(int id_num, float angle, int speed)
// {
// 	unsigned char dir;
// 	int step,circle;
// 	step = (int) abs(speed);
// 	if (speed <= 0)   speed = 100;
//   servo_sdata[0] = 123;  //0x7B
// 	servo_sdata[1] = id_num;
//     if (angle < 0)
// 	{  
// 		dir = 1;
//         angle = angle * -1;
// 	}
//     else
// 	{
//         dir = 0;
// 	}
// 	circle = (int)(angle/360);
// 	servo_sdata[2] = (int)((int)(angle)%360/10);
//     if (dir == 1)  servo_sdata[2] =servo_sdata[2] | 0x80;
//     servo_sdata[3] = ((int)(angle * 10)) % 100;
// 	servo_sdata[4] = (int)(step/100);
// 	servo_sdata[5] = (int)(step% 100);
// 	servo_sdata[7] = 16;  //0x10
// 	servo_sdata[8] = 1+circle;   
// 	servo_sdata[9] = 125; //0x7D
// 	write_data(0);   //发送控制指令，不返回数据
// 	//启动指令
// 	servo_sdata[7] = 17;
// 	servo_sdata[8] = 17;  
// 	write_data(0);   //发送控制指令，不返回数据
// }

// /*多个电机控制函数

//   控制指定电机编号的电机按照指定的速度转动到指定的角度。
// */
// void set_angles(int id_list[20], float angle_list[20], int speed_list[20], int n) 
// {
//     for(int i=0;i<n;i++)
//     {
// 			preset_angle(id_list[i],angle_list[i],speed_list[i]);     //发送角度速度信息
// 		}
//     //多舵机同时开始转动
//   servo_sdata[1]=121; //0x79广播模式
//   servo_sdata[7]=17;  //0x11
// 	servo_sdata[8]=16;  
// 	write_data(0);   //发送控制指令，不返回数据
// }

// /*多个电机控制函数

//   控制指定舵机编号的舵机按照指定的速度相对转动指定角度。
// */
// void step_angles(int id_list[20], float angle_list[20], int speed_list[20], int n) 
// {
// 	for(int i=0;i<n;i++)
//     {
// 			prestep_angle(id_list[i],angle_list[i],speed_list[i]);     //
//     }
//     //多舵机同时开始转动
//   servo_sdata[1]=121; //0x79广播模式
//   servo_sdata[7]=17;  //0x11
// 	servo_sdata[8]=17;  
// 	write_data(0);   //发送控制指令，不返回数据
// }


// /*轮子模式下设置电机的转动速度
// */
// void set_speed(int id_num,int speed,int oc)
// {
// 	servo_sdata[0] = 123;
// 	servo_sdata[1] = id_num;
// 	if(oc == 0)
// 	{
// 		if(speed > 0)
// 		{
// 			servo_sdata[2] = 0x21; 
// 		}	
// 		else if(speed < 0)
// 		{
// 			servo_sdata[2] = 0x22; 
// 			speed = -speed;
// 		}
// 		else
// 		{
// 			servo_sdata[2] = 0x20;
// 		}
// 		servo_sdata[3] = 0;
// 		servo_sdata[4] = (int)(speed / 10);
// 		servo_sdata[5] = (int)((speed) % 10);
// 		servo_sdata[7] = 0x20;
// 		servo_sdata[8] = 1;
// 		servo_sdata[9] = 125;
// 	}
// 	else
// 	{
// 		if(speed > 0)
// 		{
// 			servo_sdata[2] = 0x21; 
// 		}	
// 		else if(speed < 0)
// 		{
// 			servo_sdata[2] = 0x22; 
// 			speed = -speed;
// 		}
// 		else
// 		{
// 			servo_sdata[2] = 0x20;
// 		}
// 		servo_sdata[3] = 0;
// 		servo_sdata[4] = (int)(speed / 100);
// 		servo_sdata[5] = (int)((speed) % 100);
// 		servo_sdata[7] = 0x22;
// 		servo_sdata[8] = 1;
// 		servo_sdata[9] = 125;
// 	}
// 	write_data(0);   //发送控制指令，不返回数据
// }

// /*多个电机速度控制函数
// */
// void set_speeds(int id_list[20], int speed_list[20], int oc, int n)
// {
// 	for(int i=0;i<n;i++)
//     {
// 			preset_speed(id_list[i],speed_list[i],oc);     
//     }
// 	//多舵机同时开始转动
//   servo_sdata[1]=121; //0x79广播模式
//   servo_sdata[7]=36;  //0x11
// 	if(oc == 0)  
// 	{
// 		servo_sdata[8]=0x14;
// 	}
//   else
// 	{
// 		servo_sdata[8]=0x15;
// 	}
// 	write_data(0);   //发送控制指令，不返回数据
// }

// /*轮子模式下设置电机的转动扭矩
// */
// void set_torque(int id_num,double torque)
// {
// 	servo_sdata[0] = 123;
// 	servo_sdata[1] = id_num;
// 	if(torque > 0)
// 	{
// 		servo_sdata[2] = 0x21; 
// 	}		
// 	else if(torque < 0)
// 	{
// 		servo_sdata[2] = 0x22;
// 		torque = -torque;
// 	}
// 	else
// 	{
// 		servo_sdata[2] = 0x20; 
// 	}
// 	servo_sdata[3] = 0;
// 	servo_sdata[4] = (int)(torque*10);
// 	servo_sdata[5] = ((int)(torque*100)%10);
// 	servo_sdata[7] = 0x21;
// 	servo_sdata[8] = 1;
// 	servo_sdata[9] = 125;
// 	write_data(0);   //发送控制指令，不返回数据
// }


// /* 设置电机编号
// */
// void set_id(int id_num, int id_new)
// {
// 	write_e2(id_num,0,id_new);
// } 

// /*设置电机的波特率函数

//   可设置的波特率为19200, 57600, 115200, 230400, 500000, 1000000。
// */
// void set_baud(int id_num, int baud)
// {
// 	int baud_num=0;
// 	switch(baud)
// 	{
// 		case 19200:baud_num=1;break;
// 		case 57600:baud_num=2;break;
// 		case 115200:baud_num=3;break;
// 		case 230400:baud_num=4;break;
// 		case 500000:baud_num=5;break;
// 		case 1000000:baud_num=6;break;
// 		default: baud_num = 0;
// 	}
// 	if(baud_num>0)
// 	{
// 		write_e2(id_num,1,baud_num);
// 	}
// }

// /* 设置电机模式函数

// 电机有四种模式:轮子模式，阻尼模式，锁死模式，掉电模式
//     处于阻尼模式时，电机可以被掰动，但是阻力很大，而且转动越快，阻力越大
//     处于锁死模式时，电机控制程序启动，将电机角度固定在某个角度，不能被掰动
//     处于掉电模式时，电机可以被随意掰动，阻力很小
//     处于轮子模式时，电机可以整周运转
// */
// void set_mode(int id_num, int mode_num)
// {
// 	servo_sdata[0] = 123;
// 	servo_sdata[1] = id_num;
// 	servo_sdata[7] = 0x16;
// 	servo_sdata[8] = 0x10 + mode_num;
// 	servo_sdata[9] = 125;
// 	write_data(0);   //发送控制指令，不返回数据
// }

// /*解锁电机保护状态

//   在电机进入保护状态（过载保护、电压保护、温度保护）后，不响应电机转动指令，
//   电机回读及修改FLASH指令可使用。在排除导致电机保护的原因后（负载、供电电压、温度过高），可通过本条指令或重新上电解锁保护状态。
// */
// void unlock_stall(int id_num)
// {
// 	servo_sdata[0] = 123;  //0x7B
// 	servo_sdata[1] = id_num;
// 	servo_sdata[2] = 13;
// 	servo_sdata[3] = 50;
// 	servo_sdata[4] = 0;
// 	servo_sdata[5] = 1;
// 	servo_sdata[7] = 0x30;  
// 	servo_sdata[8] = 0;   
// 	servo_sdata[9] = 125; //0x7D
// 	write_data(0);        //发送控制指令，不返回数据
// }

// /*初始化FLASH

// 将电机e2prom参数表中除了ID、波特率（BAUDRATE）、电机正方向标志位（MotorDir）、
// 电机PPR参数的高位和低位（PPR_H、PPR_L）、霍尔编码器方向标志位（HallDir）以外的电机参数值初始化成默认值。
// */
// void e2_init(int id_num)
// {
// 	servo_sdata[0] = 123;
// 	servo_sdata[1] = id_num;
// 	servo_sdata[7] = 0x43;
// 	servo_sdata[9] = 125;
// 	write_data(0);      //发送控制指令，不返回数据
// 	delay_ms(20); 
// }

// /*编码器方向标定函数
// */
// void calibrate_hall(int id_num)
// {
// 	servo_sdata[0] = 123;  //0x7B
// 	servo_sdata[1] = id_num;
// 	servo_sdata[2] = 13;
// 	servo_sdata[3] = 50;
// 	servo_sdata[4] = 0;
// 	servo_sdata[5] = 1;
// 	servo_sdata[7] = 0x2A;  //0x10
// 	servo_sdata[8] = 0;   //0x01
// 	servo_sdata[9] = 125; //0x7D
// 	write_data(0);   //发送控制指令，不返回数据
// 	delay_ms(20);
// }

// /*设置当前位置为编码器零点的函数。
// */
// void set_zero_position(int id_num)
// {
// 	servo_sdata[0] = 123;  //0x7B
// 	servo_sdata[1] = id_num;
// 	servo_sdata[2] = 13;
// 	servo_sdata[3] = 50;
// 	servo_sdata[4] = 0;
// 	servo_sdata[5] = 1;
// 	servo_sdata[7] = 0x2B;  
// 	servo_sdata[8] = 0;   
// 	servo_sdata[9] = 125; //0x7D
// 	write_data(0);   //发送控制指令，不返回数据
// 	delay_ms(20);
// }

// /*修改电机当前正方向转动方向函数
// */
// void change_dir(int id_num)
// {
// 	int motor_dir;
// 	set_speed(id_num, 0, 0);
//   delay_ms(1);
//   motor_dir = read_e2(id_num, 2);
//   if(motor_dir> 0)
//   {
// 		write_e2(id_num, 2, 0);
// 	}
//   else
// 	{
// 		write_e2(id_num, 2, 1);
// 	}
// }

// /*获取当前ID函数
// */
// int get_id(int id_num)
// {
// 	int id=0;
// 	if (get_state(id_num,0)==1)
// 	{
// 		id = state_data[1];
// 		return id;
// 	}
// 	return READ_FLAG;
// }

// /*获取当前位置的函数
// */
// float get_angle(int id_num)
// {
// 	float angle=0;
// 	if (get_state(id_num,0)==1)
// 	{
// 		angle = (state_data[2]&0x7f) * 10 + state_data[3] * 0.1;
// 		if(state_data[2]&0x80)   angle*=-1;
// 		return angle;
// 	}
// 	return READ_FLAG;
// }

// /*获取当前速度的函数
// */
// float get_curspeed(int id_num)
// {
// 	float cur_speed=0;
// 	if (get_state(id_num,0)==1)
// 	{
// 		cur_speed = (state_data[4]&0x7f) * 100 + state_data[5];
// 		if(state_data[4]&0x80)   cur_speed*=-1;
// 		return cur_speed;
// 	}
// 	return READ_FLAG;
// }

// /*获取目标速度的函数
// */
// float get_expspeed(int id_num)
// {
// 	float exp_speed=0;
// 	if (get_state(id_num,0)==1)
// 	{
// 		exp_speed = (state_data[6]&0x7f) * 100 + state_data[7];
// 		if(state_data[6]&0x80)   exp_speed*=-1;
// 		return exp_speed;
// 	}
// 	return READ_FLAG;
// }

// /*获取电流的函数
// */
// float get_current(int id_num)
// {
// 	float current=0;
// 	if (get_state(id_num,0)==1)
// 	{
// 	    current = (state_data[8]&0x7f)/10.0+state_data[9]/100.0;//返回 电机当前电流
// 	    if(state_data[8]&0x80)    current*=-1;
// 		return current;
// 	}
// 	return READ_FLAG;
// }

// /*获取电压的函数
// */
// float get_voltage(int id_num)
// {
// 	float voltage=0;
// 	if (get_state(id_num,0)==1)
// 	{
// 		voltage = state_data[10]/10.0; 
// 		return voltage;
// 	}
// 	return READ_FLAG;
// }

// /*获取温度的函数
// */
// float get_temprature(int id_num)
// {
// 	float temprature=0;
// 	if (get_state(id_num,0)==1)
// 	{
// 		temprature = state_data[11];
// 		return temprature;
// 	}
// 	return READ_FLAG;
// }

// /*获取波特率的函数
// */
// int get_baudrate(int id_num)
// {
// 	int baudrate=0;
// 	if (get_state(id_num,0)==1)
// 	{
// 		baudrate = state_data[12];
// 		return baudrate;
// 	}
// 	return READ_FLAG;
// }

// /*获取当前控制模式函数
// */
// int get_ctrlmode(int id_num)
// {
// 	int ctrlmode=0;
// 	if (get_state(id_num,0)==1)
// 	{
// 		ctrlmode = state_data[13];
// 		return ctrlmode;
// 	}
// 	return READ_FLAG;
// }

// /*获取堵转状态的函数
// */
// int get_stallcase(int id_num)
// {
// 	int stallcase=0;
// 	if(get_state(id_num,0)==1)
// 	{
// 		stallcase = state_data[14];
// 		return stallcase;
// 	}
// 	return READ_FLAG;
// }

// /*获取电机转过的圈数函数
// */
// int get_circle(int id_num)
// {
// 	int circle=0;
// 	if(get_state2(id_num,0)==1)
// 	{
// 		circle = state2_data[4];
// 		if(state2_data[2]&0x80)   circle*=-1;      //注意这里圈数也有正负，正负根据angle的正负来定
// 		return circle;
// 	}
// 	return READ_FLAG;
// }

// /*获取电机停止状态函数
// */
// int get_stop_flag(int id_num)
// {
// 	int stop_flag=0;
// 	if(get_state2(id_num,0)==1)
// 	{
// 		stop_flag = state2_data[5];
// 		return stop_flag;
// 	}
// 	return READ_FLAG;
// }

// /*获取电机当前扭矩值（PWM占空比）函数
// */
// int get_curtorque(int id_num)
// {
// 	int cur_torque=0;
// 	if(get_state2(id_num,0)==1)
// 	{
// 		cur_torque= state2_data[6]*100+state2_data[7];
// 		return cur_torque;
// 	}
// 	return READ_FLAG;
// }

// /*配置电机减速器及编码器参数的函数
// */
// void config_motor(int id_num, int i, int ppr)
// {
// 	int PPR;
// 	PPR = 4 * ppr * i;
// 	write_ppr(id_num, PPR);
// }

// /*给电机写入对应的编码器输出脉冲PPR数据，这个数据是根据电机的参数所计算出来的。
// */
// void write_ppr(int id_num, int PPR)
// {
// 	unsigned char PPR_H,PPR_L;
// 	if (PPR > 0 && PPR <= 12000)
// 	{    PPR_H = (int)(PPR)/ 100;
//         PPR_L = (int)(PPR) % 100;
//         write_e2(id_num, 6, PPR_H);
//         delay_ms(100);
//         write_e2(id_num, 7, PPR_L);
// 	}
// }

// /*读取当前电机编码器的PPR参数值。
// */
// int read_ppr(int id_num)
// {
// 	unsigned char ppr_h,ppr_l;
// 	int PPR;
// 	ppr_h = read_e2(id_num, 6);
//     ppr_l = read_e2(id_num, 7);
//     PPR = ppr_h * 100 + ppr_l;
//     return PPR;
// }

// /*获取当前电机状态信息，例如当前角度等
// */
// int get_state(int id_num, int para_num)           //特别注意 不能多个舵机同时返回数据 即除使用一个舵机外、不允许使用广播模式（121）
// {
// 	servo_sdata[0] = 123;
// 	servo_sdata[1] = id_num;
// 	servo_sdata[7] = 0x13;  
// 	servo_sdata[9] = 125;
// 	write_data(1);   //发送读取指令，需要返回数据
// 	read_data();
// 	if(READ_FLAG==1)
// 	{
// 		for(int i=1;i<15;i++)
// 		{
// 			state_data[i] = servo_rdata[i];
// 		}
// 		state2_data[1] = state_data[1];          //同步更新ID信息
// 		state2_data[2] = state_data[2];          //同步更新Angle_H和Angle_L信息
// 		state2_data[3] = state_data[3];
// 		if(para_num>0&&para_num<15)
// 		{
// 			return state_data[para_num];
// 		}
// 	}
// 	return READ_FLAG;
// }

// /*获取当前电机状态信息，例如当前角度等
// */
// int get_state2(int id_num, int para_num)           //特别注意 不能多个舵机同时返回数据 即除使用一个舵机外、不允许使用广播模式（121）
// {
// 	servo_sdata[0] = 123;
// 	servo_sdata[1] = id_num;
// 	servo_sdata[7] = 0x14;  
// 	servo_sdata[9] = 125;
// 	write_data(1);   //发送读取指令，需要返回数据
// 	read_data();
// 	if(READ_FLAG==1)
// 	{
// 		for(int i=1;i<15;i++)
// 		{
// 			state2_data[i] = servo_rdata[i];
// 		}
// 		state_data[1] = state2_data[1];          //同步更新ID信息
// 		state_data[2] = state2_data[2];          //同步更新Angle_H和Angle_L信息
// 		state_data[3] = state2_data[3];
// 		if(para_num>0&&para_num<15)
// 		{
// 			return state_data[para_num];
// 		}
// 	}
// 	return READ_FLAG;
// }


// /* 修改FLASH中的电机参数值
// */
// void write_e2(int id_num, int address, int value)
// {
// 	u8 i=0;
// 	for(i=0;i<10;i++)
// 	{
// 		servo_sdata[i] = 0;
// 	}
// 	servo_sdata[0] = 123;
// 	servo_sdata[1] = id_num;
// 	servo_sdata[7] = 0x42;
// 	servo_sdata[8] = 0;
// 	servo_sdata[9] = 125;
// 	write_data(0);         //发送控制指令，不返回数据
// 	delay_ms(50);
// 	servo_sdata[1] = id_num;
// 	servo_sdata[2] = (int)(value%255);
// 	servo_sdata[4] = 0;
// 	servo_sdata[5] = 0;
// 	servo_sdata[7] = 0x44;  
// 	servo_sdata[8] = address;     
// 	servo_sdata[9] = 125;
// 	write_data(0);   //发送控制指令，不返回数据
// 	delay_ms(50);
// } 

// /* 读取flash中指定位置的电机参数值
// */
// int read_e2(int id_num, int address)
// {
// 	if(address>=0&&address<32)
// 	{
// 		servo_sdata[0] = 123;
// 		servo_sdata[1] = id_num;
// 		servo_sdata[7] = 0x45;
// 		servo_sdata[8] = address;
// 		servo_sdata[9] = 125;
// 		write_data(1);   //发送读取指令，需要返回数据
// 		read_data();
// 		if(READ_FLAG==1)
// 		{
// 			e2_data[address] = servo_rdata[3];
// 			return e2_data[address];
// 		}
// 		return READ_FLAG;
// 	}
// 	else
// 	{
// 		READ_FLAG = -1;
// 		return READ_FLAG;
// 	}
// } 

// /*读取FLASH中全部的电机参数值,这里前14位通过READ_E2_ALL指令读取，后面的通过read_e2函数读取。
// */
// int read_e2_all(int id_num)
// {
// 	int i=0;
// 	servo_sdata[0] = 123;
// 	servo_sdata[1] = id_num;
// 	servo_sdata[7] = 0x46;
// 	servo_sdata[8] = 0;
// 	servo_sdata[9] = 125;
// 	write_data(1);   //发送读取指令，需要返回数据
// 	read_data();
// 	if(READ_FLAG==1)
// 	{
// 		for(i=0;i<14;i++)
// 		{
// 			e2_data[i] = servo_rdata[i+1];
// 		}
// 		while(i<32&&READ_FLAG==1)
// 		{
// 			read_e2(id_num,i);
// 			i++;
// 		}
// 	}
// 	return READ_FLAG;
// }


// /*预设电机角度函数
// */
// void preset_angle(int id_num, float  angle, int speed)  
// {
// 	unsigned char dir;
// 	if(speed<=0)    speed=100;
// 	if(angle<-360)    angle=-360;
// 	if(angle>360)  angle=360;
// 	servo_sdata[0] = 123;  //0x7B
// 	servo_sdata[1] = id_num;
// 	if (angle < 0)
// 	{  
// 		dir = 1;
//     angle = angle * -1;
// 	}
// 	else
// 	{
// 		dir = 0;
// 	}
// 	servo_sdata[2] = (int)((int)(angle/10))%100;
// 	if (dir == 1)  servo_sdata[2] =servo_sdata[2] | 0x80;
// 	servo_sdata[3] = ((int)(angle * 10)) % 100;
// 	servo_sdata[4] = (int)(speed/100);
// 	servo_sdata[5] = (int)((int)speed% 100);
// 	servo_sdata[7] = 16;  //0x10
// 	servo_sdata[8] = 1;   //0x01
// 	servo_sdata[9] = 125; //0x7D
// 	write_data(0);   //发送控制指令，不返回数据
// }

// /*预设电机相对角度函数
// */
// void prestep_angle(int id_num, float  angle, int speed)  
// {
// 	unsigned char dir;
// 	int circle;
// 	if(speed<=0)    speed=100;
// 	servo_sdata[0] = 123;  //0x7B
// 	servo_sdata[1] = id_num;
//     if (angle < 0)
// 	{  
// 		dir = 1;
//     angle = angle * -1;
// 	}
// 	else
// 	{
// 		dir = 0;
// 	}
// 	circle = (int)(angle/360);
// 	servo_sdata[2] = (int)((int)(angle)%360/10);
// 	if (dir == 1)  servo_sdata[2] =servo_sdata[2] | 0x80;
// 	servo_sdata[3] = ((int)(angle * 10)) % 100;
// 	servo_sdata[4] = (int)(speed/100);
// 	servo_sdata[5] = (int)(speed% 100);
// 	servo_sdata[7] = 16;  //0x10
// 	servo_sdata[8] = 1+circle;   
// 	servo_sdata[9] = 125; //0x7D
// 	write_data(0);   //发送控制指令，不返回数据
// }

// /*预设电机转动速度函数
// */
// void preset_speed(int id_num, int  speed, int oc)  
// {
// 	servo_sdata[0] = 123;  //0x7B
// 	servo_sdata[1] = id_num;
// 	if(speed>0)    servo_sdata[2]=33;
// 	else if(speed<0)
// 	{
// 		servo_sdata[2]=34;
// 		speed = -speed;
// 	}
// 	else
// 	{
// 		servo_sdata[2]=32;
// 	}		
//   if (oc == 0)
// 	{  
// 		servo_sdata[4] = (int)(speed/10);
//     servo_sdata[5] = (int)(speed%10);
// 	}
// 	else
// 	{
// 		servo_sdata[4] = (int)(speed/100);
// 		servo_sdata[5] = (int)(speed%100);
// 	}
// 	servo_sdata[7] = 35;  //0x10
// 	servo_sdata[8] = 1;   
// 	servo_sdata[9] = 125; //0x7D
// 	write_data(0);   //发送控制指令，不返回数据
// }
