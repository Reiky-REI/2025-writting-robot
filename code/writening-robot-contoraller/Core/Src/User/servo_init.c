#include <servo_init.h>
/*舵机使用初始化函数
 @大然科技
舵机函数库中函数对servo_init.c中usart_senddata()函数进行了调用
故在使用前应配置好两函数
在更换平台或不使用该例程时，同样需要配置上述函数及【串口中断处理程序】（仿照本例程移植）
*/
////串口所需配置

int k;
extern int servo_rdata[];
extern int servo_sdata[];
extern int servo_sdata1[];
extern u8 u_numm;  //在servo.c中有定义 在中断接收函数中使用 
extern unsigned char receive_flag;

//在servo.c中调用了该函数 
void usart_senddata(char kk)
{
	char k;
	if(kk==10)	{
	  for(k=0;k<kk;k++)		{
		USART_SendData(USART1,servo_sdata[k]);   //使用USART1 向计算机发送数据
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	  }
  }
	else if(kk==4)	{
	for(k=0;k<kk;k++)
	  {
		USART_SendData(USART1,servo_sdata1[k]);   //使用USART1 向计算机发送数据
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	  }
		
	}
}

//串口中断处理函数
void USART1_IRQHandler (void)
{
int m=0;
	if (USART_GetFlagStatus(USART1,USART_FLAG_RXNE) ==SET)
	{		
		m=USART_ReceiveData(USART1); 
		
		if(m == 0x7b)
		{
			u_numm = 0;
			receive_flag = 1;
		}
		
		if(receive_flag == 1)
		{
			servo_rdata[u_numm]=m;
			u_numm++;
		}
    
    if(receive_flag == 1 && m == 0x7d && u_numm == 16)
		{
			receive_flag = 2;
		}
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);  //清中断标志位
	}

}

