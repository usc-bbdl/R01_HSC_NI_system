%% Plotter for NIPS_2012

ch = {'emg', 'stretch', 'force', 'spindle'};

for i = 1 : length(ch)
    h_fig = figure(i);
    set(h_fig, 'Units', 'inch', 'Position', [1 1 6 2]);
    obj = eval(['nips_', ch{i}, '1']);
    hrange_obj = range(obj.signals.values) / 2;
    middle_obj = min(obj.signals.values) + hrange_obj;
    h = plot(obj.time, obj.signals.values, 'k', 'LineWidth', 2);
    set(gcf, 'PaperSize', [4. 1.5],'PaperPositionMode', 'auto');
    set(gca, 'Visible', 'off');  
    %ylim([middle_obj - hrange_obj * 1.2, middle_obj + hrange_obj * 1.2]);
    export_fig(['/Users/minos_niu/Dropbox/cmn.OnGoing/NIPS 2012/',...
        'fig_results_multi_scale/nips_', ch{i}, '.pdf'], '-transparent', '-q101');
end