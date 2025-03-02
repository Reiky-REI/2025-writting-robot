#include <servo.h>

int servo_sdata[10]={ 0x7b,0x79,0,0,0,0,0x10,0x10,0x10,0x7d };  //发送数组  10bit
int servo_sdata1[4]={0x7E,0,0,0};
int servo_rdata[16]={0x7b,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x7d};    //舵机返回值存放数组 16bit
u16 i;
int n;
int len;
int id_number;                             //返回 舵机ID
double cur_angle;                          //返回 舵机实际角度
double exp_angle;                          //返回 期望角度
int run_time;                              //返回运行时长
double servo_rpara[10];                     //返回参数数组
//int servo_mode;   //返回 舵机工作模式
//int hoare_data;   //返回 霍尔计数值
//int baud_rate;   //返回 波特率
//double servo_speed;   //返回 速度

u8 u_numm;  //全局变量
unsigned char receive_flag = 0;

/*单个舵机控制函数

参数及解释：
    id_num: 舵机编号0~255（除去0x79、0x7E、0x7F）共253个ID）;121号为广播编号，即所有舵机都会响应到控制指令并执行
    angle: 舵机角度0~270度；         
    step:步数，舵机要达到指定角度分步的次数，以10ms为周期每周期最多运动3度，步数设置为“1”时则以最快速度转动。
        
注：设置完毕后，请留出足够运行的时间

*/
void set_angle(int id_num, float  angle, int step)  
{
	if(step<=0)    step=1;
	//if (rn == 0)
	//{
		if(angle<0)    angle=0;
		if(angle>285)  angle=285;
		servo_sdata[0] = 123;  //0x7B
		servo_sdata[1] = id_num;
		servo_sdata[2] = (int)((angle/10))%100;
		servo_sdata[3] = ((int)(angle * 10)) % 100;
		servo_sdata[4] = (int)(step/100);
		servo_sdata[5] = (int)(step% 100);
		servo_sdata[7] = 16;  //0x10
		servo_sdata[8] = 1;   //0x01
		servo_sdata[9] = 125; //0x7D
		servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
		//发送数组
		usart_senddata(10);

		servo_sdata[1] = id_num;
		servo_sdata[7]=17;
		servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
		usart_senddata(10);
	//}
}

/*多个舵机控制函数

控制指定舵机编号的舵机按照指定的速度转动到指定的角度。
参数及解释:
		id_list: 舵机编号组成的列表
		angle_list: 舵机角度组成的列表（0~270°）
		step: 步数，这里多个舵机共用同一个步数，以保证在不“丢步”的情况下多个舵机同时开始同时结束
		n: 用来控制控制多个舵机的协议，目前有两个版本，分别对应于n=1,n=2,  默认n=1
				n=1,长协议控制：[0x7B, ID, angle_H, angle_L, step_H, step_L, Check, Cmd, CmdData, 0x7D]
				n=2 协议格式为：[126, ID1, angle0, angle1] [126, ID2, angle0, angle1], .....[126, step0, step1, 127]

*/
void set_angles(int id_list[20], float angle_list[20], int step,int n) 
{
  if(n==1)
  {
    for(i=0;i<20;i++)
    {
      if(i!=0)
      {
				if(id_list[i]!=0)
				{
					preset_angle(id_list[i],angle_list[i],step,1);     //
					delay_ms(2);
				}
      }
      else   
      {
        preset_angle(id_list[i],angle_list[i],step,1);     //
        delay_ms(2);
      }
    }
    //多舵机同时开始转动
    servo_sdata[1]=121; //0x79广播模式
    servo_sdata[7]=17;  //0x11
		servo_sdata[8]=1;  
	  servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
    usart_senddata(10);
  }
  else if(n==2)
  {
    for(i=0;i<20;i++)
    {
      if(i!=0)
      {
        if(id_list[i]!=0)
        {
          servo_sdata1[0] = 0x7E;
          servo_sdata1[1] = id_list[i];
          servo_sdata1[2] = (int)(angle_list[i]/10)%100;
          servo_sdata1[3] = (int)(angle_list[i]*10)%100;
          usart_senddata(4);  //发送4字节
          delay_ms(2);   
        }  
      }
      else
      {
        servo_sdata1[0] = 0x7E;
        servo_sdata1[1] = id_list[i];
        servo_sdata1[2] = (int)(angle_list[i]/10)%100;
        servo_sdata1[3] = (int)(angle_list[i]*10)%100;
        usart_senddata(4);  //发送4字节
        delay_ms(2); 
      }
    }
    //设置步数
    servo_sdata1[0] = 0x7E;
    servo_sdata1[1] = (int)(step/100);
    servo_sdata1[2] =  (int)(step%100);
    servo_sdata1[3] = 0x7F;
    usart_senddata(4);
    delay_ms(2);
  }
}

/* 设置舵机模式函数

舵机有4种模式:轮子模式，阻尼模式，锁死模式，掉电模式    
   （运动及轮子模式通过专属指令实现 其余三个通过本条指令实现）
		处于阻尼模式时，舵机可以被掰动，但是阻力很大，而且转动越快，阻力越大
		处于锁死模式时，舵机控制程序启动，将舵机角度固定在某个角度，不能被掰动
		处于掉电模式时，舵机可以被随意掰动，阻力很小
    处于轮子模式时，舵机整周连续转动
   
参数及解释:
		id_num: 舵机编号,即要设置第几号舵机的模式，这里可以用广播形式控制
		mode_num: 用来选择不同的模式
		    mode_num=1,阻尼模式
				mode_num=2,锁死模式
				mode_num=3,掉电模式
        mode_num=4,轮子模式
*/
void change_mode(int id_num, int mode_num)
{
	servo_sdata[0] = 123;
	servo_sdata[1] = id_num;
	servo_sdata[7] = 0x16;
	servo_sdata[8] = 0x10 + mode_num;
	servo_sdata[9] = 125;
	servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
	usart_senddata(10);
	delay_ms(10);
}
/* 设置舵机编号

参数及解释：
    id_num: 需要重新设置编号的舵机编号,如果不知道当前舵机编号，可以用121广播，但是这时总线上只能连一个舵机，否则多个舵机会被设置成相同编号
    id_new: 新舵机编号

*/
void set_id(int id_num, int id_new)
{
	servo_sdata[0] = 123;
	servo_sdata[1] = id_num;
	servo_sdata[7] = 0x42;
	servo_sdata[8] = 0;
	servo_sdata[9] = 125;
	servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
	usart_senddata(10);
	
	servo_sdata[0] = 123;
	servo_sdata[1] = id_num;
  servo_sdata[2] = id_new;
	servo_sdata[7] = 0x44;  
	servo_sdata[8] = 0;
	servo_sdata[9] = 125;
	servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
	usart_senddata(10);
	delay_ms(10);
} 


/* 设置pid控制的参数  

参数及解释：
    id_num: 需要重新设置pid的舵机编号,如果不知道当前舵机编号，可以用121广播
		pid: pid调节的P参数，调节时在默认值20附近给值，一般情况下不需要改变。默认值见协议的EEPROM表

*/
void set_pid(int id_num,int pid)
{
	write_e2(id_num, 3, pid);
}

/*获取当前舵机状态信息，例如当前角度等

获取当前舵机状态，包括
	（1）舵机编号
	（2）当前实际角度
	（3）当前期望角度
	（4）到达期望角度剩余的步数

参数及解释:
	 id_num: 舵机编号,查询第几号舵机的模式，这里不可以用广播模式（虽然总线上只有一个舵机时，可以使用，但这里为了不出错，不让用）
	 para_num: 想要查询的参数编号，
			para_num=0, 返回所有信息组成的列表
			para_num=1, 返回当前舵机编号
			para_num=2, 返回当前角度
			para_num=3, 返回当前期望角度
			para_num=4, 返回运行时长
	 o_m:(one or more) 用来指明多个舵机(o_m=0)还是一个舵机，如果只有一个舵机可以采用广播模式，此时o_m=1

Returns:
	 返回值会根据para_num的值相应改变
			para_num=0, 返回所有信息组成的列表
			para_num=1, 返回当前舵机编号
			para_num=2, 返回当前角度
			para_num=3, 返回当前期望角度
			para_num=4, 返回运行时长

Raises:
	 如果多个舵机连接时，查询指令使用了广播模式，会return 0,不执行查询指令

*/

float get_state(int id_num, int para_num, int o_m)   //特别注意 不能多个舵机同时返回数据 即除使用一个舵机外、不允许使用广播模式（121）
{
	u_numm=0; //全局变量  
	if (id_num == 121 && o_m == 0)
	{
		return 0; 
	}
	else if(id_num == 121 && o_m > 1)
	{
		return 0; 
	}
	else
	{
		u_numm=0;
		receive_flag = 0;
		USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);  //使能中断  在用时开中断

		servo_sdata[0] = 123;
		servo_sdata[1] = id_num;
		servo_sdata[7] = 0x13;  
		
		servo_sdata[9] = 125;
		servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
		usart_senddata(10);

		delay_ms(20);   //时间为通信板返回数据最短时间 理论可改到15ms
		//通信板中有返回数据格式判断  所以只要返回即满足要求
		delay_ms(3000);

		USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);//关闭串口中断  防干扰

		if(receive_flag == 2)
		{
			receive_flag = 0;
			id_number = servo_rdata[1];
			cur_angle = (double )servo_rdata[2] * 10 + (double )servo_rdata[3] * 0.1;
			exp_angle = (double )servo_rdata[4] * 10 + (double )servo_rdata[5] * 0.1;
			run_time = servo_rdata[6] * 100 + servo_rdata[7];
			//返回参数放入数组  方便之后调用

			servo_rpara[0]=id_number;  //返回 舵机编号
			servo_rpara[1]=cur_angle;  //返回 舵机实际角度
			servo_rpara[2]=exp_angle;  //返回 期望角度
			servo_rpara[3]=run_time ;   //返回 剩余步数
							
			if(para_num==1)
				return id_number;
			else if(para_num == 2)
					return cur_angle;
			else if(para_num == +3)
					return exp_angle;
			else if(para_num == 4)
					return run_time;
			else if(para_num == 0)
			{
				servo_rpara[4]=servo_rdata[8];  //返回 舵机工作模式
					 
			}
			return 1;  //返回数据完成  可做读取成功标志位
		}
		else
		{
			return -1;
		}
	}
}
/*轮子模式下设置舵机的转动速度

参数及解释：
		id_num: 舵机编号,即要设置第几号舵机的模式，这里可以用广播模式
		speed: 机转动速度（-1000~1000），速度为正舵机正转，反之反转

*/
void set_speed(int id_num,int speed)
{
	servo_sdata[0] = 123;
	servo_sdata[1] = id_num;
	servo_sdata[7] = 0x16;
	servo_sdata[8] = 0x10 + 4;
	servo_sdata[9] = 125;
	servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
	usart_senddata(10);
	delay_ms(2);
	servo_sdata[0] = 123;
	servo_sdata[1] = id_num;
	if(speed > 0)
			servo_sdata[2] = 0x21; 
	else if(speed < 0)
			{servo_sdata[2] = 0x22;    speed = -speed;}
	else
			servo_sdata[2] = 0x20;
	
	servo_sdata[4] = (int)(speed / 10);
	servo_sdata[5] = (int)((speed) % 10);
	servo_sdata[7] = 0x20;
	servo_sdata[8] = 1;
	servo_sdata[9] = 125;
	servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
	usart_senddata(10);
	delay_ms(2);
}


/*清除两个数组内容 
*/
void servo_rdata_clear(void)
{

	for(i=0;i<15;i++)
	{
		servo_rdata[i]=0;
	}
	for(i=0;i<4;i++)
	{
		servo_rpara[i]=0;
	}
}

/* 修改FLASH中的舵机参数值

参数及解释：
    id_num: 需要修改FLASH舵机编号,如果不知道当前舵机编号，可以用121广播，但是这时总线上只能连一个舵机，否则多个舵机会被设置成相同编号
    address: flash地址
    value：flash数据

*/
void write_e2(int id_num, int address, int value)
{
	servo_sdata[0] = 123;
	servo_sdata[1] = id_num;
	servo_sdata[7] = 0x42;
	servo_sdata[8] = 0;
	servo_sdata[9] = 125;
	servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
	usart_senddata(10);
	delay_ms(2);

	servo_sdata[0] = 123;
	servo_sdata[1] = id_num;
	servo_sdata[2] = (int)(value%100);
	servo_sdata[3] = (int)(value/100);
	servo_sdata[7] = 0x44;  
	servo_sdata[8] = address;     
	servo_sdata[9] = 125;
	servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
	usart_senddata(10);
	delay_ms(2);
} 
/* 读取flash中指定位置的舵机参数值

参数及解释：
		id_num: 舵机编号,即要设置第几号舵机的模式，这里可以用广播模式
		address: flash中的相对位置，0代表0x00, 以此类推，每一位的具体含义如下表

Returns:
		返回舵机发送过来的16数据。

*/
int read_e2(int id_num, int address)
{
	u_numm=0;
	receive_flag = 0;
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);  //使能中断  在用时开中断
	
	servo_sdata[0] = 123;
	servo_sdata[1] = id_num;
	servo_sdata[7] = 0x45;
	servo_sdata[8] = address;
	servo_sdata[9] = 125;
	servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
	usart_senddata(10);
	
	delay_ms(20);   

	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);//关闭串口中断  防干扰

	if(receive_flag == 2)
	{
		receive_flag = 0;
		id_number = servo_rdata[1];
		return (servo_rdata[3]);
	}
	else
	{
		return -1;
	}
} 
/* 
读取FLASH中全部的舵机参数值

参数及解释：
		id_num: 舵机编号,如果总线上多于一个舵机这里不可以用广播模式。
    choose: 选择返回FLASH区域:1 返回前半段; 2 返回后半段

*/
void read_e2_all(int id_num)
{
		u_numm=0;
	receive_flag = 0;
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);  //使能中断  在用时开中断
	
	servo_sdata[0] = 123;
	servo_sdata[1] = id_num;
	servo_sdata[7] = 0x46;
	servo_sdata[8] = 0;
	servo_sdata[9] = 125;
	servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
	usart_senddata(10);
	
	delay_ms(20); 

	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);//关闭串口中断  防干扰
	
//	if(receive_flag == 2)
//		servo_rdata[16];
}

/*解锁舵机保护状态，在舵机进入保护状态——过载保护（堵转保护）后，不响应舵机转动指令，
舵机回读及修改FLASH指令可使用。在排除导致舵机保护的原因后（负载过大），可通过本条指令或重新上电解锁保护状态。

参数及解释：
    id_num: 舵机编号,要解锁的是第几号舵机。
*/
void unlock_stall(int id_num)
{
		servo_sdata[0] = 123;  //0x7B
		servo_sdata[1] = id_num;
		servo_sdata[2] = 13;
		servo_sdata[3] = 50;
		servo_sdata[4] = 0;
		servo_sdata[5] = 1;
		servo_sdata[7] = 0x30;  
		servo_sdata[8] = 0;   
		servo_sdata[9] = 125; //0x7D
		servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
		//发送数组
		usart_senddata(10);
		delay_ms(20);
}

/*
初始化FLASH

参数及解释：
		id_num: 舵机编号,即要初始化FLASH中除了ID号和波特率的所有值（FLASH地址及内容见协议），不可以广播
*/
void e2_init(int id_num)
{
	for(i=0;i<10;i++)
	{
		servo_sdata[i]=0;
	}
	servo_sdata[0] = 123;
	servo_sdata[1] = id_num;
	servo_sdata[7] = 0x42;
	servo_sdata[9] = 125;
	servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
	usart_senddata(10);
	delay_ms(2); 
	servo_sdata[0] = 123;
	servo_sdata[1] = id_num;
	servo_sdata[7] = 0x43;
	servo_sdata[9] = 125;
	servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
	usart_senddata(10);
	delay_ms(2); 
}

void preset_angle(int id_num, float  angle, int step ,int rn)  //set_angles()函数调用时使用，不要单独使用该函数
{
	if(step<=0)    step=1;
	if (rn == 0)
	{
		if(angle<0)    angle=0;
		if(angle>285)  angle=285;
		servo_sdata[0] = 123;  //0x7B
		servo_sdata[1] = id_num;
		servo_sdata[2] = (int)((angle/10))%100;
		servo_sdata[3] = ((int)(angle * 10)) % 100;
		servo_sdata[4] = (int)(step/100);
		servo_sdata[5] = (int)(step% 100);
		servo_sdata[7] = 16;  //0x10
		servo_sdata[8] = 1;   //0x01
		servo_sdata[9] = 125; //0x7D
		servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
		//发送数组
		usart_senddata(10);

		servo_sdata[1] = id_num;
		servo_sdata[7]=17;
		servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
		usart_senddata(10);

	}
	else if (rn == 1)
	{
		if(angle<0)    angle=0;
		if(angle>285)  angle=285;
		servo_sdata[0] = 123;  //0x7B
		servo_sdata[1] = id_num;
		servo_sdata[2] = (int)((angle/10))%100;
		servo_sdata[3] = ((int)(angle * 10)) % 100;
		servo_sdata[4] = (int)(step/100);
		servo_sdata[5] = (int)(step% 100);
		servo_sdata[7] = 16;  //0x10
		servo_sdata[8] = 1;   //0x01
		servo_sdata[9] = 125; //0x7D
		servo_sdata[6] = (servo_sdata[1]+servo_sdata[2]+servo_sdata[3]+servo_sdata[4]+servo_sdata[5]+servo_sdata[7]+servo_sdata[8])%100;
		//发送数组
		usart_senddata(10);
	}

}



