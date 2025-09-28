class WritingApp:
    def __init__(self, master, writing_sequence):
        self.master = master
        self.writing_sequence = writing_sequence
        self.canvas = None
        self.setup_ui()

    def setup_ui(self):
        self.master.title("Writing Simulation")
        self.canvas = Canvas(self.master, width=800, height=600, bg="white")
        self.canvas.pack()

        self.draw_strokes()

    def draw_strokes(self):
        for stroke in self.writing_sequence:
            pen_down = stroke['pen_down']
            pen_up = stroke['pen_up']
            self.canvas.create_oval(pen_down[0]-2, pen_down[1]-2, pen_down[0]+2, pen_down[1]+2, fill='black')
            current_pos = pen_down

            for move in stroke['moves']:
                direction = move['direction']
                steps = move['steps']
                dx, dy = self.get_direction_vector(direction)

                for _ in range(steps):
                    current_pos = (current_pos[0] + dx, current_pos[1] + dy)
                    self.canvas.create_line(current_pos[0]-1, current_pos[1]-1, current_pos[0]+1, current_pos[1]+1, fill='black')

            self.canvas.create_oval(pen_up[0]-2, pen_up[1]-2, pen_up[0]+2, pen_up[1]+2, fill='black')

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
        return vectors.get(direction, (0, 0))  # Default to (0, 0) if direction is invalid