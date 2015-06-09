function [mag1 mag2 A_rise A_fall T] = get_active_function_coeff()

T = 0.001;
mag1 = 318144;
mag2 = 45845;
t_rise = 0.04;
t_fall = 0.06;
A_rise = exp(-T/t_rise);
A_fall = exp(-T/t_fall);


