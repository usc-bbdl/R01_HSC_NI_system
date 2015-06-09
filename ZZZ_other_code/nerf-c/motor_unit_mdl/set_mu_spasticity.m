function set_mu_spasticity(~)


% 
% tau_normal_big = 0.04;  big_height = 8;
% tau_normal_med = 0.06;  med_height = 3.5;
% tau_normal_small = 0.09; small_height = 1;
% 
% tau_spastic_big = 0.04;
% tau_spastic_med = 0.06;
% tau_spastic_small = 0.09;

% twitch filter parameters
%T_twitch = 1/1024;
T_twitch = 0.001;
P = 1;
tau_twitch = 0.016;
e = exp(1);
A_twitch = exp(-T_twitch/tau_twitch);

% get_active_function_coeff
T = 0.001;
mag1 = 318144;
mag2 = 45845;
t_rise = 0.04;
t_fall = 0.06;
A_rise = exp(-T/t_rise);
A_fall = exp(-T/t_fall);




for i = 1:20
    eval(['SIZE_MU_', num2str(i), '=', num2str('(atan(150) - atan(i^0.8)) * 7')])
end