# -*- coding: utf-8 -*-
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
# 添加以下代码支持中文显示
plt.rcParams['font.sans-serif'] = ['SimHei']
plt.rcParams['axes.unicode_minus'] = False
from sklearn.model_selection import train_test_split
from sklearn.linear_model import LinearRegression
from sklearn.metrics import mean_squared_error, r2_score

# ---------------------------
# 1. 加载数据（跳过第一行ID）
# ---------------------------
file_path = "servo_pen_coordinates_02_mini.csv"
# 使用 skiprows=1 跳过首行（ID行），header=None 避免将第一行视为列名
df = pd.read_csv(file_path, header=None, skiprows=1)
# 删除缺失值所在的行，避免后续模型训练出错
df = df.dropna()
# 新增数据描述输出，查看数据统计特性
print("数据描述:")
print(df.describe())

# 检查数据（前5行，不含ID）
print("数据前5行（已跳过ID行）:")
print(df.head())

# ---------------------------
# 2. 提取自变量和因变量
# ---------------------------
# 假设数据格式为：[0:ID, 1:自变量1, 2:自变量2, 3:因变量1, 4:因变量2]（根据实际情况调整列索引）
# 注意：因为跳过了ID行，现在数据列索引从0开始，且不包含原来的ID列
# 因此：
# - 自变量为第0、1列（原CSV的第2、3列）
# - 因变量为第2、3列（原CSV的第4、5列）
X = df.iloc[:, 0:2].values  # 取前两列作为自变量 (N,2)
Y = df.iloc[:, 2:4].values  # 取后两列作为因变量 (N,2)

# ---------------------------
# 3. 分割数据集（训练集80% / 测试集20%）
# ---------------------------
X_train, X_test, Y_train, Y_test = train_test_split(
    X, 
    Y, 
    test_size=0.2, 
    random_state=42
)
print(f"\n数据集分割：")
print(f"- 训练集样本: {X_train.shape[0]}")
print(f"- 测试集样本: {X_test.shape[0]}")

# ---------------------------
# 4. 训练与评估模型
# ---------------------------
model = LinearRegression()
model.fit(X_train, Y_train)
Y_pred = model.predict(X_test)

# 全局评估
mse = mean_squared_error(Y_test, Y_pred)
r2 = r2_score(Y_test, Y_pred)

# 各因变量单独评估
print("\n=== 评估结果 ===")
print(f"全局 MSE: {mse:.4f}, R²: {r2:.4f}")
print("\n各因变量性能:")
for i in range(Y.shape[1]):
    mse_col = mean_squared_error(Y_test[:, i], Y_pred[:, i])
    r2_col = r2_score(Y_test[:, i], Y_pred[:, i])
    print(f"因变量 {i+1} -> MSE: {mse_col:.4f}, R²: {r2_col:.4f}")

# ---------------------------
# 5. 可视化与参数解读
# ---------------------------
# 绘制预测结果对比
plt.figure(figsize=(12, 5))
for i in range(2):
    plt.subplot(1, 2, i+1)
    plt.scatter(Y_test[:, i], Y_pred[:, i], alpha=0.5, c=['blue','red'][i])
    plt.plot([Y.min(), Y.max()], [Y.min(), Y.max()], 'k--', lw=2)
    plt.xlabel(f"实际值 (Y{i+1})")
    plt.ylabel(f"预测值 (Y{i+1})")
    plt.title(f"因变量 {i+1} 预测对比")
plt.tight_layout()
plt.show()

# 输出回归方程
print("\n=== 回归方程 ===")
coef = model.coef_
intercept = model.intercept_
for i in range(2):
    eq = f"Y{i+1} = {coef[i][0]:.3f}*X1 + {coef[i][1]:.3f}*X2 + {intercept[i]:.3f}"
    print(eq)