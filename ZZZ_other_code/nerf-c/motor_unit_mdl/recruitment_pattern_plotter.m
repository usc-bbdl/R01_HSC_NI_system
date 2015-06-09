function recruitment_pattern_plotter(spikes_g1, spikes_g2, total_force, prob_keep)

raster = [];

for i = 2 : 11
    p1 = ( spikes_g1.signals(i).values > 0 );
    r1 = rand(size(p1));
    raster = [raster p1 .* (r1 > prob_keep)];
end

for i = 1 : 10
    p1 = ( spikes_g2.signals(i).values > 0 );
    r1 = rand(size(p1));
    raster = [raster p1 .* (r1 > prob_keep)];
end

figure(1); 

spiketimes = find(raster > 0);

%PLTRSTPTHSMPL(RSTPTHSMPL(spiketimes, spikes,0,1000),'test',1,1,'test2')
subplot(211);
rasterplot(spiketimes,30,length(raster))
title('MotoNeuron Raster');

subplot(212);
time = 1 : length(total_force.signals.values);
time = time / max(time) * 100;
plot(time, total_force.signals.values, 'k', 'LineWidth', 2.0)