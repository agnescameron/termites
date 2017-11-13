cityPlot = csvread('../sample_data/cellfile_termpop_100.csv');

citySize = 50;

    for a = 1:citySize-1
        for b = 1:citySize-1
            Y(a, b) = cityPlot(citySize*a + b, 4);
        end
    end

    figure();
    b = bar3(Y);
    axis equal;
    colorbar

    for k = 1:length(b)
        zdata = b(k).ZData;
        b(k).CData = zdata;
        b(k).FaceColor = 'interp';
    end

