% 331 vibe test subject 
% 
% Material: 6061
% 
% LxW: 4x1 inch
% H: 7 inch
% 
% Experimental Resonant freq 576 Hz
clc; clearvars;

f_exp = 576 %Hz

L = 4*0.0254; % m
W = 1*0.0254; % m
H = 7*0.0254; % m
rho = 2.7E6; % g/m^3
E = 68.9E9; % Pa

I = L*W^3/12; % m^4

m = rho*(L*W*H)/1000; % kg

f_fund = 0.5595*sqrt(E*I/(m*H^3)) Hz

percentDiff = (f_fund-556)/556 * 100