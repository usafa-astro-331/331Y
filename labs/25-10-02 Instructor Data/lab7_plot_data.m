clc; clearvars; 
% close all;

%% Import Data

% file = 'TestA_data_Lab7_2025-10-02_122016.csv'; adjustV = 1; %Test A
file = 'TestB_data_Lab7_2025-10-02_090559.csv'; adjustV = 5/5.2; % Test B


% uiimport(file)

M = readmatrix(file);

t_ms = M(:,1);  
gyro_Z_dps = M(:,2);  
mag_X_uT = M(:,3);  
mag_Y_uT = M(:,4);  
sun_direction_deg = M(:,5); 
sun_plusXcount = M(:,6);
sun_plusYcount = M(:,7);
sun_minusXcount = M(:,8);
sun_minusYcount = M(:,9);
w_RW_cmd_RPM = M(:,10) * adjustV;
w_RW_meas_RPM = M(:,11);

t_sec = (t_ms - t_ms(1)) / 1000;

%% Plot Raw Gyro Data

figure()
hold on
grid on
title("KestrelSAT Gyro Z Data")
xlabel("Time (sec)")
ylabel("Angular Rate (deg/s)")
plot(t_sec, gyro_Z_dps)

%% Plot Raw Mag Data

figure()
hold on
grid on
title("KestrelSAT Raw Magnetometer Data")
xlabel("Time (sec)")
ylabel("Magnetic Field Strength (uT)")
plot(t_sec, mag_X_uT, 'DisplayName',"Raw Magnetometer X-Axis")
plot(t_sec, mag_Y_uT, 'DisplayName',"Raw Magnetometer Y-Axis")


%% Calibrate Magnetometer Data and Plot Magnetic Heading

mag_X_offset = (max(mag_X_uT) + min(mag_X_uT))/2;
mag_Y_offset = (max(mag_Y_uT) + min(mag_Y_uT))/2;

mag_X_range = (max(mag_X_uT) - min(mag_X_uT))/2;
mag_Y_range = (max(mag_Y_uT) - min(mag_Y_uT))/2;

mag_X_cal_uT = (mag_X_uT - mag_X_offset) / mag_X_range;
mag_Y_cal_uT = (mag_Y_uT - mag_Y_offset) / mag_Y_range;
mag_hdg = mod(atan2d(-mag_Y_cal_uT,mag_X_cal_uT),360);

% mag_avg_rang = (mag_X_range + mag_Y_range) / 2;
% mag_X_cal2_uT = (mag_X_uT - mag_X_offset) * (mag_avg_rang / mag_X_range);
% mag_Y_cal2_uT = (mag_Y_uT - mag_Y_offset) * (mag_avg_rang / mag_Y_range);
% mag_hdg2 = mod(atan2d(-mag_Y_cal2_uT,mag_X_cal2_uT),360);

figure()
hold on
grid on
title("KestrelSAT Calibrated Magnetometer Data")
xlabel("Time (sec)")
ylabel("Magnetic Field Strength (uT)")
plot(t_sec, mag_X_cal_uT, 'DisplayName',"Calibrated Magnetometer X-Axis")
plot(t_sec, mag_Y_cal_uT, 'DisplayName',"Calibrated Magnetometer Y-Axis")

%% Plot Magnetic Heading

figure()
hold on
grid on
title("KestrelSAT Magnetic Heading")
xlabel("Time (sec)")
ylabel("Heading WRT Magnetic North (deg)")
plot(t_sec, mag_hdg, 'LineWidth',2,'DisplayName',"Magnetometer-Based Heading")

% figure
% plot(mag_hdg - mag_hdg2)

%% Calculate Gyro Heading

dt = diff(t_sec);
gyro_heading_deg = zeros(size(gyro_Z_dps));
gyro_heading_deg(1) = mag_hdg(1);

for i=2:length(gyro_heading_deg)
    gyro_heading_deg(i) = gyro_heading_deg(i-1) + gyro_Z_dps(i)*dt(i-1);
end

gyro_heading_deg = wrapTo360(gyro_heading_deg);

plot(t_sec,gyro_heading_deg, 'LineWidth',2,'DisplayName',"Gyro-Based Heading")
legend

%% Plot diff 

figure()
hold on
grid on
title("KestrelSAT Difference Between Gyro Heading and Magnetometer Heading")
xlabel("Time (sec)")
ylabel("Difference of Heading WRT Magnetic North (deg)")
plot(t_sec, wrapTo180(mag_hdg - gyro_heading_deg), 'LineWidth',2)

%% Plot Wheel Speed

figure()
hold on
grid on
title("KestrelSAT Reaction Commanded Wheel Speed Test A")
xlabel("Time (sec)")
ylabel("Reaction Wheel Speed (RPM)")
plot(t_sec,w_RW_cmd_RPM, 'LineWidth',2,'DisplayName',"Commanded Wheel Speed")
plot(t_sec,w_RW_meas_RPM, 'LineWidth',2,'DisplayName',"Measured Wheel Speed")
legend()




%% Sun direciton
% 
% figure()
% hold on
% grid on
% title("KestrelSAT Heading from Body X-axis to Sun")
% xlabel("Time (sec)")
% ylabel("Heading From X-Axis to Sun")
% plot(t_sec, sun_direction_deg, 'LineWidth',2)