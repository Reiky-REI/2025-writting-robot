import inverse
import time
from servo_lib import set_angles, get_state

def main():
    # 指定坐标（可根据需求修改）
    xF = 5.75  
    yF = 20.6965
    
    # 计算所需的角度值
    phi1, phi4 = inverse.compute_phi(xF, yF)
    print("选取的phi1: {:.4f} 度, phi4: {:.4f} 度".format(phi1, phi4))
    
    # 调用 set_angles 控制舵机：
    # 舵机1赋值phi1，舵机2赋值phi4
    # set_angles(id_list=[1, 2], angle_list=[phi1, phi4], step=10, n=1)
    set_angles(id_list=[1, 2], angle_list=[90, 90], step=10, n=1)
    time.sleep(1)

    # 获取舵机1与舵机2的当前角度
    angle1 = get_state(1, 2)
    angle2 = get_state(2, 2)
    print("舵机1当前角度: {:.2f}, 舵机2当前角度: {:.2f}".format(angle1, angle2))

if __name__ == "__main__":
    main()