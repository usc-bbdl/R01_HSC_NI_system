emg = t5(:, 4);
time = t5(:, 1);
pos = 2.0 - t5(:, 6);

subplot(211);
plot(pos);
subplot(212);
plot(emg);


[x1, y1] = ginput(1);
[x2, y2] = ginput(1);

timeIndex = ceil(x1):ceil(x2);
subplot(211);
plot(time(timeIndex), pos(timeIndex));
subplot(212);
plot(time(timeIndex), emg(timeIndex));

