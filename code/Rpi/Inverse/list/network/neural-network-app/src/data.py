import pandas as pd
from sklearn.model_selection import train_test_split

def load_data(file_path="servo_pen_coordinates_02_mini.csv"):
    df = pd.read_csv(file_path, header=None, skiprows=1)
    df = df.dropna()
    # 选取前两列作为自变量，后两列作为因变量
    X = df.iloc[:, 0:2].values
    y = df.iloc[:, 2:4].values
    return train_test_split(X, y, test_size=0.2, random_state=42)

def preprocess_data(data):
    X = data.iloc[:, :-1].values  # 假设最后一列是目标变量
    y = data.iloc[:, -1].values
    
    # 数据归一化
    scaler = StandardScaler()
    X_scaled = scaler.fit_transform(X)
    
    return train_test_split(X_scaled, y, test_size=0.2, random_state=42)

def get_data(file_path):
    data = load_data(file_path)
    return preprocess_data(data)