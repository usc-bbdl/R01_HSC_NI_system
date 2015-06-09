function [A tau T] = emg_filter()

T = 1/1024;
tau = 0.008;
A = exp(-T/tau);
tf1 = zpk([0], [A A], 2*T*A, T);
tf2 = zpk([0 -A], [A A A], T^2*A/tau, T);

tf_emg = tf1 - tf2


