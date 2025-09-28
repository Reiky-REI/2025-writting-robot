import time
from front import process_character, execute_instructions

def main(character):
    # 直接生成指令序列（内部依次调用生成位图、提取骨架、生成坐标序列）
    instructions = process_character(character)
    
    # 工作空间坐标：(左上)(-7,27) 和右下(7,10)
    # 分别计算 x,y 方向的缩放比例（32为位图尺寸）
    workspace = [(-7, 27), (-7, 10), (7, 27), (7, 10)]
    scale_x = (workspace[2][0] - workspace[0][0]) / 32  # (7 - (-7))/32 = 14/32
    scale_y = (workspace[1][1] - workspace[0][1]) / 32  # (10 - 27)/32 = -17/32
    
    # 对指令中MoveTo指令进行坐标转换
    scaled_instructions = []
    for ins in instructions:
        if ins["action"] == "MoveTo":
            # 注意：这里假设原始坐标为正（0~32），转换到工作空间时需加上左上角坐标
            x = ins["x"] * scale_x + workspace[0][0]
            y = ins["y"] * scale_y + workspace[0][1]
            scaled_instructions.append({"action": "MoveTo", "x": x, "y": y})
        else:
            scaled_instructions.append(ins)
    
    # 按步骤执行指令，每步间隔0.6秒
    execute_instructions(scaled_instructions)

if __name__ == "__main__":
    character_input = input("请输入要绘制的字符: ")
    main(character_input)