import cv2
import numpy as np
import time
import csv
import sys

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

def mouse_callback(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:
        print(f"Clicked at: ({x}, {y})")
        # 每次点击时将坐标记录到 cleck.csv 文件中
        with open("cleck.csv", "a", newline="") as csvfile:
            import csv  # 确保 csv 模块已导入
            writer = csv.writer(csvfile)
            writer.writerow([x, y])

def main():
    # 初始化摄像头（根据实际情况调整摄像头索引）
    cap = cv2.VideoCapture(1)
    if not cap.isOpened():
        print("无法打开摄像头")
        return

    cap.set(cv2.CAP_PROP_EXPOSURE, -7.5) 
    
    cv2.namedWindow("调试")
    cv2.setMouseCallback("调试", mouse_callback)

    while True:
        ret, frame = cap.read()
        if not ret:
            print("读取摄像头图像失败")
            break

        # 对图像进行旋转调整
        frame = cv2.rotate(frame, cv2.ROTATE_90_COUNTERCLOCKWISE)

        # 获取笔尖坐标（无滤波处理）
        pen_x, pen_y = get_pen_tip_coordinate(frame)
        if pen_x is not None:
            print(f"检测到笔尖坐标: {pen_x:.2f}, {pen_y:.2f}")
        else:
            print("未检测到笔尖")

        cv2.imshow("调试", frame)
        # 若按下 'q' 键则退出循环
        if cv2.waitKey(30) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
