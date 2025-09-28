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
from sklearn.preprocessing import PolynomialFeatures  # 新增导入

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
# 4. 训练与评估多项式回归模型 (degree=2)
# ---------------------------
poly = PolynomialFeatures(degree=2)
X_train_poly = poly.fit_transform(X_train)
X_test_poly = poly.transform(X_test)

model_poly = LinearRegression()
model_poly.fit(X_train_poly, Y_train)
Y_pred_poly = model_poly.predict(X_test_poly)

mse_poly = mean_squared_error(Y_test, Y_pred_poly)
r2_poly = r2_score(Y_test, Y_pred_poly)
print("\n=== 多项式回归（非线性拟合）评估结果 ===")
print(f"全局 MSE: {mse_poly:.4f}, R²: {r2_poly:.4f}")
for i in range(Y.shape[1]):
    mse_col = mean_squared_error(Y_test[:, i], Y_pred_poly[:, i])
    r2_col = r2_score(Y_test[:, i], Y_pred_poly[:, i])
    print(f"因变量 {i+1} -> MSE: {mse_col:.4f}, R²: {r2_col:.4f}")

# ---------------------------
# 4. 训练与评估三角函数回归模型
# ---------------------------
import numpy as np  # 如果尚未导入，可保留

def trig_features(X):
    # 对输入X进行限定在[-1,1]范围内，然后构造新特征：原始自变量和 arccos(X)
    X_clip = np.clip(X, -1, 1)
    return np.concatenate([X, np.arccos(X_clip)], axis=1)

# 使用 trig_features 构造新特征
X_train_trig = trig_features(X_train)
X_test_trig = trig_features(X_test)

model_trig = LinearRegression()
model_trig.fit(X_train_trig, Y_train)
Y_pred_trig = model_trig.predict(X_test_trig)

mse_trig = mean_squared_error(Y_test, Y_pred_trig)
r2_trig = r2_score(Y_test, Y_pred_trig)
print("\n=== 三角函数回归评估结果（使用 arccos） ===")
print(f"全局 MSE: {mse_trig:.4f}, R²: {r2_trig:.4f}")
for i in range(Y.shape[1]):
    mse_col = mean_squared_error(Y_test[:, i], Y_pred_trig[:, i])
    r2_col = r2_score(Y_test[:, i], Y_pred_trig[:, i])
    print(f"因变量 {i+1} -> MSE: {mse_col:.4f}, R²: {r2_col:.4f}")

# 可视化预测结果对比
plt.figure(figsize=(12, 5))
for i in range(Y.shape[1]):
    plt.subplot(1, 2, i+1)
    plt.scatter(Y_test[:, i], Y_pred_trig[:, i], alpha=0.5, c='green')
    min_val = min(Y_test[:, i].min(), Y_pred_trig[:, i].min())
    max_val = max(Y_test[:, i].max(), Y_pred_trig[:, i].max())
    plt.plot([min_val, max_val], [min_val, max_val], 'k--', lw=2)
    plt.xlabel(f"实际值 (Y{i+1})")
    plt.ylabel(f"预测值 (Y{i+1})")
    plt.title(f"三角函数回归（arccos）：因变量 {i+1} 预测对比")
plt.tight_layout()
plt.show()

# 输出回归方程（新特征顺序为：[X1, X2, arccos(X1), arccos(X2)]）
print("\n=== 回归方程（三角函数，使用 arccos） ===")
feature_names = [f"X{i+1}" for i in range(X.shape[1])]
feature_names += [f"arccos(X{i+1})" for i in range(X.shape[1])]
for i in range(Y.shape[1]):
    equation_terms = [f"({coef:.3f}*{term})" for coef, term in zip(model_trig.coef_[i], feature_names)]
    equation = f"Y{i+1} = {model_trig.intercept_[i]:.3f} + " + " + ".join(equation_terms)
    print(equation)

# ---------------------------
# 5. 可视化与参数解读
# ---------------------------
# 绘制预测结果对比（更新虚线效果）
plt.figure(figsize=(12, 5))
for i in range(2):
    plt.subplot(1, 2, i+1)
    plt.scatter(Y_test[:, i], Y_pred_poly[:, i], alpha=0.5, c=['blue','red'][i])
    min_val = min(Y_test[:, i].min(), Y_pred_poly[:, i].min())
    max_val = max(Y_test[:, i].max(), Y_pred_poly[:, i].max())
    plt.plot([min_val, max_val], [min_val, max_val], 'k--', lw=2)
    plt.xlabel(f"实际值 (Y{i+1})")
    plt.ylabel(f"预测值 (Y{i+1})")
    plt.title(f"因变量 {i+1} 预测对比")
plt.tight_layout()
plt.show()

# 输出多项式回归方程（各项系数对应多项式特征）
print("\n=== 回归方程（多项式） ===")
terms = poly.get_feature_names_out(["X1", "X2"])
for i in range(Y.shape[1]):
    equation_terms = [f"({coef:.3f}*{term})" for coef, term in zip(model_poly.coef_[i], terms)]
    equation = f"Y{i+1} = {model_poly.intercept_[i]:.3f} + " + " + ".join(equation_terms)
    print(equation)