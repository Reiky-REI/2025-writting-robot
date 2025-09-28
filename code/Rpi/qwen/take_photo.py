import cv2
import numpy as np

def take_photo(output_filename="photo.jpg", camera_index=0):
    cap = cv2.VideoCapture(camera_index)
    if not cap.isOpened():
        print("无法打开摄像头")
        return

    # 设置快门速度（曝光值），根据摄像头硬件支持情况调整此参数
    # 有些摄像头需要负数曝光值来代表短曝光时间
    # 设置快门速度（曝光时间）
    # 注意：曝光值的取值范围依摄像头硬件而定，一般为负数（如 -1 到 -13）
    cap.set(cv2.CAP_PROP_EXPOSURE, -7.5)  # 设置曝光值为 -10
    cap.set(cv2.CAP_PROP_FPS, 25)  # 设置帧率为25fps

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
            M = cv2.moments(c)
            if M["m00"] != 0:
                cx = int(M["m10"] / M["m00"])
                cy = int(M["m01"] / M["m00"])
                cv2.circle(frame, (cx, cy), 5, (0, 255, 0), -1)
                print("检测到笔尖坐标: ", cx, cy)

        cv2.imshow("摄像头直播", frame)

        key = cv2.waitKey(1) & 0xFF
        if key == ord('c'):
            cv2.imwrite(output_filename, frame)
            print(f"照片已保存为 {output_filename}")
        elif key == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    take_photo("photo.jpg", 1)
