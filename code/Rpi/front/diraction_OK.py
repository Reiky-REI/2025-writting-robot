import cv2
import numpy as np
import json

def generate_instruction_json(skeleton):
    # 方向编码字典 (dx, dy) -> 方向码
    dir_codes = {
        (1, 0): 0, (1, 1): 1, (0, 1): 2, (-1, 1): 3,
        (-1, 0): 4, (-1, -1):5, (0, -1):6, (1, -1):7
    }

    # 辅助函数：获取8邻域
    def get_neighbors(x, y):
        neighbors = []
        for dy in [-1, 0, 1]:
            for dx in [-1, 0, 1]:
                if dx == 0 and dy == 0: continue
                nx, ny = x+dx, y+dy
                if 0 <= nx < w and 0 <= ny < h:
                    if skeleton[ny, nx] > 0:
                        neighbors.append((nx, ny))
        return neighbors

    # 笔画追踪算法
    def trace_stroke(start_x, start_y, visited):
        stroke = [(start_x, start_y)]
        visited[start_y, start_x] = True
        current = (start_x, start_y)
        prev = (-1, -1)

        while True:
            neighbors = [n for n in get_neighbors(*current) if n != prev]
            if len(neighbors) != 1: break
            
            next_p = neighbors[0]
            if visited[next_p[1], next_p[0]]: break
            
            stroke.append(next_p)
            visited[next_p[1], next_p[0]] = True
            prev = current
            current = next_p

        return stroke

    h, w = skeleton.shape
    visited = np.zeros_like(skeleton, dtype=bool)
    endpoints = []
    crosspoints = []

    # 检测关键点
    for y in range(h):
        for x in range(w):
            if skeleton[y, x] == 0: continue
            neighbors = get_neighbors(x, y)
            if len(neighbors) == 1:
                endpoints.append((x, y))
            elif len(neighbors) >= 3:
                crosspoints.append((x, y))

    # 追踪所有笔画
    strokes = []
    for x, y in endpoints:
        if not visited[y, x]:
            strokes.append(trace_stroke(x, y, visited))
    
    for x, y in crosspoints:
        if not visited[y, x]:
            strokes.append(trace_stroke(x, y, visited))

    # 按书写顺序排序（左上优先）
    strokes.sort(key=lambda s: (s[0][1], s[0][0]))

    # 生成指令序列
    instructions = []
    prev_end = None
    pen_down = False

    for stroke in strokes:
        if not stroke:
            continue

        start = stroke[0]
        end = stroke[-1]

        # 处理笔画间连接
        if prev_end is not None:
            dx = start[0] - prev_end[0]
            dy = start[1] - prev_end[1]
            
            # 8邻域连接检查
            if max(abs(dx), abs(dy)) <= 1:
                # 自动连接不需要抬笔
                if not pen_down:
                    instructions.append({"action": "PenDown"})
                    pen_down = True
            else:
                # 需要重新定位
                if pen_down:
                    instructions.append({"action": "PenUp"})
                    pen_down = False
                instructions.append({"action": "MoveTo", "x": int(start[0]), "y": int(start[1])})
                instructions.append({"action": "PenDown"})
                pen_down = True
        else:
            # 第一个笔画
            instructions.append({"action": "MoveTo", "x": int(start[0]), "y": int(start[1])})
            instructions.append({"action": "PenDown"})
            pen_down = True

        # 添加笔画路径
        for point in stroke[1:]:
            instructions.append({"action": "MoveTo", "x": int(point[0]), "y": int(point[1])})

        prev_end = end

    # 最终抬笔
    if pen_down:
        instructions.append({"action": "PenUp"})

    return instructions

# 使用示例
if __name__ == "__main__":
    # 载入图像
    image = cv2.imread("output_file.png", cv2.IMREAD_GRAYSCALE)
    _, binary = cv2.threshold(image, 128, 255, cv2.THRESH_BINARY)
    
    # 生成指令
    instructions = generate_instruction_json(binary)
    
    # 输出JSON
    output = {
        "version": "1.0",
        "resolution": (32, 32),
        "instructions": instructions
    }
    
    print(json.dumps(output, indent=2))