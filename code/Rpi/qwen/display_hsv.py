import cv2
import numpy as np

def mouse_callback(event, x, y, flags, param):
    hsv = param['hsv']
    if event == cv2.EVENT_LBUTTONDOWN:
        h, s, v = hsv[y, x]
        print(f"鼠标点击位置 ({x}, {y}) 的 HSV 值为：H={h}, S={s}, V={v}")

cap = cv2.VideoCapture(1)
cap.set(cv2.CAP_PROP_EXPOSURE, -10)  # 设置曝光值为 -8
cap.set(cv2.CAP_PROP_FPS, 25)  # 设置帧率为50fps
if not cap.isOpened():
    print("无法打开摄像头")
    quit()

while True:
    ret, frame = cap.read()
    if not ret:
        print("无法读取摄像头图像")
        break

    hsv_img = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    cv2.imshow("摄像头直播", frame)
    cv2.imshow("HSV 图像", hsv_img)
    
    # 设置鼠标回调函数，将当前的 HSV 图像传递过去
    cv2.setMouseCallback("摄像头直播", mouse_callback, param={'hsv': hsv_img})
    
    key = cv2.waitKey(1) & 0xFF
    if key == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()