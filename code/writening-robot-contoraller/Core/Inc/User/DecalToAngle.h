#ifndef __DecalToAngle_H
#define __DecalToAngle_H

#include "stm32f1xx_hal.h"
#include "main.h"
#include "stdio.h"
#include <stdint.h>

// 定义机械臂参数
#define l1 90
#define l2 130
#define l3 130
#define l4 90
#define l5 105
#define l6 35
#define lM 150


/**
 * @fn float* cla_angle(float x_pen, float y_pen)
 * @brief 通过笔尖坐标系的坐标计算舵机角度
 * 
 * 该函数根据逆运动学公式组计算从笔尖坐标 \((x_{pen},y_{pen})\) 到驱动舵机角度 \((\theta_1,\theta_4)\) 的映射值。
 * 公式中涉及的中间变量包括：
 *   - 对于舵机1（\(\theta_1\)）：  
 *        d₁ = -2·l₁·y_c  
 *        e₁ = -2·l₁·x_c  
 *        f₁ = x_c² + y_c² + l₁² - l₂²  
 *        \(\theta_1 = 2 \arctan\Bigl(\frac{d₁ \pm \sqrt{d₁^2+e₁^2-f₁^2}}{e₁-f₁}\Bigr)\)
 *   - 对于舵机4（\(\theta_4\)）：  
 *        d₂ = -2·l₃·y_c  
 *        e₂ = 2·l₄·l₅ - 2·x_c  
 *        f₂ = x_c² + y_c² + l₄² - l₃² + l₅² - 2·l₅·x_c  
 *        \(\theta_4 = 2 arctan\Bigl(\frac{d₂ \pm \sqrt{d₂^2+e₂^2-f₂^2}}{e₂-f₂}\Bigr)\)
 *
 * 固定参数根据实际测量得到：
 *   l₁ = 90,  l₂ = 130,  l₃ = 130,  l₄ = 90,  l₅ = 105,  l₆ = 35,  l_M = 150.
 *
 * @param x_pen 笔尖坐标系的 x 坐标
 * @param y_pen 笔尖坐标系的 y 坐标
 * @return float* 返回一个长度为2的数组，其中 angle[0] 为舵机1角度 θ₁，angle[1] 为舵机4角度 θ₄
 */
float* cla_angle(float x_pen, float y_pen);


#endif