#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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

    // 固定参数（单位：毫米或其它统一单位）
    const float l1  = 90.0f;
    const float l2  = 130.0f;
    const float l3  = 130.0f;
    const float l4  = 90.0f;
    const float l5  = 105.0f;
    const float l6  = 35.0f;
    const float lM  = 95.0f;

    // *************************************************************************
    // 【注意】：
    // 公式中涉及中间变量 x_c, y_c 为工作坐标系坐标，通常由笔尖坐标转换得到。
    // 此处为简化计算，假设工作坐标系与笔尖坐标系重合，即：
    float x_c = x_pen;
    float y_c = y_pen;
    // *************************************************************************

    // 计算 φ₀ = arctan( y_pen / (x_pen - l5/2) )
    float phi0 = atan( y_pen / ( x_pen - l5 / 2.0f ) );

    /************* 计算舵机1角度 φ₁ ******************/
    float d1 = 2.0f * l2 * x_pen - 2.0f * l2 * lM * cos(phi0);
    float e1 = 2.0f * l2 * y_pen - 2.0f * l2 * lM * sin(phi0);
    float f1 = x_pen * x_pen + y_pen * y_pen + l2 * l2 + lM * lM - l1 * l1
               - 2.0f * lM * x_pen * cos(phi0) + 2.0f * lM * y_pen * sin(phi0);
    float discriminant1 = d1 * d1 + e1 * e1 - f1 * f1;
    discriminant1 = discriminant1 < 0 ? 0 : discriminant1;
    float phi1 = 2.0f * atan((d1 + sqrt(discriminant1)) / (e1 - f1));
    // 转换 φ₁ 从弧度到角度
    phi1 = phi1 * 180.0f / 3.14159265358979323846f;

    /************* 计算舵机4角度 φ₄ ******************/
    float d2 = 2.0f * l4 * y_pen - 2.0f * l4 * lM * sin(phi0);
    float e2 = 2.0f * l4 * x_pen - 2.0f * l4 * lM * cos(phi0) - 2.0f * l4 * l5;
    float f2 = x_pen * x_pen + y_pen * y_pen + l4 * l4 + lM * lM + l5 * l5 - l3 * l3
               - 2.0f * lM * x_pen * cos(phi0) - 2.0f * l5 * x_pen + 2.0f * lM * l5 * cos(phi0)
               - 2.0f * lM * x_pen * sin(phi0);
    float discriminant2 = d2 * d2 + e2 * e2 - f2 * f2;
    discriminant2 = discriminant2 < 0 ? 0 : discriminant2;
    float phi4 = 2.0f * atan((d2 + sqrt(discriminant2)) / (e2 - f2));
    // 转换 φ₄ 从弧度到角度
    phi4 = phi4 * 180.0f / 3.14159265358979323846f;

    /************* 返回计算结果 ******************/
    angle[0] = phi1;
    angle[1] = phi4;
    return angle;
}

// 以下 main() 函数为测试代码，可编译运行验证 cla_angle() 功能
// #ifdef TEST_MAIN
int main() {
    float x, y;
    while (1) {
        printf("Enter x and y (Ctrl+C to exit): ");
        if (scanf("%f %f", &x, &y) != 2) {
            printf("Invalid input. Exiting...\n");
            break;
        }

        float* angle = cla_angle(x, y);
        printf("Motor 1 angle: %f\n", angle[0]);
        printf("Motor 4 angle: %f\n\n", angle[1]);
    }
    return 0;
}
// #endif
