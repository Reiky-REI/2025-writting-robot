def get_strokes_from_image(image_path):
    import cv2
    import numpy as np
    from diraction import generate_writing_sequence

    # 载入并预处理图像
    image = cv2.imread(image_path, cv2.IMREAD_GRAYSCALE)
    _, binary = cv2.threshold(image, 128, 255, cv2.THRESH_BINARY)

    # 生成书写序列
    writing_sequence = generate_writing_sequence(binary)
    return writing_sequence