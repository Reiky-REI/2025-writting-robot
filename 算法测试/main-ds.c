#include <math.h>
#include <stdio.h>
#include <assert.h>

#define PI 3.14159265358979323846f
#define MAX_ITER 50
#define TOLERANCE 1e-5f

// 机构参数
const float l1 = 90, l2 = 130, l3 = 130, l4 = 90, l5 = 105, l6 = 35, lM = 95;

// 角度规范化函数（输出到0°~180°）
float normalize_angle(float deg) {
    deg = fmodf(deg, 360);        // 先转换到0°~360°
    if (deg < 0) deg += 360;      // 处理负数
    return (deg <= 180) ? deg : deg - 360; // 超过180°则返回等效负角度
}

/* 计算θ₁和θ₄（弧度）并选择有效解 */
void calculate_angles(float x_c, float y_c, float* theta1, float* theta4) {
    // θ₁计算（两解选择）
    float d1 = -2 * l1 * y_c;
    float e1 = -2 * l1 * x_c;
    float f1 = x_c*x_c + y_c*y_c + l1*l1 - l2*l2;
    float sqrt1 = sqrtf(d1*d1 + e1*e1 - f1*f1);
    
    // 计算两个可能的解
    float sol1 = 2 * atan2f(d1 + sqrt1, e1 - f1);
    float sol2 = 2 * atan2f(d1 - sqrt1, e1 - f1);
    
    // 选择0°~180°范围内的解
    *theta1 = (fabsf(normalize_angle(sol1*180/PI)) <= 180 ? sol1 : sol2);

    // θ₄计算（同样处理两解）
    float dx = x_c - l5;
    float d2 = -2 * l3 * y_c;
    float e2 = 2 * l4 * dx;
    float f2 = dx*dx + y_c*y_c + l4*l4 - l3*l3;
    float sqrt2 = sqrtf(d2*d2 + e2*e2 - f2*f2);
    
    float sol4_1 = 2 * atan2f(d2 + sqrt2, e2 - f2);
    float sol4_2 = 2 * atan2f(d2 - sqrt2, e2 - f2);
    *theta4 = (fabsf(normalize_angle(sol4_1*180/PI)) <= 180 ? sol4_1 : sol4_2);
}

/* 主计算函数（返回规范化角度） */
float* cla_angle(float x_pen, float y_pen) {
    static float angles[4]; // [θ₁, θ₂, θ₃, θ₄]
    float x_c = x_pen, y_c = y_pen; // 初始猜测
    float theta1_rad, theta4_rad, theta2_rad, theta3_rad;

    for (int iter = 0; iter < MAX_ITER; ++iter) {
        // 计算θ₀（笔尖朝向角）
        float theta0 = atan2f(y_c, x_c - l5/2);

        // 计算θ₁和θ₄
        calculate_angles(x_c, y_c, &theta1_rad, &theta4_rad);

        // 计算B点和D点坐标（修正后）
        float Bx = l1 * cosf(theta1_rad);
        float By = l1 * sinf(theta1_rad);
        float Dx = l5 + l4 * cosf(theta4_rad);
        float Dy = l4 * sinf(theta4_rad);

        // 计算θ₂（B-C-D夹角）
        float dx_BC = x_c - Bx;
        float dy_BC = y_c - By;
        theta2_rad = atan2f(dy_BC, dx_BC) - theta1_rad;

        // 计算θ₃（D-C-E夹角）
        float dx_DC = x_c - Dx;
        float dy_DC = y_c - Dy;
        theta3_rad = atan2f(dy_DC, dx_DC) - theta4_rad;

        // 计算延伸长度l8
        float theta5 = PI - theta2_rad - theta0;
        float l7 = l6 * cosf(theta5);
        float l8 = lM - l7;

        // 计算残差
        float x_err = x_c + sinf(theta0)*l8 - x_pen;
        float y_err = y_c + cosf(theta0)*l8 - y_pen;

        // 更新C点坐标
        if (fabsf(x_err) < TOLERANCE && fabsf(y_err) < TOLERANCE) break;
        x_c -= 0.3 * x_err;
        y_c -= 0.3 * y_err;
    }

    // 最终规范化输出
    angles[0] = normalize_angle(theta1_rad * 180/PI);
    angles[3] = normalize_angle(theta4_rad * 180/PI);
    angles[1] = normalize_angle(theta2_rad * 180/PI);
    angles[2] = normalize_angle(theta3_rad * 180/PI);

    return angles;
}

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
