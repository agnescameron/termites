figure();

coverage = importdata('../sample_data/coverfile_termpop_10.csv');
plot(1:1500, coverage);
hold on

coverage = importdata('../sample_data/coverfile_termpop_50.csv');
plot(1:1500, coverage);

coverage = importdata('../sample_data/coverfile_termpop_100.csv');
plot(1:1500, coverage);

coverage = importdata('../sample_data/coverfile_termpop_250.csv');
plot(1:1500, coverage);

legend('pop = 10', 'pop = 50', 'pop = 100', 'pop = 250', 'Location', 'Southeast');

xlabel('number of cycles')
t = '$\eta_{grid}$';
ylabel(t,'interpreter','latex');
title('Efficiency over time for different population sizes')