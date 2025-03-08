clear; clc;

% ---- 机构参数 (mm) ----
l1 = 90;   % A->B
l2 = 130;  % B->C
l3 = 130;  % E->D
l4 = 90;   % D->C
l5 = 105;  % A->E (左右铰点间距)
l6 = 35;   % 用于计算 l7 = cos(theta5)*l6
lM = 95;   % 用于计算 l8 = lM - l7*cos(theta5)

% ---- 舵机角度范围 (度) ----
phi1_range = 0:5:180;  % 左侧舵机
phi4_range = 0:5:180;  % 右侧舵机

pen_points  = [];  % 用于记录笔尖 (x_pen, y_pen)
servo_angles = []; % 用于记录对应的 (phi1, phi4)

for phi1_deg = phi1_range
    % 左舵机角度(弧度)
    phi1 = deg2rad(phi1_deg);
    % 点 B：以 A(0,0) 为圆心，半径 l1，角度 phi1
    Bx = l1*cos(phi1);
    By = l1*sin(phi1);
    
    for phi4_deg = phi4_range
        % 右舵机角度(弧度)
        phi4 = deg2rad(phi4_deg);
        % 点 D：以 E(l5,0) 为圆心，半径 l3，角度 phi4
        % 所以 D = E + [l3*cos(phi4), l3*sin(phi4)]
        Dx = l5 + l3*cos(phi4);
        Dy =        l3*sin(phi4);
        
        % ---- 两圆相交：圆1(中心B,半径l2), 圆2(中心D,半径l4) ----
        % 圆心距
        distBD = sqrt((Dx - Bx)^2 + (Dy - By)^2);
        
        % 若两圆无交点，跳过
        if distBD > (l2 + l4) || distBD < abs(l2 - l4) || distBD < 1e-9
            continue;
        end
        
        % 按标准几何公式求交点 (可能有 0,1,2 个交点)
        r1 = l2; r2 = l4;
        % a = B 到两圆交点连线在 BD 上的投影距离
        a = (r1^2 - r2^2 + distBD^2)/(2*distBD);
        % 半弦长
        h = sqrt(r1^2 - a^2);
        
        % 圆心连线 BD 的方向向量
        vx = (Dx - Bx)/distBD;
        vy = (Dy - By)/distBD;
        
        % 交点中线 M (在 BD 上的中点)
        Mx = Bx + a*vx;
        My = By + a*vy;
        
        % 垂直方向单位矢量 (与 BD 垂直)
        rx = -vy * h;
        ry =  vx * h;
        
        % 两个交点 C1, C2
        C1x = Mx + rx;  C1y = My + ry;
        C2x = Mx - rx;  C2y = My - ry;
        
        % 分别检查这两个交点是否能得到合理的笔尖位置
        for iCase = 1:2
            if iCase == 1
                Cx = C1x;  Cy = C1y;
            else
                Cx = C2x;  Cy = C2y;
            end
            
            % =============== 下面根据你的笔尖公式计算 ===============
            % (1) θ0 = atan2(y_c, x_c - l5/2)
            theta0 = atan2(Cy, Cx - (l5/2));
            
            % (2) 求 θ2：即连杆 B->C 与 D->C 的夹角
            %    先构造向量 BC、DC，然后用点积求夹角
            BCx = Bx - Cx;  BCy = By - Cy;
            DCx = Dx - Cx;  DCy = Dy - Cy;
            lenBC = sqrt(BCx^2 + BCy^2);
            lenDC = sqrt(DCx^2 + DCy^2);
            dotVal = BCx*DCx + BCy*DCy;
            cosVal = dotVal/(lenBC*lenDC);
            % 数值上若有微小超差，裁一下
            cosVal = max(min(cosVal, 1), -1);
            angleC = acos(cosVal);  % 这就是 BC 与 DC 的夹角
            % 具体 θ2 = angleC 还是 π - angleC，要看你的机构布局
            % 若你原公式定义 θ2 为内角，可直接用 angleC
            theta2 = angleC;
            
            % (3) θ5 = π - θ2 - θ0
            theta5 = pi - theta2 - theta0;
            
            % (4) l7 = cos(θ5)*l6
            l7 = cos(theta5)*l6;
            
            % (5) l8 = lM - l7*cos(θ5)
            l8 = lM - l7*cos(theta5);
            
            % 若 l8 < 0 之类的情况可能无效，可以做个简单排除
            if l8 < 0
                continue;
            end
            
            % (6) 笔尖坐标
            x_pen = Cx + sin(theta0)*l8;
            y_pen = Cy + cos(theta0)*l8;
            
            % =============== 加入结果集合 ===============
            pen_points  = [pen_points;  x_pen, y_pen];
            servo_angles = [servo_angles; phi1_deg, phi4_deg];
        end
    end
end

%% 作图
figure(1); clf;
plot(pen_points(:,1), pen_points(:,2), 'b.','MarkerSize',8);
axis equal; grid on;
xlabel('x_{pen} (mm)');
ylabel('y_{pen} (mm)');
title('笔尖工作空间 (几何求解)');

figure(2); clf;
plot(servo_angles(:,1), servo_angles(:,2), 'b.','MarkerSize',8);
xlabel('\phi_1 (°)');
ylabel('\phi_4 (°)');
title('舵机角度对应关系');
