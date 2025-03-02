#ifndef __SERVO_H
#define __SERVO_H

#include <stm32f10x.h>
#include <delay.h>
#include <usart.h>
#include <servo_init.h>
/*******************************************************************************************
  @ 大然科技
舵机使用函数库说明
为方便用户在不研究本舵机通信协议情况下使用该舵机、特将舵机使用函数封装为库
*******************************************************************************************/

//用到的全局变量
extern int servo_rdata[];
extern double servo_rpara[];  

/*单个舵机控制函数

参数及解释：
    id_num: 舵机编号1~27； 121号为广播编号，即所有舵机都会响应到控制指令并执行
    angle: 舵机角度0~270度；         
    step:步数，舵机要达到指定角度分步的次数，以10ms为周期每周期最多运动3度，步数设置为“1”时则以最快速度转动。
    rn: (rotate/not) 
        rn == 0 控制舵机转动到指定角度
注：设置完毕后，请留出足够运行的时间
*/
void set_angle(int id_num, float  angle, int step);

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
void set_angles(int id_list[20], float angle_list[20], int step,int n);
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
void change_mode(int id_num, int mode_num);

/* 设置舵机编号

参数及解释：
    id_num: 需要重新设置编号的舵机编号,如果不知道当前舵机编号，可以用121广播，但是这时总线上只能连一个舵机，否则多个舵机会被设置成相同编号
    id_new: 新舵机编号

*/
void set_id(int id_num, int id_new);

/* 设置pid控制的参数  

参数及解释：
    id_num: 需要重新设置pid的舵机编号,如果不知道当前舵机编号，可以用121广播
		pid: pid参数的P参数，调节时在默认值20附近给值，一般情况下不需要改变。默认值见协议的EEPROM表

*/
void set_pid(int id_num,int pid);

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
			para_num=4, 返回到达期望角度剩余的步数
	 o_m:(one or more) 用来指明多个舵机(o_m=0)还是一个舵机，如果只有一个舵机可以采用广播模式，此时o_m=1

Returns:
	 返回值会根据para_num的值相应改变
			para_num=0, 返回所有信息组成的列表
			para_num=1, 返回当前舵机编号
			para_num=2, 返回当前角度
			para_num=3, 返回当前期望角度
			para_num=4, 返回到达期望角度剩余的步数

Raises:
	 如果多个舵机连接时，查询指令使用了广播模式，会return 0,不执行查询指令

*/

float get_state(int id_num, int para_num, int o_m);

/*轮子模式下设置舵机的转动速度

参数及解释：
		id_num: 舵机编号,即要设置第几号舵机的模式，这里可以用广播模式
		speed: 机转动速度（-1000~1000），速度为正舵机正转，反之反转

*/
void set_speed(int id_num,int speed);

/*设置舵机的转动速度  精确控制  转速单位为r/min

参数及解释：
		id_num: 舵机编号,即要设置第几号舵机的模式，这里可以用广播模式
		speed: 舵机转动速度，单位为r/min

*/
void set_exact_speed(int id_num,int speed);

/*清除两个数组内容 
*/
void servo_rdata_clear(void);

/* 修改FLASH中的舵机参数值

参数及解释：
    id_num: 需要修改FLASH舵机编号,如果不知道当前舵机编号，可以用121广播，但是这时总线上只能连一个舵机，否则多个舵机会被设置成相同编号
    address: flash地址
    value：flash数据

*/
void write_e2(int id_num, int address, int value);


/* 读取flash中指定位置的舵机参数值

参数及解释：
		id_num: 舵机编号,即要设置第几号舵机的模式，这里可以用广播模式
		address: flash中的相对位置，0代表0x00, 以此类推，每一位的具体含义如下表

Returns:
		返回舵机发送过来的16数据。

*/
int read_e2(int id_num, int address);

/* 
读取FLASH中全部的舵机参数值

参数及解释：
		id_num: 舵机编号,如果总线上多于一个舵机这里不可以用广播模式。
    choose: 选择返回FLASH区域:1 返回前半段; 2 返回后半段

*/
void read_e2_all(int id_num);

/*
解锁舵机保护状态，在舵机进入保护状态——过载保护（堵转保护）后，不响应舵机转动指令，
舵机回读及修改FLASH指令可使用。在排除导致舵机保护的原因后（负载过大），可通过本条指令或重新上电解锁保护状态。

过载保护，即舵机因负载过大进入自我保护状态。

参数及解释：
    id_num: 舵机编号,要解锁的是第几号舵机。

Returns:
    无
*/
void unlock_stall(int id_num);

/*
初始化FLASH

参数及解释：
		id_num: 舵机编号,即要初始化FLASH中除了ID号和波特率的所有值（FLASH地址及内容见协议），不可以广播
*/
void e2_init(int id_num);

void preset_angle(int id_num, float  angle, int step ,int rn);




#endif
