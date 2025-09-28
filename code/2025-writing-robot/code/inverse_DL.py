import torch
import torch.nn as nn

MODEL_PATH = "C:/Users/reiky/Documents/2025-writing-robot/code/Rpi/Inverse/list/network/neural-network-app/src/trained_model.pth"

# 定义深度学习网络结构（需与训练时的结构一致）
class InverseModel(nn.Module):
    def __init__(self):
        super(InverseModel, self).__init__()
        self.fc = nn.Sequential(
            nn.Linear(2, 64),
            nn.ReLU(),
            nn.Linear(64, 64),
            nn.ReLU(),
            nn.Linear(64, 2)
        )
        
    def forward(self, x):
        return self.fc(x)

# 加载模型
model = InverseModel()
model.load_state_dict(torch.load(MODEL_PATH, map_location=torch.device("cpu")))
model.eval()

def search_phi(x, y):
    """
    使用深度学习网络预测角度。
    输入:
      x: float, x 坐标
      y: float, y 坐标
    输出:
      angle1, angle2: 预测的角度
    """
    input_tensor = torch.tensor([x, y], dtype=torch.float32).unsqueeze(0)  # 形状为 [1, 2]
    with torch.no_grad():
        output = model(input_tensor)
    angle1, angle2 = output.squeeze().tolist()
    return angle1, angle2

if __name__ == "__main__":
    # 简单测试
    test_x, test_y = 200, 300
    a1, a2 = search_phi(test_x, test_y)
    print(f"预测坐标 ({test_x}, {test_y}) 的角度为: {a1:.2f}, {a2:.2f}")
