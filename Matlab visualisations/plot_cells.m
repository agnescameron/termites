cityPlot = csvread('../sample_data/cellfile_termpop_100.csv');

citySize = 30;

Y = zeros(citySize);
cover = 0;
%plot3(cityPlot(:, 1), cityPlot(:, 2), cityPlot(:, 3));

Y(10, 10) = cityPlot(100, 3);

for threshold = 5:12
    cover = 0;

    for a = 1:citySize-1
        for b = 1:citySize-1
            if cityPlot(citySize*a + b, 3) > threshold
                Y(a,b) = 5;
                cover = cover + 1;
            else
                Y(a,b) = 0;
            end
        end
    end
    
    efficiency = cover/(citySize*citySize)
    
    figure();
    b = bar3(Y);
    axis equal;
    colorbar


    for k = 1:length(b)
        zdata = b(k).ZData;
        b(k).CData = zdata;
        b(k).FaceColor = 'interp';
    end

end
