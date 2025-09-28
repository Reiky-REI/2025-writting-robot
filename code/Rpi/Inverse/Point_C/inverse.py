import math

def compute_phi(xF, yF):
    # 已知参数
    l1 = 8.5   # 连杆AB
    l2 = 13    # 连杆BC
    l3 = 13    # 连杆CD
    l4 = 8.5   # 连杆DE
    l5 = 11.5  # 固定杆A
    # l6 = 3.5   # 杆FG (未在计算中使用)
    # m = 2.5    # G距笔的距离 (未在计算中使用)
    
    # 定义一个很小的数 epsilon，用于避免除0和负判别式
    eps = 1e-12
    
    xC = xF

    # 补偿程序
    # yF = yC + (9.76 - 0.28 * yC) # = 9.76 - 0.28 * yC + yC = 9.76 + 0.72 * yC
    yC = (yF - 9.76) / 0.72

    # 计算方程 (5) 的系数，求 phi1
    d1 = -2 * l1 * yC
    e1 = -2 * l1 * xC
    f1 = xC**2 + yC**2 + l1**2 - l2**2
    discriminant1 = d1**2 + e1**2 - f1**2
    if discriminant1 < 0:
        discriminant1 = eps
    sqrt_discriminant1 = math.sqrt(discriminant1)
    denominator1 = e1 - f1
    if abs(denominator1) < eps:
        denominator1 = eps
    # 计算 phi1 的两个可能解（单位：弧度）
    phi1_1 = 2 * math.atan((d1 + sqrt_discriminant1) / denominator1)
    phi1_2 = 2 * math.atan((d1 - sqrt_discriminant1) / denominator1)
    # 转换为角度
    phi1_1_deg = math.degrees(phi1_1)
    phi1_2_deg = math.degrees(phi1_2)
    
    # 输出 phi1 的解（角度）
    print('phi1 的第一个解: {:.4f} 度'.format(phi1_1_deg))
    print('phi1 的第二个解: {:.4f} 度'.format(phi1_2_deg))
    
    # 计算方程 (6) 的系数，求 phi4
    d2 = -2 * l4 * yC
    e2 = -2 * l4 * l5 - 2 * xC
    f2 = xC**2 + yC**2 + l4**2 - l3**2 + l5**2 - 2 * xC * l5
    discriminant2 = d2**2 + e2**2 - f2**2
    if discriminant2 < 0:
        discriminant2 = eps
    sqrt_discriminant2 = math.sqrt(discriminant2)
    denominator2 = e2 - f2
    if abs(denominator2) < eps:
        denominator2 = eps
    # 计算 phi4 的两个可能解（单位：弧度）
    phi4_1 = 2 * math.atan((d2 + sqrt_discriminant2) / denominator2)
    phi4_2 = 2 * math.atan((d2 - sqrt_discriminant2) / denominator2)
    # 转换为角度
    phi4_1_deg = math.degrees(phi4_1)
    phi4_2_deg = math.degrees(phi4_2)
    
    # 输出 phi4 的解（角度）
    print('phi4 的第一个解: {:.4f} 度'.format(phi4_1_deg))
    print('phi4 的第二个解: {:.4f} 度'.format(phi4_2_deg))

if __name__ == '__main__':
    # 例: 使用 xC = 5.7500, yC = 20.6965 进行计算
    xC = -7.5
    yC = 20.0
    compute_phi(xC, yC)