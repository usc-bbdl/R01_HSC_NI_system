function [A_twitch T_twitch tau_twitch P e] = twitch_filter()
% Model adopted from Fuglevand 1993, eq 9 - 12

T_twitch = 0.001;
P = 1;
tau_twitch = 0.090;
e = exp(1);
A_twitch = exp(-T_twitch/tau_twitch);
%tf = zpk([0], [A A], P*e*T*A/tau, T);

B = [0 1 0] * P * e * A_twitch / tau_twitch
A = [1 -2*A_twitch A_twitch^2] * P * e * A_twitch / tau_twitch


