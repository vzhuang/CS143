clear mex;
close all;
clear all;

% File to parse to set up network
NETWORK_DESCRIPTOR_FILE = 'testcase2.txt';
% Maximum simulation time (s)
MAX_SIM_TIME = 100;
% 1 for Reno, 2 for Fast
TCP_ID = 1;

% Max sixe of data vector. 
VECTOR_SIZE = 100000;
% X data
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

proj( time, lr1, lr2, lr3, bo1, bo2, bo3, ...
            pl1, pl2, pl3, fr1, fr2, fr3, ...
            ws1, ws2, ws3, pd1, pd2, pd3, ...
            NETWORK_DESCRIPTOR_FILE, ...
            MAX_SIM_TIME, TCP_ID, VECTOR_SIZE);
clear mex;
   
