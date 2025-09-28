from tkinter import Tk, Canvas
import logic

class WritingApp:
    def __init__(self, master):
        self.master = master
        self.master.title("Writing Simulation")
        # 可根据需要调整画布大小
        self.canvas = Canvas(master, width=800*25, height=600*25, bg="white")
        self.canvas.pack()
        self.scale = 25  # 缩放因子

    def draw_strokes(self, writing_sequence):
        prev_pen_up = None  # 记录上一个笔画的终点，用于连接路径
        for i, stroke in enumerate(writing_sequence, start=1):
            # 缩放起始点、结束点坐标
            pen_down_orig = stroke['pen_down']
            pen_up_orig = stroke['pen_up']
            pen_down = (pen_down_orig[0] * self.scale, pen_down_orig[1] * self.scale)
            pen_up = (pen_up_orig[0] * self.scale, pen_up_orig[1] * self.scale)
            
            # 如果存在前一个笔画，则连接前一个终点与当前起点
            if prev_pen_up is not None:
                # 若两点在水平和垂直方向都有位移，则用折线连接显示转弯
                if prev_pen_up[0] != pen_down[0] and prev_pen_up[1] != pen_down[1]:
                    # 先绘制从前一个终点到转角点，再到当前起点
                    mid_point = (pen_down[0], prev_pen_up[1])
                    self.canvas.create_line(prev_pen_up[0], prev_pen_up[1],
                                            mid_point[0], mid_point[1],
                                            fill='blue', dash=(4, 2))
                    self.canvas.create_line(mid_point[0], mid_point[1],
                                            pen_down[0], pen_down[1],
                                            fill='blue', dash=(4, 2))
                else:
                    self.canvas.create_line(prev_pen_up[0], prev_pen_up[1],
                                            pen_down[0], pen_down[1],
                                            fill='blue', dash=(4, 2))
            
            # 绘制起点标记
            self.canvas.create_oval(pen_down[0]-2, pen_down[1]-2,
                                    pen_down[0]+2, pen_down[1]+2, fill='black')
            # 显示起点序号标记
            self.canvas.create_text(pen_down[0]-8, pen_down[1]-8, text=str(i), fill='green', font=('Arial', 12, 'bold'))
            
            # 绘制起点前进方向箭头（红色箭头）
            if stroke['moves']:
                first_move = stroke['moves'][0]
                direction = first_move['direction']
                dx, dy = self.get_direction_vector(direction)
                arrow_length = self.scale * 2
                arrow_end = (pen_down[0] + dx * arrow_length, pen_down[1] + dy * arrow_length)
                self.canvas.create_line(pen_down[0], pen_down[1],
                                        arrow_end[0], arrow_end[1],
                                        arrow='last', fill='red', width=2)
            
            # 绘制笔画路径：绘制每一步经过的点
            current_x, current_y = pen_down
            for move in stroke['moves']:
                direction = move['direction']
                steps = move['steps']
                dx, dy = self.get_direction_vector(direction)
                for _ in range(steps):
                    current_x += dx * self.scale
                    current_y += dy * self.scale
                    # 绘制小圆点，显示经过的点
                    r = 2  # 半径，可根据需要调整
                    self.canvas.create_oval(current_x - r, current_y - r, 
                                            current_x + r, current_y + r, 
                                            fill='black', outline='')
            
            # 绘制终点标记
            self.canvas.create_oval(pen_up[0]-2, pen_up[1]-2,
                                    pen_up[0]+2, pen_up[1]+2, fill='black')
            
            prev_pen_up = pen_up  # 更新上一个笔画的终点

    def get_direction_vector(self, direction):
        vectors = {
            0: (1, 0),   # Right
            1: (1, 1),   # Down-Right
            2: (0, 1),   # Down
            3: (-1, 1),  # Down-Left
            4: (-1, 0),  # Left
            5: (-1, -1), # Up-Left
            6: (0, -1),  # Up
            7: (1, -1)   # Up-Right
        }
        return vectors.get(direction, (0, 0))

if __name__ == "__main__":
    root = Tk()
    app = WritingApp(root)

    # Load image and generate writing sequence
    writing_sequence = logic.get_strokes_from_image("output_file.png")
    app.draw_strokes(writing_sequence)

    root.mainloop()