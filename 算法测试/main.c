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
 * @param x_pen 笔尖坐标系的 x 坐标
 * @param y_pen 笔尖坐标系的 y 坐标
 * @return float* 返回一个长度为2的数组，其中 angle[0] 为舵机1角度 θ₁，angle[1] 为舵机4角度 θ₄
 */
float* cla_angle(float x_pen, float y_pen) {
    // 机构参数（单位均为 mm）
    const double l1 = 90.0;
    const double l2 = 130.0;
    const double l3 = 130.0;
    const double l4 = 90.0;
    const double l5 = 105.0;
    const double l6 = 35.0;
    const double lM = 95.0;

    // 初始化中间点 C 的坐标，初值取笔尖坐标（粗略估计）
    double x_c = x_pen;
    double y_c = y_pen;
    
    // 初始化角度变量
    double theta0 = 0.0, theta1 = 0.0, theta2 = 0.0, theta4 = 0.0, theta5 = 0.0;

    // 迭代参数
    const int max_iter = 100;
    const double tol = 1e-6;

    printf("迭代求解过程：\n");
    for (int i = 0; i < max_iter; i++) {
        // ① 计算 theta0，采用 atan2 保证正确性
        theta0 = atan2(y_c, x_c - l5/2.0);
        
        // ② 利用 θ₁ 反解公式计算 θ₁
        double d1 = -2.0 * l1 * y_c;
        double e1 = -2.0 * l1 * x_c;
        double f1 = x_c*x_c + y_c*y_c + l1*l1 - l2*l2;
        double disc1 = d1*d1 + e1*e1 - f1*f1;
        if(disc1 < 0) disc1 = 0;
        theta1 = 2.0 * atan((d1 + sqrt(disc1)) / (e1 - f1));

        // 计算点 B 坐标
        double x_B = l1 * cos(theta1);
        double y_B = l1 * sin(theta1);

        // ③ 利用 θ₄ 反解公式计算 θ₄
        double d2 = -2.0 * l3 * y_c;
        double e2 = 2.0 * l4 * l5 - 2.0 * x_c;
        double f2 = x_c*x_c + y_c*y_c + l4*l4 - l3*l3 + l5*l5 - 2.0 * l5 * x_c;
        double disc2 = d2*d2 + e2*e2 - f2*f2;
        if(disc2 < 0) disc2 = 0;
        theta4 = 2.0 * atan((d2 + sqrt(disc2)) / (e2 - f2));

        // 计算点 D 坐标
        double x_D = x_pen - l4 * cos(theta4);
        double y_D = y_pen - l4 * sin(theta4);

        // ④ 利用点 B、D 计算 θ₂
        double A0 = 2.0 * l2 * (x_D - x_B);
        double B0 = 2.0 * l2 * (y_D - y_B);
        double l_BD = sqrt((x_D - x_B)*(x_D - x_B) + (y_D - y_B)*(y_D - y_B));
        double C0 = l2*l2 + l_BD*l_BD - l3*l3;
        double disc0 = A0*A0 + B0*B0 - C0*C0;
        if(disc0 < 0) disc0 = 0;
        theta2 = 2.0 * atan((B0 + sqrt(disc0)) / (A0 - C0));

        // ⑤ 计算 theta5，注意 180° 换为 π
        theta5 = M_PI - theta2 - theta0;

        // ⑥ 根据 theta5 计算 l7 和 l8
        double l7 = l6 * cos(theta5);
        double l8 = lM - l7 * cos(theta5);

        // ⑦ 更新中间点 C 坐标，根据笔尖与 C 的关系
        double x_c_new = x_pen - sin(theta0) * l8;
        double y_c_new = y_pen - cos(theta0) * l8;

        // 输出当前迭代的中间变量和角度值
        printf("迭代 %d:\n", i+1);
        printf("  theta0 = %f rad\n", theta0);
        printf("  theta1 = %f rad\n", theta1);
        printf("  theta2 = %f rad\n", theta2);
        printf("  theta4 = %f rad\n", theta4);
        printf("  theta5 = %f rad\n", theta5);
        printf("  x_c = %f, y_c = %f\n", x_c_new, y_c_new);

        // 检查收敛性
        if (fabs(x_c_new - x_c) < tol && fabs(y_c_new - y_c) < tol) {
            x_c = x_c_new;
            y_c = y_c_new;
            break;
        }
        x_c = x_c_new;
        y_c = y_c_new;
    }

    // 分配返回数组（调用者使用后需释放）
    float* angle = (float*)malloc(2 * sizeof(float));
    if(angle == NULL) {
        printf("Memory allocation error.\n");
        exit(-1);
    }
    angle[0] = (float)theta1;
    angle[1] = (float)theta4;

    // 输出最终求得的角度值
    printf("最终求得：\n");
    printf("  舵机1角度 (θ₁) = %f rad\n", theta1);
    printf("  舵机4角度 (θ₄) = %f rad\n", theta4);

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
