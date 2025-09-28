import argparse  # 新增模块导入
import numpy as np
import pandas as pd
import torch
import matplotlib.pyplot as plt
from model import NeuralNetwork
from train import train_model
from data import load_data
import matplotlib
matplotlib.rcParams['font.sans-serif'] = ['SimHei']
matplotlib.rcParams['axes.unicode_minus'] = False

def main(file_path, use_cuda):
    # Load the dataset from file_path
    X_train, X_test, y_train, y_test = load_data(file_path)

    # 选择设备：若 use_cuda 为 True 且 CUDA 可用则使用 CUDA，否则使用 CPU
    device = torch.device('cuda' if use_cuda and torch.cuda.is_available() else 'cpu')
    if device.type == 'cuda':
        torch.backends.cudnn.benchmark = True  # 启用 GPU 加速优化
    print(f"使用设备: {device}")
    
    # 新增调整模型参数，请在此处修改参数以改善模型性能
    hidden_size = 525
    activation = 'relu'
    output_activation='h-swish'
    learning_rate = 0.0001
    num_epochs = 200000000

    # Initialize the neural network model，并转至 device
    model = NeuralNetwork(input_size=X_train.shape[1],
                          hidden_size=hidden_size, 
                          output_size=y_train.shape[1],
                          activation=activation,
                          output_activation=output_activation).to(device)

    # Removed mixed precision conversion to avoid FP16 gradient unscale error
    # if device.type == 'cuda':
    #     model.half()   # 将模型参数转为半精度

    # Train the model（调用时传入 device 参数），增加训练轮数100000
    train_model(model, X_train, y_train, X_test, y_test, 
                num_epochs=num_epochs, 
                device=device, 
                learning_rate=learning_rate)
    torch.save(model.state_dict(), "trained_model.pth")
    print("模型已保存至 trained_model.pth")

    # 评估及可视化预测结果（推理时也可使用 AMP）
    model.eval()
    X_test_t = torch.tensor(X_test, dtype=torch.float32).to(device)
    # Removed half conversion for X_test_t to avoid FP16 gradient unscale error
    # if device.type == 'cuda':
    #     X_test_t = X_test_t.half()
    with torch.no_grad():
        y_pred_t = model(X_test_t)
    y_pred = y_pred_t.cpu().detach().numpy()

    # 可视化预测结果与期望值的散点图
    num_outputs = y_test.shape[1]
    plt.figure(figsize=(12, 5))
    for i in range(num_outputs):
        plt.subplot(1, num_outputs, i + 1)
        plt.scatter(y_test[:, i], y_pred[:, i], alpha=0.5, c='blue', label="预测值 vs 期望值")
        min_val = min(y_test[:, i].min(), y_pred[:, i].min())
        max_val = max(y_test[:, i].max(), y_pred[:, i].max())
        plt.plot([min_val, max_val], [min_val, max_val], 'r--', lw=2, label="理想线")
        plt.xlabel(f"期望值 (Y{i+1})")
        plt.ylabel(f"预测值 (Y{i+1})")
        plt.title(f"因变量 {i+1} 分布图")
        plt.legend()
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    # 新增命令行参数解析
    parser = argparse.ArgumentParser(description="Neural Network Application")
    parser.add_argument("--file", type=str, default="servo_pen_coordinates_02_mini.csv", help="Path to the dataset CSV file")
    parser.add_argument("--cuda", action="store_true", help="Use CUDA if available")
    args = parser.parse_args()
    main(args.file, args.cuda)