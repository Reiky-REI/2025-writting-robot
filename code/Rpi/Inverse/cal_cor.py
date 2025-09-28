import math

def cal_cor(xC, yC):
    """
    根据给定的 C 点坐标 (xC, yC) 计算并输出 phi1 和 phi4 的两个解（以度为单位）
    
    参数:
      xC: C 点的 x 坐标
      yC: C 点的 y 坐标
    """
    # 已知参数
    l1 = 8.5   # 连杆AB
    l2 = 13    # 连杆BC
    l3 = 13    # 连杆CD
    l4 = 8.5   # 连杆DE
    l5 = 11.5  # 固定杆A
    l6 = 3.5   # 杆FG

    # 固定的 F 点坐标与参数
    xF = 1.9318   # F 点横坐标
    yF = 24.4958  # F 点纵坐标
    m  = 4        # G距笔的距离
    
    # 计算 phi2
    d3 = -2 * l2 * yC
    e3 = -2 * l2 * xC
    f3 = xC**2 + yC**2 + l2**2 - l1**2
    discriminant3 = d3**2 + e3**2 - f3**2
    sqrt_discriminant3 = math.sqrt(discriminant3)
    denominator3 = e3 - f3
    phi2 = 2 * math.atan((d3 + sqrt_discriminant3) / denominator3)
    
    # 计算 phi0
    phi0 = 2 * math.atan(yF / xF)
    
    # 计算 l7 和 l8
    l7 = (math.pi - phi2 - phi0) * l6
    l8 = 6 - l7 + m
    
    # 计算 phi1 的系数并求解 phi1（两个可能解）
    d1 = 2 * l1 * yF - 2 * l1 * l8 * math.cos(phi0)
    e1 = 2 * l1 * xF - 2 * l1 * l8 * math.sin(phi0)
    f1 = xF**2 + yF**2 + l1**2 + l8**2 - l2**2 - 2 * xF * l8 * math.cos(phi0) + 2 * yF * l8 * math.sin(phi0)
    discriminant1 = d1**2 + e1**2 - f1**2
    sqrt_discriminant1 = math.sqrt(discriminant1)
    denominator1 = e1 - f1
    phi1_1 = 2 * math.atan((d1 + sqrt_discriminant1) / denominator1)
    phi1_2 = 2 * math.atan((d1 - sqrt_discriminant1) / denominator1)
    phi1_1_deg = math.degrees(phi1_1)
    phi1_2_deg = math.degrees(phi1_2)
    
    print(f'phi1 的第一个解: {phi1_1_deg:.4f} 度')
    print(f'phi1 的第二个解: {phi1_2_deg:.4f} 度')
    
    # 计算 phi4 的系数并求解 phi4（两个可能解）
    d2 = 2 * l4 * yF - 2 * l4 * l8 * math.cos(phi0) - 2 * l4 * l5
    e2 = 2 * l4 * xF - 2 * l4 * l8 * math.sin(phi0)
    f2 = xF**2 + yF**2 + l4**2 + l8**2 + l5**2 - l3**2 - 2 * xF * l8 * math.cos(phi0) - 2 * l5 * xF + 2 * l8 * l5 * math.cos(phi0) - 2 * l8 * xF * math.sin(phi0)
    discriminant2 = d2**2 + e2**2 - f2**2
    sqrt_discriminant2 = math.sqrt(discriminant2)
    denominator2 = e2 - f2
    phi4_1 = 2 * math.atan((d2 + sqrt_discriminant2) / denominator2)
    phi4_2 = 2 * math.atan((d2 - sqrt_discriminant2) / denominator2)
    phi4_1_deg = math.degrees(phi4_1)
    phi4_2_deg = math.degrees(phi4_2)
    
    print(f'phi4 的第一个解: {phi4_1_deg:.4f} 度')
    print(f'phi4 的第二个解: {phi4_2_deg:.4f} 度')

if __name__ == "__main__":
    # 示例调用：提供 C 点的坐标（示例数据）
    xC = 7  # C 点 x 坐标示例
    yC = 26   # C 点 y 坐标示例
    cal_cor(xC, yC)