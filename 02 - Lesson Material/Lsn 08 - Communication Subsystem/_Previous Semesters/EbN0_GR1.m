clc; clearvars; close all

Re = 6378137; %m

Pt = 10*log10(2); %dBW

Gt = 2.15; %dBi

Ll = -0.0; %dB

f = 2288E6; %Hz

h = 500000; %m

el = 20; %deg

lambda = 299792458/f %m

dmax = -Re*sind(el) + sqrt((Re*sind(el))^2 + h^2 + 2*Re*h)

Ls = 10*log10((lambda/(4*pi*dmax))^2) %db

theta = 0; %deg

Dpar = 1.2; %m

theta3db = 70*lambda/Dpar; %deg
LptRX = -12*(theta/theta3db)^2;
LptRX = 0;

theta3db = 2; %deg
LptTX = -12*(theta/theta3db)^2;
LptTX = 0;

thetaPol = 0; %deg
Lpol = 10*log10(cosd(thetaPol)^2) %db

Azenith = -0.04; %from chart
Latm = Azenith/sind(el); %db
Latm = 0;

Lrain = 0;

Lm = LptTX + LptRX + Lpol + Latm + Lrain

Tr = 30 + 273.15; %K

R = 166.66E3; %bps
 
eta = 0.68;
Gr = 10*log10((pi*Dpar/lambda)^2*eta) %dB

EbN0 = Pt + Gt + Ll + Ls + Lm + Gr + 228.6 - 10*log10(Tr) - 10*log10(R)

Threshold = EbN0 - 8.5