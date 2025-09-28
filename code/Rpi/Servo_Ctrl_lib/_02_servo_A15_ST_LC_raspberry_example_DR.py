import servo_lib as se
import time

# se.set_id(121, 1)#设置当前舵机的编号为1（注：用广播号设置时，总线上只能连接1个舵机）
# time.sleep(2.0)

# se.set_angle(121, 0, 1)#广播所有舵机快速转到0°
# time.sleep(2)
while True:
    se.set_angle(1, 15, 10)
    se.set_angle(2, 165, 10)
    se.set_angle(3, 18, 100)############
    time.sleep(0.5)
    se.set_angle(1, 90, 10)
    se.set_angle(2, 90, 10)
    se.set_angle(3, 18, 100)############
    time.sleep(0.5)
    se.set_angle(1, 30, 10)
    se.set_angle(2, 125, 10)
    time.sleep(0.5)
    se.set_angle(1, 125, 10)
    se.set_angle(2, 30, 10)
    time.sleep(0.5)
# se.set_angle(121, 200, 1)#广播所有舵机慢速转到200°
# time.sleep(2)


#se.set_angles([0, 1, 2, 4], [120, 150, 200, 260], 10, 2)#4个舵机控制，转到指定角度,使用短协议
#time.sleep(2)

# se.get_state(1, 2, 1)#读取1号舵机的当前角度
# time.sleep(0.5)

# se.change_mode(1, 4)#设置1号舵机的模式为轮子模式
# time.sleep(0.5)

# se.set_speed(1, 500)#设置1号舵机为轮子模式，转速为500,转动
# time.sleep(2.0)

# se.set_speed(1, 0)#设置1号舵机为轮子模式，转速为0，停止
# time.sleep(2.0)

#se.set_pid(1, 21)#设置1号舵机的PID调节的P参数为21

#se.read_e2(1, 3)#读1号舵机的角度PID的P参数
#time.sleep(0.5)

#se.read_e2_all(1)#读1号舵机的e2prom中全部的舵机参数值
#time.sleep(0.5)

#se.e2_init(1)#示例：将1号舵机的e2prom初始化（除ID和波特率之外）
#time.sleep(0.5)

