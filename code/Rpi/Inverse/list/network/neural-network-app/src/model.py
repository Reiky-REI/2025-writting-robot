import torch
import torch.nn as nn

class NeuralNetwork(nn.Module):
    def __init__(self, input_size, hidden_size, output_size, activation='relu', output_activation='none'):
        super(NeuralNetwork, self).__init__()
        # 根据 activation 参数选择隐藏层激活函数
        switcher = {
            'relu': nn.ReLU(),
            'tanh': nn.Tanh(),
            'swish': nn.SiLU(),
            'h-swish': nn.Hardswish(),
            'sigmoid': nn.Sigmoid()
        }
        self.act_in = switcher.get(activation.lower(), nn.ReLU())

        # 根据 output_activation 参数选择输出层激活函数
        if output_activation.lower() == 'sigmoid':
            self.act_out = nn.Sigmoid()
        elif output_activation.lower() == 'tanh':
            self.act_out = nn.Tanh()
        elif output_activation.lower() == 'softmax':
            self.act_out = nn.Softmax(dim=1)
        elif output_activation.lower() == 'softplus':
            self.act_out = nn.Softplus()
        elif output_activation.lower() == 'relu':
            self.act_out = nn.ReLU()
        else:
            self.act_out = None

        self.fc1 = nn.Linear(input_size, hidden_size)
        self.fc2 = nn.Linear(hidden_size, output_size)

    def forward(self, x):
        x = self.fc1(x)
        x = self.act_in(x)
        x = self.fc2(x)
        # 如果指定了输出层激活函数则执行
        if self.act_out is not None:
            x = self.act_out(x)
        return x