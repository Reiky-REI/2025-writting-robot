import argparse
import os
import torch
import pandas as pd
import numpy as np
from model import NeuralNetwork

def load_new_data(file_path):
    df = pd.read_csv(file_path, header=None)
    X_new = df.iloc[:, 0:2].values
    return X_new

def predict(file_path, model_path=r"C:\Users\reiky\Documents\2025-writing-robot\code\Rpi\Inverse\list\network\neural-network-app\src\trained_model.pth"):
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    if not os.path.exists(model_path):
        raise FileNotFoundError(f"模型文件 {model_path} 未找到，请检查路径。")
    
    X_new = load_new_data(file_path)
    input_size = X_new.shape[1]
    hidden_size = 500
    output_size = 2
    activation = 'relu'
    output_activation = 'h-swish'
    
    model = NeuralNetwork(input_size, hidden_size, output_size, activation, output_activation).to(device)
    model.load_state_dict(torch.load(model_path, map_location=device))
    model.eval()
    
    X_tensor = torch.tensor(X_new, dtype=torch.float32).to(device)
    with torch.no_grad():
        y_pred = model(X_tensor)
        
    return y_pred.cpu().numpy()

def predict_angle(x, y, model_path=r"trained_model.pth"):
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    if not os.path.exists(model_path):
        raise FileNotFoundError(f"模型文件 {model_path} 未找到，请检查路径。")
    
    input_data = torch.tensor([[x, y]], dtype=torch.float32).to(device)
    model = NeuralNetwork(input_size=2, hidden_size=500, output_size=2, 
                            activation='relu', output_activation='h-swish').to(device)
    model.load_state_dict(torch.load(model_path, map_location=device))
    model.eval()
    
    with torch.no_grad():
        pred = model(input_data)
    
    return pred.cpu().numpy()[0]

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="神经网络角度预测")
    parser.add_argument("--file", type=str, default="new_data.csv", help="CSV 数据文件路径")
    parser.add_argument("--x", type=float, help="输入的 x 坐标")
    parser.add_argument("--y", type=float, help="输入的 y 坐标")
    parser.add_argument("--model", type=str, default=r"trained_model.pth", help="模型文件路径")
    args = parser.parse_args()
    
    if args.x is not None and args.y is not None:
        angles = predict_angle(args.x, args.y, model_path=args.model)
        print(f"输入 (x, y)=({args.x}, {args.y}) 的预测角度为: {angles}")
    else:
        predictions = predict(args.file, model_path=args.model)
        print("预测结果：", predictions)