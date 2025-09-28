import torch
from transformers import AutoTokenizer, AutoModel

# 如果你使用 int4 量化后的模型，确保已经安装相关依赖，并使用对应的加载参数
# 例如，如果使用 AutoGPTQ，你可能需要使用 AutoGPTQForCausalLM 来加载模型
# 下面的示例仍使用 AutoModel，从 Hugging Face 上加载官方权重

# 模型名称或本地路径
model_name = "internlm/internlm-xcomposer2-4khd-7b"

# 加载 tokenizer
tokenizer = AutoTokenizer.from_pretrained(model_name, trust_remote_code=True)

# 加载模型
# 注意：这里建议使用 torch_dtype=torch.bfloat16 以降低显存占用，如需 int4 量化请参考对应方案
model = AutoModel.from_pretrained(
    model_name, 
    torch_dtype=torch.bfloat16,  # 如果使用 int4，请替换为对应的加载配置
    trust_remote_code=True
).cuda().eval()

# 准备图像和问题
# 请确保你安装了 Pillow 库： pip install pillow
from PIL import Image
image_path = "image.png"  # 替换成你自己的图片路径
image = Image.open(image_path)

# 构造问题
query = "<ImageHere>请描述图片中的内容。"

# 对于该模型，视觉问答是通过 chat 方法实现的，
# 第一个对话回合调用 chat 时需要传入 image 参数
with torch.cuda.amp.autocast():
    # 第一次对话：传入 image 和问题，得到响应以及对话历史（history）
    response, history = model.chat(
        tokenizer, 
        query=query, 
        image=image,    # 图像既可以传文件路径，也可以传 PIL.Image 对象
        hd_num=55,      # 可调参数，具体数值参考官方示例或调优
        history=[], 
        do_sample=False, 
        num_beams=3
    )

print("回答:", response)

# 第二次对话示例：基于上一次对话历史提出新的问题
follow_up = "请详细说明图中提到的部分。"
with torch.cuda.amp.autocast():
    response2, _ = model.chat(
        tokenizer, 
        query=follow_up, 
        image=image, 
        hd_num=55, 
        history=history, 
        do_sample=False, 
        num_beams=3
    )

print("跟进回答:", response2)
