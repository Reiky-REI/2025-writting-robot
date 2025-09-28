# -*- coding=utf-8 -*-

"""
天津市大然科技有限公司-总线舵机库

舵机型号：大然A15-ST型舵机
适用平台：树莓派平台（树莓派3,树莓派zero）
库版本号：v1.0
测试主控版本：树莓派zero w
测试人员：靳照乾
测试时间：2018.10.15


代码版本：v1.0
created in 2018.10.12   by Tang Zhao
"""

import serial
import math as cm
import time

# uart = serial.Serial('/dev/ttyAMA0', 19200, timeout=0.5)
# uart = serial.Serial('COM11', 115200, timeout=0.5)
try:
    uart = serial.Serial('COM14', 115200, timeout=0.5)
except Exception as e:
    print("无法配置串口 COM14，请检查端口占用或权限问题：", e)
    uart = None



def write_data(data=[], cn=0):
    if cn == 0:
        if len(data) == 10:
            data[6] = 0
            sum = 0
            for i in range(8):
                sum += data[i + 1]
            data[6] = sum % 100
            uart.write(data)
    else:
        if len(data) > 0:
            uart.write(data)


def read_data(num=16):
    i = 100  # 经过测试，发现正常接收16位耗时大概为500，这里设置1000用来保证数据接收完成
    byte_list = []
    n_s = True
    while uart.inWaiting() < num-1 and i > 0:  # To do:
        i -= 1
        if uart.inWaiting() > 0 and n_s:
            if list(uart.read(1))[0] == 123:
                n_s = False
                byte_list.append(123)
        else:
            time.sleep(0.001)
    while uart.inWaiting() > 0:
        byte_list.append(list(uart.read(1))[0])
    if len(byte_list) == num:
        return byte_list
    else:
        print("接收的数据有误:")
        print(byte_list)
        return []


def set_angle(id_num, angle, step, rn=0):
    """单个舵机控制函数。

    控制指定舵机编号的舵机按照指定的速度转动到指定的角度。

    Args:
        id_num: 舵机编号1-255（除去0x79、0x7E、0x7F）共253个ID,出厂默认为0； 121号为广播编号，即所有舵机都会接收到控制指令并执行
        angle: 舵机角度（0~270）。
        step: 步数，舵机要达到指定角度分步的次数，以10ms为周期每周期最多运动3度，步数设置为“1”时则以最快速度转动。
        rn: (rotate/not) 用来控制舵机是否马上转动，按照最新舵机协议，控制舵机转动分为两步：第一步是让给舵机发送控制数据：包括角度和步数 ；
            第二步发送一个start命令让舵机开始转动。这里的rn就是控制是否执行第二步。
            当rn=0（默认）执行第二步，舵机即刻转动

    Returns:
        无

    Raises:
        无

    """
    if step <= 0:
        step = 1
    data = [0, 0, 13, 50, 0, 1, 0, 0, 0, 0]
    data[0] = 123
    data[1] = id_num
    # 下面两句话用来防止舵机运动到极限位置外，目前不支持负角度
    if angle < 0:
        print("ID号为" + str(id_num) + "的舵机角度超出了运动范围：" + str(angle))
        angle = 0
    elif angle > 285:
        print("ID号为" + str(id_num) + "的舵机角度超出了运动范围：" + str(angle))
        angle = 285
    data[2] = (int(angle)) // 10
    data[3] = (int(angle * 10)) % 100
    data[4] = step // 100
    data[5] = step % 100
    data[6] = 16
    data[7] = 16
    data[8] = 1
    data[9] = 125
    write_data(data)
    if rn == 0:  # start 指令，启动舵机转动
        data[7] = 17
        write_data(data)


def set_angles(id_list=[1, 2, 3], angle_list=[150.0, 150.0, 150.0], step=10, n=1):
    """多个舵机控制函数。

    控制指定舵机编号的舵机按照指定的速度转动到指定的角度。

    Args:
        id_list: 舵机编号组成的列表
        angle_list: 舵机角度组成的列表（0~270）
        step: 步数，这里多个舵机共用同一个步数，以保证在不“丢步”的情况下多个舵机同时开始同时结束，
        n: 用来控制控制多个舵机的协议，目前有三个版本，分别对应于n=1,n=2 默认n=1
        n=1,最初控制协议。
        n=2 协议格式为：[126, ID1, angle0, angle1] [126, ID2, angle0, angle1], .....[126, step0, step1, 127]

    Returns:
        无

    Raises:
        如果id_list和angle_list长度不一致时会显示Parameter errors in set_angles()!

    """
    length = len(id_list)
    if length > 0 and len(angle_list) == length:
        if n == 1:
            for i in range(length):
                set_angle(id_list[i], angle_list[i], step, rn=1)
            # start 指令，启动所有舵机转动，用广播模式
            data = [0, 0, 13, 50, 0, 1, 0, 0, 0, 0]
            data[0] = 123
            data[1] = 121
            data[6] = 16
            data[7] = 17
            data[8] = 2
            data[9] = 125
            write_data(data)
        elif n == 2:
            for i in range(length):
                data = [126, 0, 0, 0]
                data[1] = id_list[i]
                if angle_list[i] < 0:
                    print("ID号为" + str(id_list[i]) + "的舵机角度超出了运动范围：" + str(angle_list[i]))
                    angle_list[i] = 0
                elif angle_list[i] > 285:
                    print("ID号为" + str(id_list[i]) + "的舵机角度超出了运动范围：" + str(angle_list[i]))
                    angle_list[i] = 285
                data[2] = (int(angle_list[i])) // 10
                data[3] = (int(angle_list[i] * 10)) % 100
                write_data(data=data, cn=1)
            data = [126, 0, 0, 127]
            data[1] = step // 100
            data[2] = step % 100
            write_data(data=data, cn=1)
        time.sleep(step * 0.01)
    else:
        print("Parameter errors in set_angles()!")


def change_mode(id_num, mode_num=1):
    """设置舵机模式函数。

    舵机有三种模式:阻尼模式，锁死模式，掉电模式
        处于阻尼模式时，舵机舵机这一被掰动，但是阻力很大，而且转动越快，阻力越大
        处于锁死模式时，舵机控制程序启动，将舵机角度固定在某个角度，不能被掰动
        处于掉电模式时，舵机可以被随意掰动，阻力很小

    Args:
        id_num: 舵机编号,即要设置第几号舵机的模式，这里可以用广播模式
        mode_num: 用来选择不同的模式
            mode_num=1,阻尼模式
            mode_num=2,锁死模式
            mode_num=3,掉电模式
            mode_num=4,轮子模式

    Returns:
        无

    Raises:
        无

    """
    data = [0, 0, 13, 50, 0, 1, 0, 0, 0, 0]
    data[0] = 123
    data[1] = id_num
    data[6] = 0
    data[7] = 22
    data[8] = 16 + mode_num
    data[9] = 125
    write_data(data)


def set_id(id_num=121, id_new=1):
    """设置舵机编号。

    改变舵机编号。

    Args:
        id_num: 需要重新设置编号的舵机编号,如果不知道当前舵机编号，可以用121广播，但是这时总线上只能连一个舵机，否则多个舵机会被设置成相同编号
        id_new: 新舵机编号

    Returns:
        无



    Raises:
        无

    """
    write_e2(id_num=id_num, address=0, value=id_new)


def set_pid(id_num=121, pid=50):
    """设置pid控制的p参数。

    调节pid参数，因为目前舵机控制只用了只用了比例环节，所以这里只要给p参数即可。

    Args:
        id_num: 需要重新设置pid的舵机编号,如果不知道当前舵机编号，可以用121广播
        pid: pid参数，因为目前舵机控制只用了只用了比例环节，所以这里只要给p参数即可，默认=20，调节时在这附近给值，一般情况下不需要改变

    Returns:
        无

    Raises:
        无

    """
    write_e2(id_num=id_num, address=3, value=pid)


def get_state(id_num=1, para_num=2, o_m=0):
    """获取当前舵机状态信息，例如当前角度等

     获取当前舵机状态，包括
        （1）舵机编号
        （2）当前实际角度
        （3）当前期望角度
        （4）返回舵机所需运行时长（ms）
        以后还会继续增加

     Args:
         id_num: 舵机编号,查询第几号舵机的模式，这里不可以用广播模式（虽然总线上只有一个舵机时，可以使用，但这里为了不出错，不让用）
        para_num: 想要查询的参数编号，
            para_num=0, 返回所有信息组成的列表
            para_num=1, 返回当前舵机编号
            para_num=2, 返回当前角度
            para_num=3, 返回当前期望角度
            para_num=4, 返回舵机所需运行时长（ms）
        o_m:(one or more) 用来指明多个舵机还是一个舵机，如果只有一个舵机可以采用广播模式，此时o_m=1


     Returns:
         返回值会根据para_num的值相应改变
            para_num=0, 返回所有信息组成的列表
            para_num=1, 返回当前舵机编号
            para_num=2, 返回当前角度
            para_num=3, 返回当前期望角度
            para_num=4, 返回舵机所需运行时长（ms）

     Raises:
         如果查询指令使用了广播模式，同时由没有指定当前总线上只有一个舵机，会直接报错，打印错误信息并返回False
         或者如果接受到的数据有问题，也会报错，并返回False

     """
    if id_num == 121 and o_m == 0:
        print("the id_num in the get_state can not equal to 121.")
        return False
    else:
        # 发送查询指令段程序
        data = [0, 0, 13, 50, 0, 1, 0, 0, 0, 0]
        data[0] = 123
        data[1] = id_num
        data[6] = 0
        data[7] = 19
        data[8] = 1
        data[9] = 125
        write_data(data)
        byte_list = read_data(16)
        print(byte_list)
        if para_num == 0:
            if len(byte_list) == 16:
                return byte_list
            else:
                print("舵机返回的数据出错")
                return byte_list
        if len(byte_list) == 16:
            if byte_list[0] == 123:
                id_number = byte_list[1]
                cur_angle = (byte_list[2] * 10 + byte_list[3] * 0.1)
                exp_angle = (byte_list[4] * 10 + byte_list[5] * 0.1)
                rem_step = byte_list[6] * 100 + byte_list[7]
                mode_num = byte_list[8]
            else:
                print("data errors: the first byte is not {  in get_state()!")
                return False
            if para_num == 1:
                return id_number
            elif para_num == 2:
                return cur_angle
            elif para_num == 3:
                return exp_angle
            elif para_num == 4:
                return rem_step
            elif para_num == 5:
                return [id_number, cur_angle]
            elif para_num == 6:
                return [id_number, cur_angle, mode_num]
        else:
            return False


def set_speed(id_num=1, speed=500):
    """轮子模式下设置舵机的转动速度

    Args:
        id_num: 舵机编号,即要设置第几号舵机的模式，这里可以用广播模式
        speed: 舵机转动速度（-1000~1000），速度为正，舵机正转，反之反转。

    Returns:
        无

    Raises:
        无

    """
    data = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    data[0] = 123
    data[1] = id_num
    if speed > 0:
        data[2] = 33
    elif speed < 0:
        data[2] = 34
        speed = cm.fabs(speed)
    else:
        data[2] = 32
    data[4] = int(speed / 10)
    data[5] = int(speed) % 10
    data[6] = 20
    data[7] = 32
    data[8] = 1
    data[9] = 125
    write_data(data)


def write_e2(id_num=1, address=0, value=0):
    """修改e2prom中的舵机参数值（备注：除舵机ID号外，其他参数一般情况下无需修改）

    Args:
        id_num: 舵机编号,即要设置第几号舵机的模式，这里可以用广播模式
        address: e2中的相对地址，0代表0x00, 12代表0x12, 以此类推，每一位的具体含义如下表
                Address	Item	    describe	                    初始值
                0X00	ID  	    舵机私有ID号	                0xXX	X
                0X01	OFFSET1	    角度偏移量低位	                0x46	70
                0X02	OFFSET2	    角度偏移量高位	                0x00	0
                0X03	ParaP	    PID参数P参数	                0x14	20
                0X04	ParaI	    PID参数I参数	                0x00	0
                0X05	ParaD	    PID参数D参数	                0x28	0
                0X06	MaxDuty	    PWM最大占空比（最大力矩）	    0x64	100
                0x07	MinDuty     PWM最小占空比（最小力矩）	    0x0d	13
                0x08	PosError    目标位子容错	                0x02	2
                0x09	InitMode    舵机上电时候的模式	            0x11	17
                0x10	StallLimit	堵转保护限制	                0x14	20
                0x11	ChangeIdCount 舵机改变ID次数	            0x00	0
                0x12	BaudRate	波特率选择      	            0x01	1
        value: e2中对应地址的设置目标值

    Returns:
        无

    Raises:
        无

    """
    data = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    data[0] = 123
    data[1] = id_num
    data[7] = 66
    data[9] = 125
    write_data(data)
    data[2] = value
    data[7] = 68
    data[8] = address
    write_data(data)


def read_e2(id_num=1, address=0):
    """读取e2prom中指定位置的舵机参数值

    Args:
        id_num: 舵机编号,即要设置第几号舵机的模式，这里可以用广播模式
        address: e2中的相对地址，0代表0x00, 12代表0x12, 以此类推，每一位的具体含义如下表
                Address	Item	    describe	                    初始值
                0X00	ID  	    舵机私有ID号	                0xXX	X
                0X01	OFFSET1	    角度偏移量低位	                0x46	70
                0X02	OFFSET2	    角度偏移量高位	                0x00	0
                0X03	ParaP	    PID参数P参数	                0x14	20
                0X04	ParaI	    PID参数I参数	                0x00	0
                0X05	ParaD	    PID参数D参数	                0x28	0
                0X06	MaxDuty	    PWM最大占空比（最大力矩）	    0x64	100
                0x07	MinDuty     PWM最小占空比（最小力矩）	    0x0d	13
                0x08	PosError    目标位子容错	                0x02	2
                0x09	InitMode    舵机上电时候的模式	            0x11	17
                0x10	StallLimit	堵转保护限制	                0x14	20
                0x11	ChangeIdCount 舵机改变ID次数	            0x00	0
                0x12	BaudRate	波特率选择      	            0x01	1

    Returns:
        返回舵机发送过来的16位数据列表。
    Raises:
        无

    """
    data = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    data[0] = 123
    data[1] = id_num
    data[7] = 69
    data[8] = address
    data[9] = 125
    write_data(data)
    byte_list = read_data(16)
    print(byte_list)
    if len(byte_list) > 4:
        items_value = ['ID号', '角度偏移量低位', '角度偏移量高位', 'P参数-PID', 'I参数-PID', 'D参数-PID', 'PWM最大占空比（最大力矩）', 'PWM最小占空比（最小力矩）',
                       '最小误差（ADC）', '上电初始模式', '堵转保护角度阈值', '保留位', '急转功能开启状态']
        print(items_value[address] + "为" + str(byte_list[3]))
        return byte_list[3]
    else:
        return False


def read_e2_all(id_num=1):
    """读取e2prom中全部的舵机参数值

    Args:
        id_num: 舵机编号,即要设置第几号舵机的模式，如果总线上多于一个舵机这里不可以用广播模式。

    Returns:
        返回舵机发送过来的16数据,如下面的例子所示。
        [123, 1, 70, 0, 20, 0, 0, 100, 13, 2, 17, 20, 1, 1, 0, 125]
        第一位和最后一位为数据包的开头结尾，第n位代表地址为0x0(n-2)的e2值，例如第2位代表地址为0x00的值，即私有ID号为1.
        每一位的具体含义如下表
                Address	Item	    describe	                    初始值
                Address	Item	    describe	                    初始值
                0X00	ID  	    舵机私有ID号	                0xXX	X
                0X01	OFFSET1	    角度偏移量低位	                0x46	70
                0X02	OFFSET2	    角度偏移量高位	                0x00	0
                0X03	ParaP	    PID参数P参数	                0x14	20
                0X04	ParaI	    PID参数I参数	                0x00	0
                0X05	ParaD	    PID参数D参数	                0x28	0
                0X06	MaxDuty	    PWM最大占空比（最大力矩）	    0x64	100
                0x07	MinDuty     PWM最小占空比（最小力矩）	    0x0d	13
                0x08	PosError    目标位子容错	                0x02	2
                0x09	InitMode    舵机上电时候的模式	            0x11	17
                0x10	StallLimit	堵转保护限制	                0x14	20
                0x11	ChangeIdCount 舵机改变ID次数	            0x00	0
                0x12	BaudRate	波特率选择      	            0x01	1

    Raises:
        无

    """
    data = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    data[0] = 123
    data[1] = id_num
    data[7] = 70
    data[9] = 125
    write_data(data)
    byte_list = read_data(16)
    print(byte_list)
    return byte_list


def e2_init(id_num=0):
    """将舵机e2prom中全部的舵机参数值初始化成默认值，舵机私有ID号除外

    Args:
        id_num: 舵机编号,即要初始化第几号舵机e2prom中全部的舵机参数值。

    Returns:
        无。
    Raises:
        无

    """
    data = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    data[0] = 123
    data[1] = id_num
    data[7] = 67
    data[9] = 125
    write_data(data)


def unlock_stall(id_num=1):
    """解锁舵机保护状态，在舵机进入保护状态——过载保护（堵转保护）后，不响应舵机转动指令，
    舵机回读及修改FLASH指令可使用。在排除导致舵机保护的原因后（负载过大），可通过本条指令或重新上电解锁保护状态。

    过载保护，即舵机因负载过大进入自我保护状态。

    Args:
        id_num: 舵机编号,要解锁的是第几号舵机。

    Returns:
        无

    Raises:
        无

    """
    data = [0, 0, 13, 50, 0, 1, 0, 0, 0, 0]
    data[0] = 123
    data[1] = id_num
    data[7] = 0x30
    data[9] = 125
    write_data(data)


def servo_test():
    """检测舵机好坏的函数。

    控制所有舵机在90度-180度之间来回转动。

    Args:
        无

    Returns:
        无

    Raises:
        无

    """
    while True:
        #set_angle(0, 90, 50,0)
        #time.sleep(0.5)
        #set_angle(0, 180, 10,0)
        #time.sleep(0.5)
        #set_angle(0, 135, 20,0)
        #time.sleep(2.5)
        get_state(121,2,1)
        time.sleep(0.5)


def servo_list(num=13):
    """查询从1号到num号多个舵机当前角度，

    Args:
        num: 舵机数目也是最大的舵机ID号,。

    Returns:
        无。
    Raises:
        无

    """
    angle_list = []
    for i in range(num):
        angle = get_state(i + 1, 2)
        angle_list.append(angle)
        print(angle)
    return angle_list
