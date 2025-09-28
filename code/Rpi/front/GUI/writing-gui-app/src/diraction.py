import cv2
import numpy as np

# 方向编码字典 (dx, dy) -> 方向码
dir_codes = {
    (1, 0): 0, (1, 1): 1, (0, 1): 2, (-1, 1): 3,
    (-1, 0): 4, (-1, -1):5, (0, -1):6, (1, -1):7
}

def generate_writing_sequence(skeleton):
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

    # 生成移动指令
    writing_sequence = []
    for stroke in strokes:
        if not stroke: continue
        
        # 落笔坐标
        start_x, start_y = stroke[0]
        
        # 计算移动路径
        moves = []
        current_dir = None
        step_count = 0
        
        for i in range(len(stroke)-1):
            x1, y1 = stroke[i]
            x2, y2 = stroke[i+1]
            dx, dy = x2-x1, y2-y1
            direction = dir_codes.get((dx, dy), -1)
            
            if direction == current_dir:
                step_count += 1
            else:
                if current_dir is not None:
                    moves.append({"direction": current_dir, "steps": step_count})
                current_dir = direction
                step_count = 1
        
        if current_dir is not None:
            moves.append({"direction": current_dir, "steps": step_count})
        
        # 抬笔坐标
        end_x, end_y = stroke[-1]
        
        writing_sequence.append({
            "pen_down": (int(start_x), int(start_y)),
            "moves": moves,
            "pen_up": (int(end_x), int(end_y))
        })

    return writing_sequence

# 使用示例
if __name__ == "__main__":
    # 指定文件路径
    file_path = r"output_file.png"
    
    # 载入并预处理图像
    image = cv2.imread(file_path, cv2.IMREAD_GRAYSCALE)
    _, binary = cv2.threshold(image, 128, 255, cv2.THRESH_BINARY)
    
    # 生成书写序列
    writing_sequence = generate_writing_sequence(binary)
    
    # 输出结果（示例）
    for i, stroke in enumerate(writing_sequence):
        print(f"Stroke {i+1}:")
        print(f"  Pen Down at: {stroke['pen_down']}")
        print("  Writing Path:")
        for move in stroke["moves"]:
            print(f"    Direction: {move['direction']}, Steps: {move['steps']}")
        print(f"  Pen Up at: {stroke['pen_up']}\n")