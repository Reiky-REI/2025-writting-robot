import cv2
import numpy as np
import time
import csv
import sys
from tqdm import tqdm  # 新增进度条支持
import servo_lib as servo_lib

def get_pen_tip_coordinate(frame):
    # 转换颜色空间并提取笔尖区域（颜色阈值参考 take_photo.py）
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    lower_color = np.array([0, 90, 124])
    upper_color = np.array([6, 150, 255])
    mask = cv2.inRange(hsv, lower_color, upper_color)
    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    if contours:
        c = max(contours, key=cv2.contourArea)
        points = c.reshape(-1, 2)
        median_x = round(float(np.median(points[:, 0])), 2)
        median_y = round(float(np.median(points[:, 1])), 2)
        return median_x, median_y
    else:
        return None, None

def check_servo_state(servo_id, expected_angle, tolerance=5):
    """
    调用舵机读取函数获取真实角度，并判断与期望角度的误差是否超出容差范围，
    如果超出则认为达到极限位置。
    """
    current_angle = servo_lib.get_state(servo_id, 2, o_m=1)
    if current_angle is False:
        return False, None
    limit_reached = abs(current_angle - expected_angle) > tolerance
    return limit_reached, current_angle

def main():
    # 删除卡尔曼滤波器初始化代码
    # ...之前的Kalman初始化代码已删除...
    
    # 初始化摄像头（可根据实际情况调整摄像头索引）
    cap = cv2.VideoCapture(1)
    if not cap.isOpened():
        print("无法打开摄像头")
        return

    csv_filename = "servo_pen_coordinates_02.csv"
    with open(csv_filename, mode='w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(["servo1_angle", "servo2_angle", "pen_x", "pen_y", "real_angle1", "real_angle2", "limit1", "limit2"])

        step_angle = 2  # 角度步长调整为0.5度
        for angle1 in tqdm(np.arange(0, 180, step_angle), desc="Servo1 Progress"):
            for angle2 in tqdm(np.arange(0, 180, step_angle), desc=f"Servo2 Progress (servo1={angle1:.2f})", leave=False):
                print(f"设置舵机1：{angle1:.2f}度, 舵机2：{angle2:.2f}度")
                servo_lib.set_angle(1, angle1, step=10, rn=0)
                servo_lib.set_angle(2, angle2, step=10, rn=0)
                time.sleep(0.2)  # 内层延时调整为 0.2s

                limit1, real_angle1 = check_servo_state(1, angle1)
                limit2, real_angle2 = check_servo_state(2, angle2)
                print(f"舵机1真实角度: {real_angle1} {'已到极限' if limit1 else ''}")
                print(f"舵机2真实角度: {real_angle2} {'已到极限' if limit2 else ''}")

                ret, frame = cap.read()
                if not ret:
                    print("读取摄像头图像失败")
                    continue

                frame = cv2.rotate(frame, cv2.ROTATE_90_COUNTERCLOCKWISE)

                # 直接获取原始笔尖坐标，无滤波处理
                pen_x, pen_y = get_pen_tip_coordinate(frame)
                if pen_x is not None:
                    print(f"检测到笔尖坐标: {pen_x:.2f}, {pen_y:.2f}")
                else:
                    print("未检测到笔尖")
                
                # 写入CSV，直接记录原始笔尖坐标
                writer.writerow([angle1, angle2, pen_x, pen_y, real_angle1, real_angle2, limit1, limit2])
                csvfile.flush()
                cv2.circle(frame, (int(pen_x) if pen_x is not None else 0,
                                   int(pen_y) if pen_y is not None else 0), 5, (0, 255, 0), -1)
                cv2.imshow("调试", frame)
                if cv2.waitKey(30) & 0xFF == ord('q'):
                    cap.release()
                    cv2.destroyAllWindows()
                    return
            time.sleep(2)  # 外层延时 2s

    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
