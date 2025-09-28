import os
from PIL import Image, ImageDraw, ImageFont

def generate_bitmap(character, font_path, font_size=320, canvas_size=(1000, 1000), target_size=(32, 32)):
    """
    根据字符生成缩放后的位图
    :param character: 需要绘制的字符
    :param font_path: 字体文件路径
    :param font_size: 使用的字号
    :param canvas_size: 临时画布尺寸
    :param target_size: 输出图像尺寸
    :return: PIL.Image 对象（RGB 模式）
    """
    # 创建透明背景图像
    temp_img = Image.new("RGBA", canvas_size, (0, 0, 0, 0))
    draw_temp = ImageDraw.Draw(temp_img)

    # 加载字体文件，如失败则使用默认字体
    try:
        font = ImageFont.truetype(font_path, font_size)
    except IOError:
        font = ImageFont.load_default()

    # 绘制字符
    draw_temp.text((0, 0), character, fill="black", font=font)
    bbox = temp_img.getbbox()
    if bbox is None:
        print("调试信息：字体文件路径 ->", font_path)
        print("调试信息：字符内容 ->", character)
        if not os.path.exists(font_path):
            raise FileNotFoundError(f"字体文件不存在，请检查路径: {font_path}")
        raise ValueError("未能检测到绘制的字符，请检查字体文件和字符内容。")

    # 裁剪实际绘制区域并缩放
    cropped_img = temp_img.crop(bbox)
    filled_img = cropped_img.resize(target_size, resample=Image.Resampling.LANCZOS)

    # 用黑色背景填充绘制区域
    alpha = filled_img.split()[3]
    final_img = Image.new("RGB", filled_img.size, "black")
    white_layer = Image.new("RGB", filled_img.size, "white")
    final_img.paste(white_layer, mask=alpha)

    # 返回 PIL.Image 对象（而不是 np.array(final_img)）
    return final_img

def main():
    # 示例测试
    target_char = "示"
    font_path = os.path.join(os.path.dirname(__file__), "input.ttf")
    img = generate_bitmap(target_char, font_path)
    img.save("test_output.png")
    print("保存成功：test_output.png")

if __name__ == "__main__":
    main()

