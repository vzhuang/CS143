clear mex;
close all;
clear all;
VECTOR_SIZE = 100000;
time = zeros(1,VECTOR_SIZE);
% Link Rate
lr1 = zeros(1,VECTOR_SIZE);
lr2 = zeros(1,VECTOR_SIZE);
lr3 = zeros(1,VECTOR_SIZE);
% Buffer Occupancy
bo1 = zeros(1,VECTOR_SIZE);
bo2 = zeros(1,VECTOR_SIZE);
bo3 = zeros(1,VECTOR_SIZE);
% Packet Loss
pl1 = zeros(1,VECTOR_SIZE);
pl2 = zeros(1,VECTOR_SIZE);
pl3 = zeros(1,VECTOR_SIZE);
% Flow Rate
fr1 = zeros(1,VECTOR_SIZE);
fr2 = zeros(1,VECTOR_SIZE);
fr3 = zeros(1,VECTOR_SIZE);
% Window Size
ws1 = zeros(1,VECTOR_SIZE);
ws2 = zeros(1,VECTOR_SIZE);
ws3 = zeros(1,VECTOR_SIZE);
% Packet Delay
pd1 = zeros(1,VECTOR_SIZE);
pd2 = zeros(1,VECTOR_SIZE);
pd3 = zeros(1,VECTOR_SIZE);

link_rates = figure;
figure(link_rates);
set(gcf,'numbertitle','off','name','Link Rate vs Time');
movegui('northwest');
plot([0], [0], 'g-'); hold on; plot([0], [0], 'r-');plot([0], [0], 'k-');
xlabel('Time (s)');
ylabel('Link Rate (Mbps)');

buffer_occupancies = figure;
figure(buffer_occupancies);
set(gcf,'numbertitle','off','name','Buffer Occupancy vs Time');
movegui('southwest');
plot([0], [0], 'g-'); hold on; plot([0], [0], 'r-');plot([0], [0], 'k-');
xlabel('Time (s)');
ylabel('Buffer Occupancy (# packets)');

packet_losses = figure;
figure(packet_losses);
set(gcf,'numbertitle','off','name','Packet Loss vs Time');
movegui('north');
plot([0], [0], 'g-'); hold on; plot([0], [0], 'r-');plot([0], [0], 'k-');
xlabel('Time (s)');
ylabel('Packet Loss (# packets)');

flow_rates = figure;
figure(flow_rates);
set(gcf,'numbertitle','off','name','Flow Rate vs Time');
movegui('south');
plot([0], [0], 'g-'); hold on; plot([0], [0], 'r-');plot([0], [0], 'k-');
xlabel('Time (s)');
ylabel('Flow Rate (Mbps)');

window_sizes = figure;
figure(window_sizes);
set(gcf,'numbertitle','off','name','Window Size vs Time');
movegui('northeast');
plot([0], [0], 'g-'); hold on; plot([0], [0], 'r-');plot([0], [0], 'k-');
xlabel('Time (s)');
ylabel('Window Size');


packet_delays = figure;
figure(packet_delays);
set(gcf,'numbertitle','off','name','Packet Delay vs Time');
movegui('southeast');
plot([0], [0], 'g-'); hold on; plot([0], [0], 'r-');plot([0], [0], 'k-');
xlabel('Time(s)');
ylabel('Packet Delay (ms)');

proj( time, lr1, lr2, lr3, bo1, bo2, bo3, ...
            pl1, pl2, pl3, fr1, fr2, fr3, ...
            ws1, ws2, ws3, pd1, pd2, pd3, ...
            'testcase1.txt', 100, 1);
clear mex;
   