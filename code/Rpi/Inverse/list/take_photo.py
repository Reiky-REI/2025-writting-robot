import cv2
import numpy as np

def take_photo(output_filename="photo.jpg", camera_index=0):
    # 删除卡尔曼滤波器初始化代码
    # ...之前的Kalman初始化代码已删除...

    cap = cv2.VideoCapture(camera_index)
    if not cap.isOpened():
        print("无法打开摄像头")
        return

    cap.set(cv2.CAP_PROP_EXPOSURE, -10.5)
    cap.set(cv2.CAP_PROP_FPS, 25)

    try:
        while True:
            ret, frame = cap.read()
            if not ret:
                print("无法读取摄像头图像")
                break

            hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
            lower_color = np.array([0, 90, 124])
            upper_color = np.array([6, 150, 255])
            mask = cv2.inRange(hsv, lower_color, upper_color)
            cv2.imshow("Mask调试", mask)

            contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
            if contours:
                c = max(contours, key=cv2.contourArea)
                points = c.reshape(-1, 2)
                median_x = round(float(np.median(points[:, 0])), 2)
                median_y = round(float(np.median(points[:, 1])), 2)
                print(f"检测到笔尖坐标: {median_x:.2f}, {median_y:.2f}")
            else:
                print("未检测到有效笔尖")
            
            cv2.imshow("摄像头直播", frame)
            key = cv2.waitKey(1) & 0xFF
            if key == ord('c'):
                cv2.imwrite(output_filename, frame)
                print(f"照片已保存为 {output_filename}")
            elif key == ord('q'):
                break
    except KeyboardInterrupt:
        print("Keyboard interrupt detected, exiting loop")

    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    take_photo("photo.jpg", 1)
