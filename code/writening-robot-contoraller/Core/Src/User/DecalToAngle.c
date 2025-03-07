#include"DecalToAngle.h"

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
float* cla_angle(float x_pen, float y_pen) {
    // 静态数组保证在函数返回后数据依然有效
    static float angle[2];

    // *************************************************************************
    // 【注意】：
    // 公式中涉及中间变量 x_c, y_c 为工作坐标系坐标，通常由笔尖坐标转换得到。
    // 此处为简化计算，假设工作坐标系与笔尖坐标系重合，即：
    float x_c = x_pen;
    float y_c = y_pen;
    // *************************************************************************

    /************* 计算舵机1角度 θ₁ ******************/
    // d₁ = -2 * l₁ * y_c
    float d1 = -2.0f * l1 * y_c;
    // e₁ = -2 * l₁ * x_c
    float e1 = -2.0f * l1 * x_c;
    // f₁ = x_c² + y_c² + l₁² - l₂²
    float f1 = x_c * x_c + y_c * y_c + l1 * l1 - l2 * l2;
    // 计算判别式，保证根号内非负（若出现负值，则置为0）
    float discriminant1 = d1 * d1 + e1 * e1 - f1 * f1;
    discriminant1 = discriminant1 < 0 ? 0 : discriminant1;
    // 根据公式：θ₁ = 2·arctan((d₁ ± √(d₁²+e₁²−f₁²))/(e₁ − f₁))
    // 此处选择正号分支，实际应用中可根据需要选择正负分支。
    float theta_1 = 2.0f * atan((d1 + sqrt(discriminant1)) / (e1 - f1));

    /************* 计算舵机4角度 θ₄ ******************/
    // d₂ = -2 * l₃ * y_c
    float d2 = -2.0f * l3 * y_c;
    // e₂ = 2 * l₄ * l₅ - 2 * x_c
    float e2 = 2.0f * l4 * l5 - 2.0f * x_c;
    // f₂ = x_c² + y_c² + l₄² - l₃² + l₅² - 2 * l₅ * x_c
    float f2 = x_c * x_c + y_c * y_c + l4 * l4 - l3 * l3 + l5 * l5 - 2.0f * l5 * x_c;
    // 计算判别式，确保根号下非负
    float discriminant2 = d2 * d2 + e2 * e2 - f2 * f2;
    discriminant2 = discriminant2 < 0 ? 0 : discriminant2;
    // 根据公式：θ₄ = 2·arctan((d₂ ± √(d₂²+e₂²−f₂²))/(e₂ − f₂))
    // 同样，这里选择正号分支。
    float theta_4 = 2.0f * atan((d2 + sqrt(discriminant2)) / (e2 - f2));

    /************* 返回计算结果 ******************/
    angle[0] = theta_1;
    angle[1] = theta_4;
    return angle;
}