% 已知参数
l1 = 9; % 连杆AB
l2 = 13; % 连杆BC
l3 = 13; % 连杆CD
l4 = 9; %连杆DE
l5 = 10.5; %固定杆AE
l6 = 3.5; %
phi1 = (31*pi)/72; % phi1角度
xB = 1.948; % B点x轴坐标
yB = 8.787; % B点y轴坐标
xD = 7.575; %D点x轴坐标
yD = 8.787; % D点y轴坐标
m = 2.5;
% 计算 lBD
lBD = sqrt((xD - xB)^2 + (yD - yB)^2);
% 计算 A0, B0, C0
A0 = 2 * l2 * (xD - xB);
B0 = 2 * l2 * (yD - yB);
C0 = l2^2 + lBD^2 - l3^2;
% 计算 phi2
phi2 = 2 * atan((B0 + sqrt(A0^2 + B0^2 - C0^2)) / (A0 + C0));
% 计算 phi3（phi3 = 180度 - phi2）
phi3 = pi - phi2; % 180度 = pi 弧度
% 计算 C 点坐标
xC = l1 * cos(phi1) + l2 * cos(phi2);
yC = l1 * sin(phi1) + l2 * sin(phi2);
fprintf('C 点坐标: (%.4f, %.4f)\n', xC, yC);
% 计算极坐标 L0 和 phi0
L0 = sqrt((xC - l5/2)^2 + yC^2);
phi0 = atan2(yC, xC - l5/2); % 使用 atan2 避免象限问题
% 计算 phi5
phi5 = phi3 - phi0;
% 计算 l7
l7 = cos(phi5) * l6;
% 计算 l8
l8 = 6 - l7 + m;
% 计算 F 点坐标
xF = l1 * cos(phi1) + l2 * cos(phi2) + l8 * cos(phi0);
yF = l1 * sin(phi1) + l2 * sin(phi2) + l8 * sin(phi0);
% 输出结果
fprintf('F 点坐标: (%.4f, %.4f)\n', xF, yF);