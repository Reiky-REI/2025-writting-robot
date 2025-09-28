import math

def compute_phi(xC, yC):
    # 已知参数
    l1 = 8.2; l2 = 13.3; l3 = 13.3; l4 = 8.2; l5 = 11;
    
    # 计算phi1的解并转换为非负角度
    d1 = -2 * l1 * yC; e1 = -2 * l1 * xC; f1 = xC**2 + yC**2 + l1**2 - l2**2;
    discriminant1 = max(d1**2 + e1**2 - f1**2, 1e-10)
    sqrt_discriminant1 = math.sqrt(discriminant1)
    denominator1 = e1 - f1; 
    if abs(denominator1) < 1e-10: denominator1 = 1e-10
    
    phi1_1 = 2 * math.atan((d1 + sqrt_discriminant1) / denominator1)
    phi1_2 = 2 * math.atan((d1 - sqrt_discriminant1) / denominator1)
    phi1_1_deg = math.fmod(math.degrees(phi1_1), 360) # 确保非负
    phi1_2_deg = math.fmod(math.degrees(phi1_2), 360)

    # 计算phi4的解并转换为非负角度
    d2 = -2 * l4 * yC; e2 = -2 * l4 * l5 - 2 * xC; 
    f2 = xC**2 + yC**2 + l4**2 - l3**2 + l5**2 - 2 * xC * l5;
    discriminant2 = max(d2**2 + e2**2 - f2**2, 1e-10)
    sqrt_discriminant2 = math.sqrt(discriminant2)
    denominator2 = e2 - f2; 
    if abs(denominator2) < 1e-10: denominator2 = 1e-10
    
    phi4_1 = 2 * math.atan((d2 + sqrt_discriminant2) / denominator2)
    phi4_2 = 2 * math.atan((d2 - sqrt_discriminant2) / denominator2)
    phi4_1_deg = math.fmod(math.degrees(phi4_1), 360)
    phi4_2_deg = math.fmod(math.degrees(phi4_2), 360)
    
    # 所有可能的角度对
    angles_pairs = [(phi1_1_deg, phi4_1_deg), (phi1_1_deg, phi4_2_deg),
                    (phi1_2_deg, phi4_1_deg), (phi1_2_deg, phi4_2_deg)]
    
    # 初始化最大距离和对应的角度对
    max_distance = 0
    angle1 = 0; angle2 = 0
    
    # 遍历所有可能的角度对，寻找相距最远的一对
    for pair in angles_pairs:
        distance = abs(pair[0] - pair[1])
        if distance > max_distance:
            max_distance = distance
            angle1 = pair[0]
            angle2 = pair[1]
    
    return angle1, angle2