#include "graphing.h"
#define VECTOR_SIZE 10000

static double * time;
static double * lr1, lr2, lr3;
static double * bo1, bo2, bo3;
static double * pl1, pl2, pl3;
static double * fr1, fr2, fr3;
static double * ws1, ws2, ws3;
static double * pd1, pd2, pd3;
static int vector_index;

extern double global_time;
extern char * file_name;


// Init components for all testcases
void init_graphs(Network * network, mxArray **plhs) {

    // Initialize counter
    vector_index = 0;
    // Init data pointers
    // Time
    time = mxGetPr(prhs[0]);
    // Link rates
    lr1 = mxGetPr(prhs[1]);
    lr2 = mxGetPr(prhs[2]);
    lr3 = mxGetPr(prhs[3]);
    // Buffer occupancies
    bo1 = mxGetPr(prhs[4]);
    bo2 = mxGetPr(prhs[5]);
    bo3 = mxGetPr(prhs[6]);
    // Packet Losses
    pl1 = mxGetPr(prhs[7]);
    pl2 = mxGetPr(prhs[8]);
    pl3 = mxGetPr(prhs[9]);
    // Flow Rates
    fr1 = mxGetPr(prhs[10]);
    fr2 = mxGetPr(prhs[11]);
    fr3 = mxGetPr(prhs[12]);
    // Window Sizes
    ws1 = mxGetPr(prhs[13]);
    ws2 = mxGetPr(prhs[14]);
    ws3 = mxGetPr(prhs[15]);
    // Packet Delays
    pd1 = mxGetPr(prhs[16]);
    pd2 = mxGetPr(prhs[17]);
    pd3 = mxGetPr(prhs[18]);
}

void update_graphs(Network * network, mxArray **plhs) {
    // Increment time
    vector_index++;
    time[vector_index] = global_time;
    // Boundary check
    if(vector_index >= VECTOR_SIZE) {
        mexPrintf("Max data points exceeded\n");
        return;
    }
    // Update flowrate
    update_flow();
    // Update buffer occupancy
    update_bo();
    // Flush graphs
    mexEvalString("pause(.00001)");
}

void update_flow()
{
    mexEvalString("figure = link_rates;");
    mexEvalString("axh = findobj( link_rates, 'Type', 'Line' );");  
    if (!strcmp(file_name, "testcase0.txt")) {
        flow1[i] = network->all_links[0]->get_flowrate()* 8.0 / 1000000.0;
        mexEvalString("set(axh(1), 'XData', time, 'YData', lr1)");
    }
    if (!strcmp(file_name, "testcase1.txt")) {
        flow2[i] = -1 * i;
    }
    if (!strcmp(file_name, "testcase2.txt")) {
        flow1[i] = i;
    }

    mexEvalString("set(axh(3), 'XData', time, 'YData', lr1)");
    mexEvalString("set(axh(2), 'XData', time, 'YData', lr2)");
    mexEvalString("set(axh(1), 'XData', time, 'YData', lr3)");
}   

void update_testcase1(Network * network, mxArray **plhs)
{
    vector_time[vector_index] = global_time;
    vector_flow_rate1[vector_index] = network->all_links[1]->get_flowrate()* 8.0 / 1000000.0;
    vector_flow_rate2[vector_index] = network->all_links[2]->get_flowrate()* 8.0 / 1000000.0;
    vector_bo1[vector_index] = network->all_links[1]->packets_stored;
    vector_bo2[vector_index] = network->all_links[2]->packets_stored;
    // Refresh flow rate graph
    mexEvalString("figure(fig1)");
    inplot[0] = plhs[0]; // time
    inplot[1] = plhs[1]; // set x axis to link1 flowrate
    mexCallMATLAB(0, NULL, 3, inplot, "plot");
    mexEvalString("hold on");
    inplot[1] = plhs[2]; //set y axis to link2 flowrate
    mexCallMATLAB(0, NULL, 3, inplot, "plot");
    mexEvalString("hold off");
    // Refresh buffer occupancy rate graph
    mexEvalString("figure(fig2)");
    inplot[1] = plhs[3]; // set y axis to link1 buffer occupancy
    mexCallMATLAB(0, NULL, 2, inplot, "plot");
    mexEvalString("hold on");
    inplot[1] = plhs[4]; // set y axis to link2 buffer occupancy
    mexCallMATLAB(0, NULL, 2, inplot, "plot");
    mexEvalString("hold off");
}
