from tkinter import Tk, Canvas

class GridApp:
    def __init__(self, master):
        self.master = master
        self.master.title("32x32 Grid with Coordinates")
        self.canvas = Canvas(master, width=320, height=320, bg="white")
        self.canvas.pack()
        self.draw_grid()

    def draw_grid(self):
        for x in range(32):
            for y in range(32):
                # 计算每个方格的左上角和右下角坐标
                x1 = x * 10
                y1 = y * 10
                x2 = x1 + 10
                y2 = y1 + 10
                # 绘制方格
                self.canvas.create_rectangle(x1, y1, x2, y2, outline="black")
                # 在方格中显示坐标
                self.canvas.create_text(x1 + 5, y1 + 5, text=f"({x},{y})", font=("Arial", 8))

if __name__ == "__main__":
    root = Tk()
    app = GridApp(root)
    root.mainloop()