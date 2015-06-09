function set_trfcn(fname)
open_system('motor_unit');
%open_system('test/twitch');
set_param('motor_unit','Lock','off')


tau_normal_big = 0.04;  big_height = 8;
tau_normal_med = 0.06;  med_height = 3.5;
tau_normal_small = 0.09; small_height = 1;

tau_spastic_big = 0.04;
tau_spastic_med = 0.06;
tau_spastic_small = 0.09;

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



%add_block('built-in/MATLAB Fcn',['test/',fname], 'matlabfcn',fname,'name',fname)
% NORMAL big motor unit
set_param('motor_unit/NORMAL/big motor unit/Fuglevand twitch', 'sys', ['zpk([0], [' num2str(A_twitch) ' ' num2str(A_twitch) '], ' num2str(P) '*' num2str(e) '*' num2str(T_twitch) '*' num2str(A_twitch) '/' num2str(tau_twitch) ', ' num2str(T_twitch) ')']);
set_param('motor_unit/NORMAL/big motor unit/active state', 'sys', ['0*zpk([0], ' num2str(A_rise) ', ' num2str(mag1) ', T) + zpk([0], exp(-T/' num2str(tau_normal_big) '),  mag2*' num2str(big_height) ', T)'  ]);

% NORMAL med motor unit
set_param('motor_unit/NORMAL/med motor unit/Fuglevand twitch', 'sys', ['zpk([0], [' num2str(A_twitch) ' ' num2str(A_twitch) '], ' num2str(P) '*' num2str(e) '*' num2str(T_twitch) '*' num2str(A_twitch) '/' num2str(tau_twitch) ', ' num2str(T_twitch) ')']);
set_param('motor_unit/NORMAL/med motor unit/active state', 'sys', ['0*zpk([0], A_rise, mag1, T) + zpk([0], exp(-T/' num2str(tau_normal_med) '),  mag2*' num2str(med_height) ', T)'  ]);

% NORMAL small motor unit
set_param('motor_unit/NORMAL/small motor unit/Fuglevand twitch', 'sys', ['zpk([0], [' num2str(A_twitch) ' ' num2str(A_twitch) '], ' num2str(P) '*' num2str(e) '*' num2str(T_twitch) '*' num2str(A_twitch) '/' num2str(tau_twitch) ', ' num2str(T_twitch) ')']);
set_param('motor_unit/NORMAL/small motor unit/active state', 'sys', ['0*zpk([0], A_rise, mag1, T) + zpk([0], exp(-T/' num2str(tau_normal_small) '), mag2*' num2str(small_height) ', T)'  ]);

% Spastic big motor unit
set_param('motor_unit/Spastic/big motor unit/Fuglevand twitch', 'sys', ['zpk([0], [' num2str(A_twitch) ' ' num2str(A_twitch) '], ' num2str(P) '*' num2str(e) '*' num2str(T_twitch) '*' num2str(A_twitch) '/' num2str(tau_twitch) ', ' num2str(T_twitch) ')']);
set_param('motor_unit/Spastic/big motor unit/active state', 'sys', ['0*zpk([0], A_rise, mag1, T) + zpk([0], exp(-T/' num2str(tau_spastic_big) '), mag2*' num2str(big_height) ', T)'  ]);

% Spastic med motor unit
set_param('motor_unit/Spastic/med motor unit/Fuglevand twitch', 'sys', ['zpk([0], [' num2str(A_twitch) ' ' num2str(A_twitch) '], ' num2str(P) '*' num2str(e) '*' num2str(T_twitch) '*' num2str(A_twitch) '/' num2str(tau_twitch) ', ' num2str(T_twitch) ')']);
set_param('motor_unit/Spastic/med motor unit/active state', 'sys', ['0*zpk([0], A_rise, mag1, T) + zpk([0], exp(-T/' num2str(tau_spastic_med) '),  mag2*' num2str(med_height) ', T)'  ]);

% Spastic small motor unit
set_param('motor_unit/Spastic/small motor unit/Fuglevand twitch', 'sys', ['zpk([0], [' num2str(A_twitch) ' ' num2str(A_twitch) '], ' num2str(P) '*' num2str(e) '*' num2str(T_twitch) '*' num2str(A_twitch) '/' num2str(tau_twitch) ', ' num2str(T_twitch) ')']);
set_param('motor_unit/Spastic/small motor unit/active state', 'sys', ['0*zpk([0], A_rise, mag1, T) + zpk([0], exp(-T/' num2str(tau_spastic_small) '),  mag2*' num2str(small_height) ', T)'  ]);



save_system('motor_unit');

