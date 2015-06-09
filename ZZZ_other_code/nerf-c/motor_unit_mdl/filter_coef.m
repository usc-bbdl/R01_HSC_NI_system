function [num, den] = filter_coef(mag1, mag2, trise, tfall, T)
%#codegen

% Shadmehr muscle model gives the h(t) of active_state
% h(t) = mag1*exp(-t/tfall) - mag2*exp(-t/rise)
% H(s) = mag1/(s+1/tfall) - mag2/(s+1/trise)
%      = a/(s+c) - b/(s+d)
% This func: H(s) -> H(z) 

% a = 48144*128;
% b = 45845*128;
coder.extrinsic('tf', 'c2d', 'tfdata', 'dfilt.df2sos', 'dfilt', 'df2sos','filter'); 

a = mag1;
b = mag2;
c = 1/tfall;  % tf, tr in seconds. 
d = 1/trise;
num1 = [0 a];
den1 = [1 c];
tf1 = tf(num1, den1);

num2 = [0 b];
den2 = [1 c];
tf2 = tf(num2, den2);

tf_a = tf1 - tf2;

tf_a_digital = c2d(tf_a, T, 'zoh');
[num den] = tfdata(tf_a_digital, 'v');
str = sprintf('[%s]', num2str(num));
disp(str);

str = sprintf('[%s]', num2str(den));
disp(str);
% 
% a1=den(1)
% a2=den(2)
% a3=den(3)
% b1=num(1)
% b2=num(2)
% b3=num(3)
% 
% %hd=dfilt.df2sos(b1,a1,b2,a2,b3,a3)
% bcoeff = [b1 b2 b3]
% acoeff = [a1 a2 a3]
% 
% y=[0;0]  % type definition for output
% y = filtfilt(bcoeff, acoeff,  x) 
