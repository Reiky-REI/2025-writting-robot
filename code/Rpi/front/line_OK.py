from PIL import Image
import sys
import numpy as np
import cv2 as cv

# 新增骨架提取函数（优化版）
def skeleton(im):
    """
    骨架提取优化：
    先进行二值化，再尝试使用 cv.ximgproc.thinning 提取骨架，
    若不可用则采用迭代腐蚀-开运算法进行退化式提取。
    :param im: 灰阶图像，np.array 格式
    :return: 骨架图（uint8）
    """
    _, im_thresh = cv.threshold(im, 0, 255, cv.THRESH_BINARY + cv.THRESH_OTSU)
    try:
        # 尝试使用 cv.ximgproc.thinning（需 opencv-contrib-python）
        skel = cv.ximgproc.thinning(im_thresh)
    except Exception:
        # 回退到迭代方法
        skel = np.zeros(im_thresh.shape, np.uint8)
        element = cv.getStructuringElement(cv.MORPH_CROSS, (3, 3))
        done = False
        while not done:
            eroded = cv.erode(im_thresh, element)
            temp = cv.dilate(eroded, element)
            temp = cv.subtract(im_thresh, temp)
            skel = cv.bitwise_or(skel, temp)
            im_thresh = eroded.copy()
            done = (cv.countNonZero(im_thresh) == 0)
    return skel

def erode_image(img, kernel):
    width, height = img.size
    k_height = len(kernel)
    k_width = len(kernel[0])
    k_center_x = k_width // 2
    k_center_y = k_height // 2

    original_pixels = img.load()
    eroded_img = Image.new("L", (width, height))
    eroded_pixels = eroded_img.load()

    for x in range(width):
        for y in range(height):
            min_val = 255
            for ky in range(k_height):
                for kx in range(k_width):
                    if kernel[ky][kx] == 1:
                        nx = x + kx - k_center_x
                        ny = y + ky - k_center_y
                        if 0 <= nx < width and 0 <= ny < height:
                            min_val = min(min_val, original_pixels[nx, ny])
            eroded_pixels[x, y] = min_val
    return eroded_img

def dilate_image(img, kernel):
    width, height = img.size
    k_height = len(kernel)
    k_width = len(kernel[0])
    k_center_x = k_width // 2
    k_center_y = k_height // 2

    original_pixels = img.load()
    dilated_img = Image.new("L", (width, height))
    dilated_pixels = dilated_img.load()

    for x in range(width):
        for y in range(height):
            max_val = 0
            for ky in range(k_height):
                for kx in range(k_width):
                    if kernel[ky][kx] == 1:
                        nx = x + kx - k_center_x
                        ny = y + ky - k_center_y
                        if 0 <= nx < width and 0 <= ny < height:
                            max_val = max(max_val, original_pixels[nx, ny])
            dilated_pixels[x, y] = max_val
    return dilated_img

def process_image(input_path, output_path):
    # 開啟影像並轉換成灰階
    img = Image.open(input_path).convert("L")
    
    # 過濾亮度：若亮度小於200則設為0
    pixels = img.load()
    for x in range(img.width):
        for y in range(img.height):
            if pixels[x, y] < 212:
                pixels[x, y] = 0

    # 新增骨架提取优化：转换为 NumPy 数组，提取骨架，然后回转为 PIL Image
    im_np = np.array(img)
    skel = skeleton(im_np)
    img = Image.fromarray(skel)
    
    # 定義四個不同的算子（kernel）
    kernel1 = [
        [0, 0, 0],
        [0, 1, 0],
        [0, 0, 0]
    ]
    kernel2 = [
        [0, 1, 0],
        [1, 1, 1],
        [0, 1, 0]
    ]
    kernel3 = [
        [1, 0, 0],
        [0, 1, 0],
        [0, 0, 1]
    ]
    kernel4 = [
        [0, 0, 1],
        [0, 1, 0],
        [1, 0, 0]
    ]
    
    kernels = [kernel1, kernel2, kernel3, kernel4]
    opened_images = []
    
    # 分別對每種算子做開運算（腐蝕後膨脹）
    for kernel in kernels:
        eroded_img = erode_image(img, kernel)
        opened_img = dilate_image(eroded_img, kernel)
        opened_images.append(opened_img)
    
    # 逐像素將四幅圖像的亮度疊加, 並限制最大值為255
    width, height = img.size
    combined_img = Image.new("L", (width, height))
    combined_pixels = combined_img.load()
    
    for x in range(width):
        for y in range(height):
            total = sum(im.load()[x, y] for im in opened_images)
            combined_pixels[x, y] = min(total, 255)
    
    combined_img.save(output_path)
    print(f"Processed image saved to {output_path}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python line.py <input_image> <output_image>")
        sys.exit(1)
        
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    process_image(input_file, output_file)