clc; clearvars; close all

Re = 6378137; %m

h = 120000; %m

Pt = 0; %dBW
Gt = 2; %dBi
Gr = 8;
Ll = -2; %dB

theta = 40; %deg Pointing Error
thetaPol = 60; %deg Polarization error

% Dpar = 3; %m

theta3db_rx = 78; %deg
theta3db_tx = 78; %deg

R = 10E3; %bps

Azenith = -0.02; %from chart

f = 0.915E9; %Hz
el = 75; %deg

Tr = 40 + 273.15; %K

%%

lambda = 299792458/f %m
dmax = -Re*sind(el) + sqrt((Re*sind(el))^2 + h^2 + 2*Re*h)
Ls = 10*log10((lambda/(4*pi*dmax))^2) %db


LptRX = -12*(theta/theta3db_rx)^2 


LptTX = -12*(theta/theta3db_tx)^2 


Lpol = 10*log10(cosd(thetaPol)^2) %db

Latm = Azenith/sind(el) %db

Lrain = 0;

Lm = LptTX + LptRX + Lpol + Latm + Lrain



% eta = 0.6; %assumed
% Gr = 10*log10((pi*Dpar/lambda)^2*eta); %dB



EbN0 = Pt + Gt + Ll + Ls + Lm + Gr + 228.6 - 10*log10(Tr) - 10*log10(R)