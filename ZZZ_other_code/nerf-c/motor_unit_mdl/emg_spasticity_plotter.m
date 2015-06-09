load('spasticity_emg_data01.mat');

figure(1); hold on;
for i = 1:8
    ch = eval(['pos' num2str(i)]);
    plot(ch.signals.values,'k', 'linewidth', 1.5);
end

figure(2); hold on;
for i = 1:8
    ch = eval(['vel' num2str(i)]);
    plot(ch.signals.values,'k', 'linewidth', 1.5);
    ylim([0 0.15])
end

figure(3); hold on;


f=1024;%  sampling frequency
f_cutoff = 40; % cutoff frequency

fnorm =f_cutoff/(f/2); % normalized cut off freq, you can change it to any value depending on your requirements

[b1,a1] = butter(2,fnorm,'low'); % Low pass Butterworth filter of order 10



for i = 1:8
    ch = eval(['emg' num2str(i)]);
    emg_filt = filtfilt(b1,a1, abs(ch.signals.values)); % filtering

    plot(emg_filt - i * 250,'k', 'linewidth', 1.5);
end