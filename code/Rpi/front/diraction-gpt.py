import numpy as np
from PIL import Image
import json

# 加载PNG图像并转换为二值数组
image_path = 'output_file.png'
image = Image.open(image_path).convert('L')
image_array = np.array(image) // 255  # 将像素值转换为0或1

# 获取图像的尺寸
height, width = image_array.shape

# 定义八邻域的移动方向
directions = [(-1, 0), (1, 0), (0, -1), (0, 1), (-1, -1), (-1, 1), (1, -1), (1, 1)]

# 初始化路径列表
paths = []

# 遍历图像数组，寻找笔画的起点
visited = np.zeros_like(image_array)
for y in range(height):
    for x in range(width):
        if image_array[y, x] == 1 and not visited[y, x]:
            # 找到一个新的笔画，初始化路径
            path = []
            pen_down = False
            current_pos = (x, y)
            path.append(f"MoveTo({x}, {y})")
            path.append("下笔")
            visited[y, x] = 1

            # 使用深度优先搜索（DFS）来遍历笔画的像素点
            stack = [current_pos]
            while stack:
                cx, cy = stack.pop()
                for dx, dy in directions:
                    nx, ny = cx + dx, cy + dy
                    if 0 <= nx < width and 0 <= ny < height and image_array[ny, nx] == 1 and not visited[ny, nx]:
                        visited[ny, nx] = 1
                        path.append(f"MoveTo({nx}, {ny})")
                        stack.append((nx, ny))

            path.append("抬笔")
            paths.append(path)

# 将路径列表转换为JSON格式并打印
json_output = json.dumps(paths, ensure_ascii=False, indent=2)
print(json_output)
