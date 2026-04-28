%given:
n = 3; %msn life (yrs)
h = 500; %km
Preq_eclipse = 2; %W
Preq_sun = 4; %W
eta_eclipse = .65; %
eta_sun = 0.8; %
theata = 25; %worst case incidence angle (deg)
degradation = 0.05; % percent/year

%constants:
S_sun = 1358; %W/m^2
R_E = 6378.137; %km
mu_E = 398600.5;

%from datasheet:
eta_cell = .239; %
A_panel = 10*(0.02775*0.01069)

%%

P = 2*pi*sqrt((R_E+h)^3/mu_E) %sec
rho = 2*asind(R_E/(R_E+h)) %deg
T_eclipse = (rho/360)*P %sec
T_sun = P - T_eclipse; %sec

P_req = (Preq_eclipse*T_eclipse/eta_eclipse + Preq_sun*T_sun/eta_sun)/T_sun

%%

P_BOL = P_req/(1-degradation)^n

A_req = P_BOL / (S_sun*eta_cell*cosd(theata))

n_panels_req = A_req / A_panel %Answer = 9.18 --> round up to 10