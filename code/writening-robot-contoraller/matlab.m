%% simulate_robot.m
% 三维仿真程序
% 本程序基于平面五连杆机构（混合链机器人）建立仿真，
% 利用已知的逆运动学公式计算舵机角度 θ₁ 和 θ₄，并绘制机器人运动及笔尖轨迹。
% 说明：
%   - 参数：l1=90, l2=130, l3=130, l4=90, l5=105, l6=35, lM=95（单位均为毫米）
%   - θ₁ 为链接 l1 与 l5（墙面）的夹角，θ₄ 为链接 l4 与 l5之间的夹角。
%   - 设定墙面为 x = l5 （即所有点满足 x <= l5）。
%   - penDown 为布尔型，true 表示落笔写字，false 表示抬笔移动。
%
% 注意：本示例中中间变量 x_c, y_c 简化为与笔尖坐标重合，实际应用中可加入坐标转换。

clear; clc; close all;

%% 固定参数（单位：mm）
l1 = 90;    l2 = 130;    l3 = 130;
l4 = 90;    l5 = 105;    l6 = 35; 
lM = 95;    % 末端连杆参数

%% 生成笔尖轨迹（确保所有点满足 x_pen <= l5，即在墙面一侧）
% 此处我们以一个简单的曲线路径为例
N = 100;
t = linspace(0,2*pi,N);
% 生成一条扁平的椭圆（注意 x 坐标控制在 60~l5 范围内）
x_pen = 60 + 30*cos(t);  
y_pen = 80 + 30*sin(t);

% 保证 x_pen 不超过墙面（l5=105）
x_pen(x_pen > l5) = l5; 

% 设置笔尖状态：落笔写字（true）或抬笔移动（false）
% 此处我们令前半段为写字，后半段为抬笔移动
penDown = true * ones(1,N);
penDown(round(N/2):end) = false;

%% 初始化画布
figure('Name','混合链机器人三维仿真','NumberTitle','off');
axis([-20 130 -20 150 -20 20]);
grid on;
xlabel('X (mm)');
ylabel('Y (mm)');
zlabel('Z (mm)');
view(3);
hold on;

% 绘制墙面（以一条竖直线表示 x = l5 ）
plot3([l5 l5],[ -20, 150],[0 0],'k','LineWidth',3);

% 用于记录落笔轨迹
trace_x = [];
trace_y = [];
trace_z = [];

%% 仿真动画循环
for i=1:N
    % 取当前笔尖坐标
    xp = x_pen(i);
    yp = y_pen(i);
    
    % 确保笔尖不超出墙面
    if xp > l5
        xp = l5;
    end
    
    % 定义工作坐标系 (简化假设 x_c = x_pen, y_c = y_pen)
    x_c = xp;
    y_c = yp;
    
    %% 计算舵机角度 θ₁ (舵机1)
    % 公式：d1 = -2*l1*y_c, e1 = -2*l1*x_c, f1 = x_c^2 + y_c^2 + l1^2 - l2^2,
    %       θ₁ = 2 * arctan((d1 + sqrt(d1^2+e1^2-f1^2))/(e1 - f1))
    d1 = -2 * l1 * y_c;
    e1 = -2 * l1 * x_c;
    f1 = x_c^2 + y_c^2 + l1^2 - l2^2;
    disc1 = d1^2 + e1^2 - f1^2;
    if disc1 < 0, disc1 = 0; end
    theta1 = 2 * atan((d1 + sqrt(disc1))/(e1 - f1));
    
    %% 计算舵机角度 θ₄ (舵机4)
    % 公式：d2 = -2*l3*y_c, e2 = 2*l4*l5 - 2*x_c, f2 = x_c^2+y_c^2+l4^2 - l3^2 + l5^2 - 2*l5*x_c,
    %       θ₄ = 2 * arctan((d2 + sqrt(d2^2+e2^2-f2^2))/(e2 - f2))
    d2 = -2 * l3 * y_c;
    e2 = 2 * l4 * l5 - 2 * x_c;
    f2 = x_c^2 + y_c^2 + l4^2 - l3^2 + l5^2 - 2 * l5 * x_c;
    disc2 = d2^2 + e2^2 - f2^2;
    if disc2 < 0, disc2 = 0; end
    theta4 = 2 * atan((d2 + sqrt(disc2))/(e2 - f2));
    
    %% 计算关键点坐标
    % 基座在原点，计算舵机1末端（点 B）
    xB = l1 * cos(theta1);
    yB = l1 * sin(theta1);
    
    % 对于舵机4，先由逆运动学得知笔尖由点 D 到达，故计算 D 点坐标：
    xD = xp - l4 * cos(theta4);
    yD = yp - l4 * sin(theta4);
    
    %% 绘制机器人连杆
    cla;  % 清除当前坐标区内容
    hold on;
    
    % 绘制墙面
    plot3([l5 l5],[-20,150],[0,0],'k','LineWidth',3);
    
    % 绘制基座、舵机1连杆、舵机4连杆及其他连杆
    % 连杆1：从基座 (0,0) 到点 B
    plot3([0, xB],[0, yB],[0,0],'b','LineWidth',4);
    % 连杆2：从点B到虚拟连杆末端（设为交点 D，用于闭链计算，此处简化处理）
    plot3([xB, xD],[yB, yD],[0,0],'g','LineWidth',4);
    % 连杆3（笔尖连杆）：从点 D 到笔尖 (xp, yp)
    plot3([xD, xp],[yD, yp],[0,0],'r','LineWidth',4);
    
    % 绘制各关键点
    plot3(0,0,0,'ko','MarkerSize',8,'MarkerFaceColor','k');      % 基座
    plot3(xB,yB,0,'ko','MarkerSize',8,'MarkerFaceColor','k');      % 点 B
    plot3(xD,yD,0,'ko','MarkerSize',8,'MarkerFaceColor','k');      % 点 D
    plot3(xp,yp,0,'ko','MarkerSize',8,'MarkerFaceColor','k');      % 笔尖
    
    % 如果笔尖状态为落笔（penDown true），记录笔迹轨迹
    if penDown(i)
        trace_x(end+1) = xp;
        trace_y(end+1) = yp;
        trace_z(end+1) = 0;  % 平面仿真，z=0
    end
    % 绘制笔迹轨迹
    if ~isempty(trace_x)
        plot3(trace_x, trace_y, trace_z, 'm-', 'LineWidth',2);
    end
    
    % 为方便观察，同时显示舵机角度信息（转换为度单位）
    title(sprintf('Frame %d: \\theta_1 = %.2f^\\circ,  \\theta_4 = %.2f^\\circ', ...
          i, theta1*180/pi, theta4*180/pi));
    xlabel('X (mm)'); ylabel('Y (mm)'); zlabel('Z (mm)');
    axis([-20 130 -20 150 -20 20]);
    grid on;
    drawnow;
    pause(0.05);
end

%% 最终显示笔迹轨迹
figure('Name','笔迹轨迹','NumberTitle','off');
plot3(trace_x, trace_y, trace_z, 'm-', 'LineWidth',2);
xlabel('X (mm)'); ylabel('Y (mm)'); zlabel('Z (mm)');
title('落笔轨迹');
grid on; axis equal;