import os
from PIL import Image, ImageDraw, ImageFont

# 目标字符与字体文件路径
target_char = "瑞"
# font_path = "./input.ttf"  # 请将此处修改为你的等线字体文件路径
font_path = "code/Rpi/front/input.ttf"  # 请将此处修改为你的等线字体文件路径

# 使用较大的画布和字号渲染字符，以便提取真实绘制区域
temp_canvas_size = (1000, 1000)  # 临时画布尺寸
temp_img = Image.new("RGBA", temp_canvas_size, (0, 0, 0, 0))  # 创建透明背景图像
draw_temp = ImageDraw.Draw(temp_img)

# 选择较大的字号，这里以320为例，确保字符足够大
font_size = 320
try:
    font = ImageFont.truetype(font_path, font_size)
except IOError:
    font = ImageFont.load_default()

# 在临时画布上绘制字符，起始位置设为 (0, 0)
draw_temp.text((0, 0), target_char, fill="black", font=font)

# 通过 getbbox() 获取字符的真实边界框（不包含多余透明区域）
bbox = temp_img.getbbox()  # 返回 (left, upper, right, lower) 元组
if bbox is None:
    # 输出调试信息，检查字体文件路径和字符内容
    print("调试信息：字体文件路径 ->", font_path)
    print("调试信息：字符内容 ->", target_char)
    # 检查字体文件是否存在
    if not os.path.exists(font_path):
        raise FileNotFoundError(f"字体文件不存在，请检查路径: {font_path}")
    raise ValueError("未能检测到绘制的字符，请检查字体文件和字符内容。")

# 裁剪出实际绘制区域
cropped_img = temp_img.crop(bbox)

# 将裁剪后的图像缩放到目标尺寸 32x32
filled_img = cropped_img.resize((32, 32), resample=Image.Resampling.LANCZOS)

# 制作黑色背景，并将 filled_img 中的绘制区域（通过 alpha 渠道）填充为白色
alpha = filled_img.split()[3]
final_img = Image.new("RGB", filled_img.size, "black")
white_layer = Image.new("RGB", filled_img.size, "white")
final_img.paste(white_layer, mask=alpha)

# 保存最终图像为 PNG 文件
output_file = "output_filled.png"
final_img.save(output_file)
print(f"修改颜色后的 PNG 文件已生成: {output_file}")