#ifndef __SERVO_H
#define __SERVO_H

#include <stm32f1xx.h>
// #include <delay.h>
#include "usart.h"
#include <S05-MD_init.h>
// #include <sys.h>
/*******************************************************************************************
  @ 大然科技
电机使用函数库说明
为方便用户在不研究本电机通信协议情况下使用该电机、特将电机使用函数封装为库
*******************************************************************************************/

/*发送电机的数据协议函数。

	Args:
	r_n: 是否需要在发送数据后需要接收数据的判断标志位。
		 r_n=0时，表示发送数据后无需接收数据。
		 r_n！=0时，表示需要在发送数据后再接收数据，用于发送读取指令。

	Returns:
		无

	Raises:
		无
*/
void write_data(int r_n);

/*接收电机的数据协议函数。

	Args:
    无

	Returns:
		无

	Raises:
		无
*/
void read_data(void);



/*单个电机控制函数。

  控制指定电机编号的电机按照指定的速度转动到指定的角度。这里是相对于已经设置了电机的0度初始角度而言的控制电机转动的角度，是为控制电机转动的绝对角度。

	Args:
		id_num: 电机编号1-255（除去0x79、0x7E、0x7F）共253个ID,出厂默认为0； 121号为广播编号，即所有舵机都会接收到控制指令并执行
		angle: 电机角度（-360~360）。
		speed: 速度（r/min）,必须是整数，范围（0~1000）。

	Returns:
		无

	Raises:
		无
*/
void set_angle(int id_num, float  angle, int speed);

/*单个电机控制函数。

  控制指定电机编号的电机按照指定的速度相对转动指定角度。这里的角度是相对于电机当前位置而言，对应的是控制电机转动的相对角度。

	Args:
		id_num: 电机编号1-255（除去0x79、0x7E、0x7F）共253个ID,出厂默认为0； 121号为广播编号，即所有舵机都会接收到控制指令并执行
		angle: 电机角度（-360~360）+ -100~100转。
		speed: 速度（r/min）,必须是整数，范围（0~1000）。

	Returns:
		无

	Raises:
		无
*/
void step_angle(int id_num, float angle, int speed);

/*多个电机控制函数。

  控制指定电机编号的电机按照指定的速度转动到指定的角度。

	Args:
		id_list: 电机编号组成的列表
		angle_list: 电机角度组成的列表（-360~360）
		speed_list: 速度组成的列表（r/min）,必须是整数。
		n: 该条指令中需要控制的电机数目，根据实际电机数目填写。
		
	Returns:
		无

	Raises:
		无
*/
void set_angles(int id_list[20], float angle_list[20], int speed_list[20], int n);

/*多个电机控制函数。

  控制指定电机编号的电机按照指定的速度相对转动指定角度。

	Args:
		id_list: 电机编号组成的列表
		angle_list: 电机角度组成的列表（-360~360）+ -100~100转
		speed_list: 速度组成的列表（r/min）,必须是整数。
		n: 该条指令中需要控制的电机数目，根据实际电机数目填写。
		
	Returns:
		无

	Raises:
		无
*/
void step_angles(int id_list[20], float angle_list[20], int speed_list[20], int n);

/*轮子模式下设置电机的转动速度

	Args:
		id_num: 电机编号,即要设置第几号电机的模式，这里可以用广播模式
		speed: 电机转动速度（-1000~1000），速度为正，电机正转，反之反转。
		oc: 速度控制开环/闭环控制位，oc=0为速度开环，oc=1为速度闭环。

	Returns:
		无

	Raises:
		无
*/
void set_speed(int id_num,int speed,int oc);

/*多个电机速度控制函数。

  控制指定电机编号的电机按照指定的速度连续转动

  Args:
      id_list: 电机编号组成的列表
      speed_list: 速度列表(列表元素必须为整数)，在oc取不同值是含义不同.
                  开环模式下（oc=0），表示PWM占空比，范围-1000~1000，分别对应-100%~100%
                  闭环模式下，表示转速，单位为r/min，范围为负最大转速（-MaxSpeed）~最大转速(-MaxSpeed)
      oc: 速度控制开环/闭环控制位，oc=0为速度开环，oc=1为速度闭环。
			n: 该条指令中需要控制的电机数目，根据实际电机数目填写。

  Returns:
      无

  Raises:
      如果id_list和angle_list,speed_list长度不一致时会显示Parameter errors in set_speeds()!

*/
void set_speeds(int id_list[20], int speed_list[20], int oc, int n);

/*轮子模式下设置电机的转动扭矩

  Args:
      id_num: 电机编号,即要设置第几号电机的模式，这里可以用广播模式
      torque:参数范围（-5~5），参数为正，电机正转，反之反转。（参数绝对值越大电机扭力相对越大）

  Returns:
      无

  Raises:
      无
*/
void set_torque(int id_num,double torque);

/*设置电机编号。

  改变电机编号。

	Args:
		id_num: 需要重新设置编号的电机编号,如果不知道当前电机编号，可以用121广播，但是这时总线上只能连一个电机，否则多个电机会被设置成相同编号
		id_new: 新电机编号

	Returns:
		无

	Raises:
		无
*/
void set_id(int id_num, int id_new);

/*设置电机波特率。

	Args:
		id_num: 电机编号,即要设置第几号电机的模式，这里可以用广播模式
		baud: 要设置的波特率，可设置的列表为[19200, 57600, 115200, 230400, 500000, 1000000]。

	Returns:
		无

	Raises:
		无
*/
void set_baud(int id_num, int baud);

/*设置电机模式函数。

    电机有四种模式:阻尼模式，锁死模式，掉电模式，轮子模式
        处于阻尼模式时，电机可以被掰动，但是阻力很大，而且转动越快，阻力越大
        处于锁死模式时，电机控制程序启动，将电机角度固定在某个角度，不能被掰动
        处于掉电模式时，电机可以被随意掰动，阻力很小
        处于轮子模式时，电机可以按照指定的速度连续转动，速度可以自行设定

    Args:
        id_num: 电机编号,即要设置第几号电机的模式，这里可以用广播模式
        mode: 用来选择不同的模式
            mode_num=1,阻尼模式
            mode_num=2,锁死模式
            mode_num=3,掉电模式
            mode_num=4,轮子模式

    Returns:
        无

    Raises:
        无
*/
void set_mode(int id_num, int mode_num);

/*解锁电机保护状态，在电机进入保护状态（过载保护、电压保护、温度保护）后，不响应电机转动指令，
  电机回读及修改FLASH指令可使用。在排除导致电机保护的原因后（负载、供电电压、温度过高），可通过本条指令或重新上电解锁保护状态。

    过载保护，即电机因负载过大进入自我保护状态。
    电压保护，电机允许的供电范围为5.0-12v，不在该范围内则进入电压保护状态。
    温度保护，在长时间大负载运转或堵转导致电机过热，进入温度保护状态，默认触发温度保护值为60℃。（触发保护值可通过修改FLASH设置）。

    Args:
        id_num: 电机编号,要解锁的是第几号电机。

    Returns:
        无

    Raises:
        无
*/
void unlock_stall(int id_num);

/*初始化参数

将电机e2prom参数表中除了ID、波特率（BAUDRATE）、电机正方向标志位（MotorDir）、
电机PPR参数的高位和低位（PPR_H、PPR_L）、霍尔编码器方向标志位（HallDir）以外的电机参数值初始化成默认值。

    Args:
        id_num: 电机编号,即要初始化第几号电机e2prom中的电机参数值。

    Returns:
        无
    Raises:
        无
*/
void e2_init(int id_num);

/*编码器方向标定函数
       不同电机编码器的接线方向会有所不同，最终影响闭环控制，所以在初始使用时，需要首先调用该指令，进行编码器方向标定。
       对编码器的方向进行标定，标定后控制板会将对应参数保存下来，所以正常情况下，只需标定一次即可（同一个电机）。
			 另外标定的同时会自动测试电机最大转速（MaxSpeed）,并同步更新Flash中MaxSpeed参数。

    Args:
        id_num: 电机编号,即要标定的是第几号电机。

    Returns:
        无
    Raises:
        无
*/
void calibrate_hall(int id_num);

/*设置当前位置为编码器零点的函数。

    Args:
        id_num: 电机编号,即要设置的是第几号电机。

    Returns:
        无
    Raises:
        无
*/
void set_zero_position(int id_num);

/*修改电机当前正向转动方向的函数。

  调用该函数，可以将电机正方向反过来。例如当前正方向为顺时针，调用该函数后，电机正方向改为逆时针，以此类推。

		Args:
				id_num: 电机编号,即要设置的是第几号电机。

		Returns:
				无
		Raises:
				无
*/
void change_dir(int id_num);

/*获取当前ID函数

		Args:
				id_num: 电机编号,即要获取的是第几号电机。

    Returns:
        成功返回id
				失败返回读取状态标志位READ_FLAG。
    Raises:
        无
*/
int get_id(int id_num);

/*获取当前位置的函数

		Args:
        id_num: 电机编号,即要获取的是第几号电机。

    Returns:
        成功返回angle，单位为度（°）
				失败返回读取状态标志位READ_FLAG。
    Raises:
        无

*/
float get_angle(int id_num);

/*获取当前速度的函数

		Args:
        id_num: 电机编号,即要获取的是第几号电机。

    Returns:
        成功返回cur_speed，单位为输出占空比，如100代表10%。
				失败返回读取状态标志位READ_FLAG。
    Raises:
        无

*/
float get_curspeed(int id_num);

/*获取目标速度的函数

		Args:
        id_num: 电机编号,即要获取的是第几号电机。

    Returns:
        成功返回exp_speed，单位为输出占空比，如100代表10%。
				失败返回读取状态标志位READ_FLAG。
    Raises:
        无

*/
float get_expspeed(int id_num);

/*获取电流的函数

		Args:
        id_num: 电机编号,即要获取的是第几号电机。

    Returns:
        成功返回current，单位为安培（A）。
				失败返回读取状态标志位READ_FLAG。
    Raises:
        无

*/
float get_current(int id_num);

/*获取电压的函数

		Args:
        id_num: 电机编号,即要获取的是第几号电机。

    Returns:
        成功返回voltage，单位为伏特（V）。
				失败返回读取状态标志位READ_FLAG。
    Raises:
        无

*/
float get_voltage(int id_num);

/*获取温度的函数

		Args:
        id_num: 电机编号,即要获取的是第几号电机。

    Returns:
        成功返回temprature，单位为摄氏度（°C）。
				失败返回读取状态标志位READ_FLAG。
    Raises:
        无

*/
float get_temprature(int id_num);

/*获取波特率的函数

		Args:
        id_num: 电机编号,即要获取的是第几号电机。

    Returns:
        成功返回baudrate，参数含义对应见通信协议说明书表1-5。
				失败返回读取状态标志位READ_FLAG。
    Raises:
        无

*/
int get_baudrate(int id_num);

/*获取当前控制模式函数

		Args:
        id_num: 电机编号,即要获取的是第几号电机。

    Returns:
        成功返回ctrlmode，参数含义对应见通信协议说明书表1-4。
				失败返回读取状态标志位READ_FLAG。
    Raises:
        无

*/
int get_ctrlmode(int id_num);

/*获取堵转状态的函数

		Args:
        id_num: 电机编号,即要获取的是第几号电机。

    Returns:
        成功返回stallcase，0表示未进入保护，1表示进入保护
				失败返回读取状态标志位READ_FLAG。
    Raises:
        无

*/
int get_stallcase(int id_num);

/*获取电机转过的圈数函数

		Args:
        id_num: 电机编号,即要获取的是第几号电机。

    Returns:
        成功返回circle，单位为圈。
				失败返回读取状态标志位READ_FLAG。
    Raises:
        无

*/
int get_circle(int id_num);

/*获取电机停止状态函数

		Args:
        id_num: 电机编号,即要获取的是第几号电机。

    Returns:
        成功返回stop_flag，在没有到达目标位置，stop_flag=0；当电机转动到目标位置后，stop_flag=1。
				失败返回读取状态标志位READ_FLAG。
    Raises:
        无

*/
int get_stop_flag(int id_num);

/*获取电机当前扭矩值（PWM占空比）函数

		Args:
        id_num: 电机编号,即要获取的是第几号电机。

    Returns:
        成功返回cur_torque，单位为PWM占空比。
				失败返回读取状态标志位READ_FLAG。
    Raises:
        无

*/
int get_curtorque(int id_num);

/*配置电机减速器及编码器参数的函数

  配置电机的齿轮减速比i及霍尔编码器基础脉冲数ppr
  该函数功能与write_ppr函数基本一致，两者参数的换算关系为PPR=4*i*ppr

    Args:
        id_num: 电机编号,即要配置的是第几号电机
        i：齿轮减速比
        ppr:电机霍尔编码器编码器基础脉冲数

    Returns:
        无
    Raises:
        无
*/
void config_motor(int id_num, int i, int ppr);

/*写入电机PPR参数

给电机写入对应的编码器输出脉冲PPR数据，这个数据是根据电机的参数所计算出来的。
编码器输出脉冲数指电机输出轴转动一圈，编码器输出的脉冲数PPR

    Args:
        id_num: 电机编号,即要写入的是第几号电机。
        PPR:电机的编码器输出脉冲，PPR=4*编码器基础脉冲数*电机减速比，PPR = PPR_H*100+PPR_L。

    Returns:
        无
    Raises:
        无
*/
void write_ppr(int id_num, int PPR);

/*读取当前电机编码器的PPR参数值。

    Args:
        id_num: 电机编号,即要查询的是第几号电机。

    Returns:
        返回PPR，电机的编码器输出脉冲，PPR=4*编码器基础脉冲数*电机减速比，PPR = PPR_H*100+PPR_L。
    Raises:
        无
*/
int read_ppr(int id_num);

/*获取当前电机状态信息，例如当前角度等

     Args:
        id_num: 电机编号,查询第几号电机的信息，如果总线有多个电机时，不能用121广播编号
        para_num: 想要查询的参数信息对应的数据的编号，para_num的数值对应下面Address的数值。
                Address	    	Item	    	describe
                  01			ID  	    	电机私有ID号
                  02			Angle_H			电机当前角度高位	 	当前角度=（Angle_H&0x7f）*10+Angle_L*0.1
                  03			Angle_L			电机当前角度低位   	*最高位为符号位，（Angle_H>>7）0表示正数，1表示负数
                  04			CurSpeed_H		电机当前速度高位	 	当前速度=（CurSpeed_H&0x7f）*100+CurSpeed_L
                  05			CurSpeed_L		电机当前速度低位	 	*最高位为符号位，（CurSpeed_H>>7）0表示正数，1表示负数
                  06			ExpSpeed_H		电机期望速度高位	 	期望速度=（ExpSpeed_H&0x7f）*100+ExpSpeed_L
                  07			ExpSpeed_L		电机期望速度低位	 	*最高位为符号位，（Exp_Speed>>7）0表示正数，1表示负数
                  08			Current_H   	电机电流高位	     	电机当前电流 =（Current_H&0x7f）/10+Current_L/100
                  09			Current_L  		电机电流低位	     	*最高位为符号位，（Current_H>>7）0表示正数，1表示负数
                  10			Voltage  		电机电压          	电机当前电压 = Voltage/10
                  11			Temprature		温度	          	电机当前温度 =  Temprature
                  12			BAUDRATE		波特率选择		  	范围1-6，1代表19200；2代表57600；3代表115200；4代表230400；5代表500000；6代表1000000，见通信协议说明书内表1-5）
                  13			CtrlMode		电机的控制模式		范围16-22和32，见通信协议说明书内表1-4
                  14			StallCase		保护状态标志位		范围0-1，0表示未进入保护，1表示进入保护

     Returns:
        成功返回值保存在state_data数组
		失败返回读取状态标志位READ_FLAG。

     Raises:
         无
*/
int get_state(int id_num, int para_num);

/*获取电机的角度信息函数

    Args:
        id_num: 电机编号,查询第几号电机的信息，如果总线有多个电机时，不能用121广播编号
        para_num: 想要查询的参数信息对应的数据的编号，para_num的数值对应下面Address的数值。
				Address		Item	    	describe
                  01		ID  	    	电机私有ID号
                  02		Angle_H			电机当前角度高位	 当前角度=（Angle_H&0x7f）*10+Angle_L*0.1
                  03		Angle_L			电机当前角度低位   *最高位为符号位，（Angle_H>>7）0表示正数，1表示负数
                  04		Circle			电机转过的圈数，用来表示超过360度的角度信息；圈数的方向与设置角度的方向相同，如设置电机角度为副角度，圈数返回实际应为负圈数。
                  05		Stop_flag		电机的角度控制是否转动到指定位置的标志位，在没有到达目标位置，Stop_flag=0；当电机转动到目标位置后，Stop_flag=1。该值也可作为舵机是否转动到目标角度的判断标志位。
                  06		CurTorque_H		当前电机PWM的占空比信息，范围0-1000。当前占空比= CurTorque_H*100+ CurTorque_L
                  07		CurTorque_L
				  08    	MaxSpeed    	电机最大转速 = MaxSpeed*10，单位为转每分钟（r/min）,电机最大转速会在编码器方向标定时一块自动标定。

    Returns:
		 成功返回值保存在state_data数组
		 失败返回读取状态标志位READ_FLAG。
				 
    Raises:
         无
*/
int get_state2(int id_num, int para_num);

/*修改e2prom中的电机参数值（备注：除电机ID号外，其他参数一般情况下无需修改）

    Args:
        id_num: 电机编号,即要设置第几号电机的模式，这里可以用广播模式
        address: e2中的相对地址，0代表0x00, 28代表0x28, 以此类推，每一位的具体含义如下表
                Address	Item	    		describe	            初始值
                  00	ID  	    		电机私有ID号	            0xXX	X
                  01	BAUDRATE			波特率选择	                0x03	3
                  02	MoterDir			电机方向	                0x00	0
                  03	Angle_Kp			角度PID参数P参数	            0x14	20
                  04	Angle_Ki			角度PID参数I参数	            0x00	0
                  05	Angle_Kd			角度PID参数D参数	            0x28	0
                  06	PPR_H	    		编码器输出脉冲数高位	        0x00	0
                  07	PPR_L       		编码器输出脉冲数低位	        0x46	70
                  08	PosError    		目标位子容错	                0x03	3
                  09	InitMode    		电机上电时候的模式	          	0x10	16
                  10	Speed_Kp			速度PID P参数	            0x03	3
                  11	Speed_Ki			速度PID I参数	            0x14	20
                  12	Speed_Kd			速度PID D参数	            0x32	60
                  13	StallMode			堵转保护模式选择            	0x11	17
                  14	StallTime			堵转保护触发时间	            0x05	5
                  15	Stall_PWM			堵转后输出的PWM	            0x14	20
                  16	Stall_Current		堵转后输出的电流*10	      	0x02	2
                  17	Current_Kp			电流PID  P参数	            0x05	5
                  18	Current_Ki			电流PID  I参数	            0x00	0
                  19	Current_Kd			电流PID  D参数	            0x00	0
                  20	Max_Current			堵转保护电流阀值	            0x04	4
                  21	Max_Temp			温度保护阀值	                0x3C	60
                  22	Min_Voltage			低压保护阀值	                0x05	5
                  23	Max_Voltage			高压保护阀值	                0x0D	13
                  24	Stop_Time			速度位置控制切换时间	        0x04	4
				  25  	MaxSpeed			电机最大速度/10（r/min）		0x15	21
				  26…27	Free    			FLASH预留	                0x00	0
                  28	HallDir	    		编码器方向标志位	            0x01	1
                  29	Motor_Number		电机厂家识别号	          	0x65	101
                  30	Product_Version		产品版本号	              	0x02	2
                  31	Product_Number		产品识别号	              	0x0C	12

        value: e2中对应地址的设置目标值

    Returns:
        无

    Raises:
        无
*/
void write_e2(int id_num, int address, int value);

/*读取e2prom中指定位置的电机参数值

    Args:
        id_num: 电机编号,即要设置第几号电机的模式，这里可以用广播模式
        address: e2中的相对地址，0代表0x00, 13代表0x13, 以此类推，每一位的具体含义如下表
                Address	Item	    		describe	            初始值
                  00	ID  	    		电机私有ID号	            0xXX	X
                  01	BAUDRATE			波特率选择	                0x03	3
                  02	MoterDir			电机方向	                0x00	0
                  03	Angle_Kp			角度PID参数P参数	            0x14	20
                  04	Angle_Ki			角度PID参数I参数	            0x00	0
                  05	Angle_Kd			角度PID参数D参数	            0x28	0
                  06	PPR_H	    		编码器输出脉冲数高位	        0x00	0
                  07	PPR_L       		编码器输出脉冲数低位	        0x46	70
                  08	PosError    		目标位子容错	                0x03	3
                  09	InitMode    		电机上电时候的模式	          	0x10	16
                  10	Speed_Kp			速度PID P参数	            0x03	3
                  11	Speed_Ki			速度PID I参数	            0x14	20
                  12	Speed_Kd			速度PID D参数	            0x32	60
                  13	StallMode			堵转保护模式选择            	0x11	17
                  14	StallTime			堵转保护触发时间	            0x05	5
                  15	Stall_PWM			堵转后输出的PWM	            0x14	20
                  16	Stall_Current		堵转后输出的电流*10	      	0x02	2
                  17	Current_Kp			电流PID  P参数	            0x05	5
                  18	Current_Ki			电流PID  I参数	            0x00	0
                  19	Current_Kd			电流PID  D参数	            0x00	0
                  20	Max_Current			堵转保护电流阀值	            0x04	4
                  21	Max_Temp			温度保护阀值	                0x3C	60
                  22	Min_Voltage			低压保护阀值	                0x05	5
                  23	Max_Voltage			高压保护阀值	                0x0D	13
                  24	Stop_Time			速度位置控制切换时间	        0x04	4
				  25  	MaxSpeed			电机最大速度/10（r/min）		0x15	21
				  26…27	Free    			FLASH预留	                0x00	0
                  28	HallDir	    		编码器方向标志位	            0x01	1
                  29	Motor_Number		电机厂家识别号	          	0x65	101
                  30	Product_Version		产品版本号	              	0x02	2
                  31	Product_Number		产品识别号	              	0x0C	12

    Returns:
        返回电机发送过来的1位数据。
    Raises:
        无
*/
int read_e2(int id_num, int address);

/*读取e2prom中全部的电机参数值，这里读取14位数据。

    Args:
        id_num: 电机编号,即要设置第几号电机的模式，如果总线上多于一个电机这里不可以用广播模式。

    Returns:
        返回电机发送过来的16数据,如下面的例子所示。
        [123, 1, 70, 0, 20, 0, 0, 100, 13, 2, 17, 20, 1, 1, 0, 125]
        第一位和最后一位为数据包的开头结尾，第n位代表地址为0x0(n-2)的e2值，例如第2位代表地址为0x00的值，即私有ID号为1.
        每一位的具体含义如下表
                Address	Item	    		describe	            初始值
                  00	ID  	    		电机私有ID号	            0xXX	X
                  01	BAUDRATE			波特率选择	                0x03	3
                  02	MoterDir			电机方向	                0x00	0
                  03	Angle_Kp			角度PID参数P参数	            0x14	20
                  04	Angle_Ki			角度PID参数I参数	            0x00	0
                  05	Angle_Kd			角度PID参数D参数	            0x28	0
                  06	PPR_H	    		编码器输出脉冲数高位	        0x00	0
                  07	PPR_L       		编码器输出脉冲数低位	        0x46	70
                  08	PosError    		目标位子容错	                0x03	3
                  09	InitMode    		电机上电时候的模式	          	0x10	16
                  10	Speed_Kp			速度PID P参数	            0x03	3
                  11	Speed_Ki			速度PID I参数	            0x14	20
                  12	Speed_Kd			速度PID D参数	            0x32	60
                  13	StallMode			堵转保护模式选择            	0x11	17
                  14	StallTime			堵转保护触发时间	            0x05	5
                  15	Stall_PWM			堵转后输出的PWM	            0x14	20
                  16	Stall_Current		堵转后输出的电流*10	      	0x02	2
                  17	Current_Kp			电流PID  P参数	            0x05	5
                  18	Current_Ki			电流PID  I参数	            0x00	0
                  19	Current_Kd			电流PID  D参数	            0x00	0
                  20	Max_Current			堵转保护电流阀值	            0x04	4
                  21	Max_Temp			温度保护阀值	                0x3C	60
                  22	Min_Voltage			低压保护阀值	                0x05	5
                  23	Max_Voltage			高压保护阀值	                0x0D	13
                  24	Stop_Time			速度位置控制切换时间	        0x04	4
				  25  	MaxSpeed			电机最大速度/10（r/min）		0x15	21
				  26…27	Free    			FLASH预留	                0x00	0
                  28	HallDir	    		编码器方向标志位	            0x01	1
                  29	Motor_Number		电机厂家识别号	          	0x65	101
                  30	Product_Version		产品版本号	              	0x02	2
                  31	Product_Number		产品识别号	              	0x0C	12

    Raises:
        无
*/
int read_e2_all(int id_num);

/*电机预设控制函数。

  控制指定电机编号的电机按照指定的速度转动到指定绝对角度。

	Args:
		id_num: 电机编号1-255（除去0x79、0x7E、0x7F）共253个ID,出厂默认为0； 121号为广播编号，即所有舵机都会接收到控制指令并执行
		angle: 电机角度（-360~360）。
		speed: 速度（r/min）,必须是整数

	Returns:
		无

	Raises:
		无
*/
void preset_angle(int id_num, float  angle, int speed);

/*电机预设控制函数。

  控制指定电机编号的电机按照指定的速度转动到指定相对角度。

	Args:
		id_num: 电机编号1-255（除去0x79、0x7E、0x7F）共253个ID,出厂默认为0； 121号为广播编号，即所有舵机都会接收到控制指令并执行
		angle: 电机角度（-360~360）+ -100~100转。
		speed: 速度（r/min）,必须是整数

	Returns:
		无

	Raises:
		无
*/
void prestep_angle(int id_num, float  angle, int speed) ;

/*轮子模式下预设电机的转动速度

  预设电机在轮子模式下的转动速度,但是不立即转动，收到启动指令后再开始转动，多用于多个电机速度控制。

  Args:
    id_num: 电机编号,即要设置第几号电机的模式，这里可以用广播模式
    speed: 电机转动速度（-1000~1000），速度为正，电机正转，反之反转。
    oc: 速度控制开环/闭环控制位，oc=0为速度开环，oc=1为速度闭环。

  Returns:
    无

  Raises:
    无

*/
void preset_speed(int id_num, int  speed, int oc);

#endif
