#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @fn float* cla_angle(float x_pen, float y_pen)
 * @brief Calculate the motor angles from the pen tip coordinates.
 *
 * @param x_pen Pen tip x coordinate.
 * @param y_pen Pen tip y coordinate.
 * @return float* Returns an array of length 2, where angle[0] is motor 1 angle (θ₁)
 *                and angle[1] is motor 4 angle (θ₄).
 */
float* cla_angle(float x_pen, float y_pen) {
    // Mechanism parameters (units: mm)
    const double l1 = 90.0;
    const double l2 = 130.0;
    const double l3 = 130.0;
    const double l4 = 90.0;
    const double l5 = 105.0;
    const double l6 = 35.0;
    const double lM = 95.0;

    // Initialize intermediate point C coordinates, using the pen tip as an initial guess.
    double x_c = x_pen;
    double y_c = y_pen;
    
    // Initialize angle variables.
    double theta0 = 0.0, theta1 = 0.0, theta2 = 0.0, theta4 = 0.0, theta5 = 0.0;

    // Iteration parameters.
    const int max_iter = 100;
    const double tol = 1e-6;

    printf("Iteration process:\n");
    for (int i = 0; i < max_iter; i++) {
        // 1. Calculate theta0 using atan2 to ensure proper quadrant determination.
        theta0 = atan2(y_c, x_c - l5/2.0);
        
        // 2. Solve for θ₁ using the inverse kinematics formula.
        double d1 = -2.0 * l1 * y_c;
        double e1 = -2.0 * l1 * x_c;
        double f1 = x_c*x_c + y_c*y_c + l1*l1 - l2*l2;
        double disc1 = d1*d1 + e1*e1 - f1*f1;
        if(disc1 < 0) disc1 = 0; // Prevent negative square root.
        theta1 = 2.0 * atan((d1 + sqrt(disc1)) / (e1 - f1));

        // Calculate point B coordinates.
        double x_B = l1 * cos(theta1);
        double y_B = l1 * sin(theta1);

        // 3. Solve for θ₄ using its inverse kinematics formula.
        double d2 = -2.0 * l3 * y_c;
        double e2 = 2.0 * l4 * l5 - 2.0 * x_c;
        double f2 = x_c*x_c + y_c*y_c + l4*l4 - l3*l3 + l5*l5 - 2.0 * l5 * x_c;
        double disc2 = d2*d2 + e2*e2 - f2*f2;
        if(disc2 < 0) disc2 = 0;
        theta4 = 2.0 * atan((d2 + sqrt(disc2)) / (e2 - f2));

        // Calculate point D coordinates.
        double x_D = x_pen - l4 * cos(theta4);
        double y_D = y_pen - l4 * sin(theta4);

        // 4. Solve for θ₂ using points B and D.
        double A0 = 2.0 * l2 * (x_D - x_B);
        double B0 = 2.0 * l2 * (y_D - y_B);
        double l_BD = sqrt((x_D - x_B)*(x_D - x_B) + (y_D - y_B)*(y_D - y_B));
        double C0 = l2*l2 + l_BD*l_BD - l3*l3;
        double disc0 = A0*A0 + B0*B0 - C0*C0;
        if(disc0 < 0) disc0 = 0;
        theta2 = 2.0 * atan((B0 + sqrt(disc0)) / (A0 - C0));

        // 5. Calculate theta5. (180° is replaced with π radians.)
        theta5 = M_PI - theta2 - theta0;

        // 6. Calculate l7 and l8 from theta5.
        double l7 = l6 * cos(theta5);
        double l8 = lM - l7 * cos(theta5);

        // 7. Update the intermediate point C coordinates using the pen tip and theta0.
        double x_c_new = x_pen - sin(theta0) * l8;
        double y_c_new = y_pen - cos(theta0) * l8;

        // Debug: Print current iteration key values.
        printf("Iteration %d:\n", i+1);
        printf("  theta0 = %f rad\n", theta0);
        printf("  theta1 = %f rad\n", theta1);
        printf("  theta2 = %f rad\n", theta2);
        printf("  theta4 = %f rad\n", theta4);
        printf("  theta5 = %f rad\n", theta5);
        printf("  Updated C point: x_c = %f, y_c = %f\n", x_c_new, y_c_new);

        // Check for convergence.
        if (fabs(x_c_new - x_c) < tol && fabs(y_c_new - y_c) < tol) {
            x_c = x_c_new;
            y_c = y_c_new;
            break;
        }
        x_c = x_c_new;
        y_c = y_c_new;
    }

    // Allocate the return array (remember to free this memory in the caller).
    float* angle = (float*)malloc(2 * sizeof(float));
    if(angle == NULL) {
        printf("Memory allocation error.\n");
        exit(-1);
    }
    angle[0] = (float)theta1 * 180.0 / M_PI;
    angle[1] = 90 - (float)theta4 * 180.0 / M_PI;

    // Final output of calculated angles.
    printf("Final results:\n");
    printf("  Motor 1 angle (θ₁) = %f rad\n", theta1);
    printf("  Motor 4 angle (θ₄) = %f rad\n", theta4);

    return angle;
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
        printf("Motor 1 angle: %f rad\n", angle[0]);
        printf("Motor 4 angle: %f rad\n\n", angle[1]);
        free(angle);
    }
    return 0;
}
