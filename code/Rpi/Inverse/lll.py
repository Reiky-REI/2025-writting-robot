import numpy as np

# 定义已知参数
l1 = 8.5  # 连杆AB的长度
l2 = 13   # 连杆BC的长度
l3 = 13   # 连杆CD的长度
l4 = 8.5  # 连杆DE的长度
l5 = 11.5 # 固定杆A的长度
l6 = 3.5  # 杆FG的长度
m = 2.5   # G距笔的距离

# 定义角度和坐标的取值范围
# phi1 角度范围从 180 度到 360 度，共取 10 个值
phi1_range = np.linspace(np.pi, 2 * np.pi, 10)
# phi4 角度范围从 180 度到 360 度，共取 10 个值
phi4_range = np.linspace(np.pi, 2 * np.pi, 10)
# xB 坐标范围从 -14.25 到 2.75，共取 10 个值
xB_range = np.linspace(-14.25, 2.75, 10)
# yB 坐标范围从 0 到 8.5，共取 10 个值
yB_range = np.linspace(0, 8.5, 10)
# xD 坐标范围从 -2.75 到 14.25，共取 10 个值
xD_range = np.linspace(-2.75, 14.25, 10)
# yD 坐标范围从 0 到 8.5，共取 10 个值
yD_range = np.linspace(0, 8.5, 10)

# 初始化用于存储 F 点坐标的列表
xF_values = []
yF_values = []

# 提前计算一些常量，避免重复计算
l2_squared = l2 ** 2
l3_squared = l3 ** 2

# 遍历所有可能的取值组合
for phi1 in phi1_range:
    for phi4 in phi4_range:
        for xB in xB_range:
            for yB in yB_range:
                for xD in xD_range:
                    for yD in yD_range:
                        # 计算 B 点和 D 点之间的距离 lBD
                        lBD = np.sqrt((xD - xB) ** 2 + (yD - yB) ** 2)
                        # 计算辅助参数 A0、B0、C0
                        A0 = 2 * l2 * (xD - xB)
                        B0 = 2 * l2 * (yD - yB)
                        C0 = l2_squared + lBD ** 2 - l3_squared
                        # 检查是否满足条件，避免出现复数结果
                        if A0 ** 2 + B0 ** 2 - C0 ** 2 >= 0:
                            # 避免除零错误
                            denominator = A0 + C0
                            if denominator != 0:
                                # 计算角度 phi2
                                phi2 = 2 * np.arctan((B0 + np.sqrt(A0 ** 2 + B0 ** 2 - C0 ** 2)) / denominator)
                                # 计算角度 phi3
                                phi3 = np.pi - phi2
                                # 计算 C 点的坐标
                                xC = l1 * np.cos(phi1) + l2 * np.cos(phi2)
                                yC = l1 * np.sin(phi1) + l2 * np.sin(phi2)
                                # 计算 C 点的极坐标参数 L0 和 phi0
                                L0 = np.sqrt(xC ** 2 + yC ** 2)
                                phi0 = np.arctan2(yC, xC)
                                # 计算角度 phi5
                                phi5 = phi3 - phi0
                                # 计算长度 l7
                                l7 = np.cos(phi5) * l6
                                # 计算长度 l8
                                l8 = 6 - l7 + m
                                # 计算 F 点的坐标
                                xF = l1 * np.cos(phi1) + l2 * np.cos(phi2) + l8 * np.cos(phi0)
                                yF = l1 * np.sin(phi1) + l2 * np.sin(phi2) + l8 * np.sin(phi0)
                                # 仅存储 yF 大于等于 0 的点
                                if yF >= 0:
                                    # 将符合条件的 F 点坐标添加到列表中
                                    xF_values.append(xF)
                                    yF_values.append(yF)

# 将结果保存到文件中
with open('f_point_coordinates.txt', 'w') as f:
    f.write("xF, yF\n")
    for xF, yF in zip(xF_values, yF_values):
        f.write(f"{xF}, {yF}\n")
 