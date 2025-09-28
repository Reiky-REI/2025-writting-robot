function cal_cor(xC,yC)
% 已知参数
l1 = 8.5; % 连杆AB
l2 = 13; % 连杆BC
l3 = 13; % 连杆CD
l4 = 8.5; %连杆DE
l5 = 11.5; %固定杆A
l6 = 3.5; %杆FG
% yC = 1.5000;%C点横坐标
% xC = 19.0204;%C点纵坐标
xF = 1.9318; %F点横坐标
yF = 24.4958;%F点纵坐标
m = 4;%G距笔的距离
%计算phi2
d3 = - 2 * l2 * yC;
e3 = - 2 * l2 * xC;
f3 = xC^2 + yC^2 + l2^2 - l1^2;
discriminant3 = d3^2 + e3^2 - f3^2;
sqrt_discriminant3 = sqrt(discriminant3);
denominator3 = e3 - f3;
phi2 = 2 * atan((d3 + sqrt_discriminant3) / denominator3);
%计算ph0
phi0 = 2 * atan(yF/xF);
l7 = (pi - phi2 - phi0)* l6;
l8 = 6 - l7 + m;
% 计算方程 (5) 的系数
d1 = 2 * l1 * yF - 2 * l1 * l8 * cos(phi0);
e1 = 2 * l1 * xF - 2 * l1 * l8 * sin(phi0);
f1 = xF^2 + yF^2 + l1^2 + l8^2 - l2^2 - 2 * xF * l8 * cos(phi0) + 2 * yF * l8 * sin(phi0);
discriminant1 = d1^2 + e1^2 - f1^2;
sqrt_discriminant1 = sqrt(discriminant1);
denominator1 = e1 - f1;
% 计算 phi1 的两个可能解
phi1_1 = 2 * atan((d1 + sqrt_discriminant1) / denominator1);
phi1_2 = 2 * atan((d1 - sqrt_discriminant1) / denominator1);
% 将弧度转换为角度
phi1_1_deg = rad2deg(phi1_1);
phi1_2_deg = rad2deg(phi1_2);
% 输出 phi1 的解（角度）
fprintf('phi1 的第一个解: %.4f 度\n', phi1_1_deg);
fprintf('phi1 的第二个解: %.4f 度\n', phi1_2_deg);
% 计算方程 (6) 的系数
d2 = 2 * l4 * yF - 2 * l4 * l8 * cos(phi0) - 2 * l4 * l5;
e2 = 2 * l4 * xF - 2 * l4 * l8 * sin(phi0);
f2 = xF^2 + yF^2 + l4^2 + l8^2 + l5^2 - l3^2 - 2 * xF * l8 * cos(phi0) - 2 * l5 * xF + 2 * l8 * l5 * cos(phi0) - 2 * l8 * xF * sin(phi0);
discriminant2 = d2^2 + e2^2 - f2^2;
sqrt_discriminant2 = sqrt(discriminant2);
denominator2 = e2 - f2;
% 计算 phi4 的两个可能解
phi4_1 = 2 * atan((d2 + sqrt_discriminant2) / denominator2);
phi4_2 = 2 * atan((d2 - sqrt_discriminant2) / denominator2);
% 将弧度转换为角度
phi4_1_deg = rad2deg(phi4_1);
phi4_2_deg = rad2deg(phi4_2);
% 输出 phi4 的解（角度）
fprintf('phi4 的第一个解: %.4f 度\n', phi4_1_deg);
fprintf('phi4 的第二个解: %.4f 度\n', phi4_2_deg);

end