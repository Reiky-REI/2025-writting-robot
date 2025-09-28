import numpy as np
import torch
import torch.nn as nn
import torch.optim as optim
import torch.cuda.amp as amp
import msvcrt  # 新增模块用于键盘检测
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from data import load_data

class NeuralNetwork(nn.Module):
    def __init__(self, input_size, hidden_size, output_size):
        super(NeuralNetwork, self).__init__()
        self.fc1 = nn.Linear(input_size, hidden_size)
        self.fc2 = nn.Linear(hidden_size, output_size)
        self.relu = nn.ReLU()

    def forward(self, x):
        x = self.fc1(x)
        x = self.relu(x)
        x = self.fc2(x)
        return x

def train_model(model, X_train, y_train, X_test, y_test, num_epochs=200, learning_rate=0.001, device=torch.device('cpu')):
    # 转换数据为张量，并转至 device（异步传输）
    X_train_t = torch.tensor(X_train, dtype=torch.float32).to(device, non_blocking=True)
    y_train_t = torch.tensor(y_train, dtype=torch.float32).to(device, non_blocking=True)
    X_test_t = torch.tensor(X_test, dtype=torch.float32).to(device, non_blocking=True)
    y_test_t = torch.tensor(y_test, dtype=torch.float32).to(device, non_blocking=True)

    # Removed explicit half precision conversion to avoid FP16 gradient unscale error

    # 定义优化器和损失函数
    optimizer = optim.Adam(model.parameters(), lr=learning_rate)
    criterion = nn.MSELoss()

    # 创建GradScaler用于 AMP
    scaler = amp.GradScaler()

    overfit_count = 0
    best_test_loss = float('inf')

    for epoch in range(num_epochs):
        # 每轮开始检测键盘输入，若按下 'q' 则退出训练
        if msvcrt.kbhit():
            key = msvcrt.getch()
            if key.lower() == b'q':
                print("收到 'q' 按键，停止训练。")
                break

        model.train()
        optimizer.zero_grad()
        with amp.autocast(enabled=(device.type=='cuda')):
            output = model(X_train_t)
            loss = criterion(output, y_train_t)
        scaler.scale(loss).backward()
        scaler.step(optimizer)
        scaler.update()

        # 在每个 epoch 后评估模型
        model.eval()
        with torch.no_grad():
            with amp.autocast(enabled=(device.type=='cuda')):
                test_output = model(X_test_t)
                test_loss = criterion(test_output, y_test_t)
        if epoch % 10 == 0 or epoch == num_epochs - 1:
            print(f"Epoch {epoch}: Train Loss: {loss.item():.4f}, Test Loss: {test_loss.item():.4f}")

        if test_loss < best_test_loss:
            best_test_loss = test_loss
            overfit_count = 0
        else:
            overfit_count += 1

        # if overfit_count >= 25000:
        #     print("检测到过拟合，停止训练并保存数据集。")
        #     import pandas as pd
        #     train_data = np.hstack((X_train, y_train))
        #     pd.DataFrame(train_data).to_csv("overfit_train_data.csv", index=False)
        #     break

def evaluate_model(model, test_loader):
    model.eval()
    with torch.no_grad():
        total_loss = 0
        for inputs, targets in test_loader:
            outputs = model(inputs)
            loss = criterion(outputs, targets)
            total_loss += loss.item()
    return total_loss / len(test_loader)

if __name__ == "__main__":
    # Load data
    X, y = load_data()
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

    # Normalize data
    scaler = StandardScaler()
    X_train = scaler.fit_transform(X_train)
    X_test = scaler.transform(X_test)

    # Convert to PyTorch tensors
    X_train_tensor = torch.FloatTensor(X_train)
    y_train_tensor = torch.FloatTensor(y_train)
    X_test_tensor = torch.FloatTensor(X_test)
    y_test_tensor = torch.FloatTensor(y_test)

    # Create data loaders with pinned memory to speed up CPU->GPU transfer
    train_loader = torch.utils.data.DataLoader(
        torch.utils.data.TensorDataset(X_train_tensor, y_train_tensor),
        batch_size=32, shuffle=True, pin_memory=True
    )
    test_loader = torch.utils.data.DataLoader(
        torch.utils.data.TensorDataset(X_test_tensor, y_test_tensor),
        batch_size=32, shuffle=False, pin_memory=True
    )

    # Initialize model, criterion, and optimizer
    input_size = X_train.shape[1]
    hidden_size = 64
    output_size = y_train.shape[1]
    model = NeuralNetwork(input_size, hidden_size, output_size)
    criterion = nn.MSELoss()
    optimizer = optim.Adam(model.parameters(), lr=0.001)

    # Train the model
    num_epochs = 100
    train_model(model, X_train, y_train, X_test, y_test)

    # Evaluate the model
    test_loss = evaluate_model(model, test_loader)
    print(f'Test Loss: {test_loss:.4f}')