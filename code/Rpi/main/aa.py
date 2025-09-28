import numpy as np
from scipy.optimize import fsolve

def five_bar_ik_opt(x_PEN, y_PEN, prev_angles=None):
    """
    输入：
      x_PEN, y_PEN: 平台上 P 点的坐标
      prev_angles: (phi1_prev, phi3_prev, phi5_prev) 三个角的上一次解，单位弧度；
                   若为 None，则取默认初值
    输出：
      (phi1, phi3, phi5) 以角度制输出的解
    """
    # 定义机构常量（可根据实际情况修改）
    l1 = 82      # 连杆1长度，同时也是连杆4长度
    l2 = 133      # 连杆2长度，同时也是连杆3长度
    l5 = 110      # A到E的固定距离（A=(l5/2,0), E=(-l5/2,0)）
    l6 = 36      # 与平台副有关的常量 |CC_Ω^B| = |CC_Ω^C| = l6
    l7 = 84      # F-P间的定值距离
    
    # 若未提供上一时刻解，给出默认初值（均取 phi0 或者中间值）
    r_P = np.sqrt(x_PEN**2 + y_PEN**2)
    phi_0 = np.arctan2(y_PEN, x_PEN)
    if prev_angles is None:
        # prev_angles = (phi_0, np.pi/2, phi_0)  # 默认初值
        prev_angles = (phi_0, np.pi/2, phi_0)  # 默认初值

    phi1_prev, phi3_prev, phi5_prev = prev_angles

    # 候选 phi3 范围（注意机械约束，此处取 [0.5, 2π-0.5]）
    phi3_candidates = np.linspace(0.5, 2*np.pi - 0.5, 50)
    
    best_cost = np.inf
    best_solution = None
    
    # 对每个候选 phi3，计算 r_C 并求解闭链方程求出 phi1 和 phi5
    for phi3 in phi3_candidates:
        # 根据平台反馈，计算 r_C (单位: same as r_P)
        # 注意：l8 = l7 - l6*cos(phi3/2)，因此 r_C = r_P - l8 = r_P - l7 + l6*cos(phi3/2)
        r_C = r_P - l7 + l6 * np.cos(phi3 / 2)
        
        # 定义闭链约束函数，未知数为 [phi1, phi5]
        def equations(angles):
            phi1, phi5 = angles
            # A 点和 E 点
            A = np.array([l5/2, 0])
            E = np.array([-l5/2, 0])
            # B 点 = A + l1*(cos(phi1), sin(phi1))
            B = A + l1 * np.array([np.cos(phi1), np.sin(phi1)])
            # D 点 = E + l1*(cos(phi5), sin(phi5))
            D = E + l1 * np.array([np.cos(phi5), np.sin(phi5)])
            # C 点：由平台反馈在极坐标下确定
            C = np.array([r_C * np.cos(phi_0), r_C * np.sin(phi_0)])
            eq1 = (C[0] - B[0])**2 + (C[1] - B[1])**2 - l2**2
            eq2 = (C[0] - D[0])**2 + (C[1] - D[1])**2 - l2**2
            return [eq1, eq2]
        
        # 初始猜测可选用上一时刻对应的 phi1 和 phi5
        initial_guess = [phi1_prev, phi5_prev]
        try:
            sol = fsolve(equations, initial_guess, xtol=1e-8)
            phi1_sol, phi5_sol = sol
            # 计算成本函数：平滑性准则
            cost = (phi1_sol - phi1_prev)**2 + (phi3 - phi3_prev)**2 + (phi5_sol - phi5_prev)**2
            # 记录下成本最小的方案
            if cost < best_cost:
                best_cost = cost
                best_solution = (phi1_sol, phi3, phi5_sol)
        except Exception as e:
            # 如果求解失败，则跳过
            continue

    if best_solution is None:
        raise ValueError("未能求得满足闭链约束的解")
    
    phi1_sol, phi3_sol, phi5_sol = best_solution
    # 若需要确保机构所有元件均在y>0区域，可进一步检查并调整
    # 这里简单处理：如果 sin(phi1) < 0 或 sin(phi5) < 0，则加2π
    if np.sin(phi1_sol) < 0:
        phi1_sol += 2*np.pi
    if np.sin(phi5_sol) < 0:
        phi5_sol += 2*np.pi

    # 转换为角度制输出，并取模360°
    phi1_deg = np.degrees(phi1_sol) % 360
    phi3_deg = np.degrees(phi3_sol) % 360
    phi5_deg = np.degrees(phi5_sol) % 360

    return phi1_deg, phi3_deg, phi5_deg

# 示例调用
if __name__ == '__main__':
    # 假设当前 P 点坐标
    x_PEN = 0
    y_PEN = 264
    # 假设上一时刻解（单位：弧度），可根据实际运动历史记录
    prev_angles = (np.deg2rad(45), np.deg2rad(60), np.deg2rad(45))
    phi1_deg, phi3_deg, phi5_deg = five_bar_ik_opt(x_PEN, y_PEN, prev_angles)
    print("phi1 = {:.2f}°".format(phi1_deg))
    print("phi3 = {:.2f}°".format(phi3_deg))
    print("phi5 = {:.2f}°".format(phi5_deg))
