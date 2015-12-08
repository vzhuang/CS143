#include "graphing.h"
#define VECTOR_SIZE 10000

static double * vector_time;
static double * lr1, * lr2, * lr3;
static double * bo1, * bo2, * bo3;
static double * pl1, * pl2, * pl3;
static double * fr1, * fr2, * fr3;
static double * ws1, * ws2, * ws3;
static double * pd1, * pd2, * pd3;
static int vector_index;

extern double global_time;
extern char * file_name;


// Init components for all testcases
void init_graphs(Network * network, const mxArray **prhs) {

    // Initialize counter
    vector_index = 0;
    // Init data pointers
    // Time
    vector_time = mxGetPr(prhs[0]);
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
    mexPrintf("Graph Init complete\n");
}

void update_graphs(Network * network, const mxArray **prhs) {
    // Increment time
    vector_index++;
    vector_time[vector_index] = global_time;
    // Boundary check
    if(vector_index >= VECTOR_SIZE - 1) {
        mexPrintf("Max data points exceeded\n");
        return;
    }

    // Update flowrate
    update_lrs(network);
    // Update buffer occupancy
    update_bos(network);
    // Update packet loss
    update_pls(network);
    // Update buffer flow rates
    update_frs(network);
    // Update window sizes
    update_wss(network);
   // Update packet delays
    update_pds(network);
    // Flush graphs
    mexEvalString("pause(.00001)");
}

void update_lrs(Network * network) {
    mexEvalString("figure = link_rates;");
    mexEvalString("axh = findobj( link_rates, 'Type', 'Line' );");  
    if (!strcmp(file_name, "testcase0.txt")) {
        lr1[vector_index] = network->all_links[0]->get_flowrate()* 8.0 / 1000000.0;
        mexEvalString("set(axh(1), 'XData', time, 'YData', lr1)");
    }
    if (!strcmp(file_name, "testcase1.txt")) {
        lr1[vector_index] = network->all_links[1]->get_flowrate()* 8.0 / 1000000.0;
        mexEvalString("set(axh(1), 'XData', time, 'YData', lr1)");
        lr2[vector_index] = network->all_links[2]->get_flowrate()* 8.0 / 1000000.0;
        mexEvalString("set(axh(2), 'XData', time, 'YData', lr2)");
    }
    if (!strcmp(file_name, "testcase2.txt")) {
        lr1[vector_index] = network->all_links[0]->get_flowrate()* 8.0 / 1000000.0;
        mexEvalString("set(axh(1), 'XData', time, 'YData', lr1)");
        lr2[vector_index] = network->all_links[1]->get_flowrate()* 8.0 / 1000000.0;
        mexEvalString("set(axh(2), 'XData', time, 'YData', lr2)");
        lr3[vector_index] = network->all_links[2]->get_flowrate()* 8.0 / 1000000.0;
        mexEvalString("set(axh(3), 'XData', time, 'YData', lr3)");
    }
}   

void update_bos(Network * network) {
    mexEvalString("figure = buffer_occupancies;");
    mexEvalString("axh = findobj( buffer_occupancies, 'Type', 'Line' );");  
    if (!strcmp(file_name, "testcase0.txt")) {
        bo1[vector_index] = (double) network->all_links[0]->get_packets_stored();
        mexEvalString("set(axh(1), 'XData', time, 'YData', bo1)");
    }
    if (!strcmp(file_name, "testcase1.txt")) {
        bo1[vector_index] = (double)network->all_links[1]->get_packets_stored();
        mexEvalString("set(axh(1), 'XData', time, 'YData', bo1)");
        bo2[vector_index] = (double)network->all_links[2]->get_packets_stored();
        mexEvalString("set(axh(2), 'XData', time, 'YData', bo2)");
    }
    if (!strcmp(file_name, "testcase2.txt")) {
        bo1[vector_index] = (double) network->all_links[0]->get_packets_stored();
        mexEvalString("set(axh(1), 'XData', time, 'YData', bo1)");
        bo2[vector_index] = (double) network->all_links[1]->get_packets_stored();
        mexEvalString("set(axh(2), 'XData', time, 'YData', bo2)");
        bo3[vector_index] = (double) network->all_links[2]->get_packets_stored();
        mexEvalString("set(axh(3), 'XData', time, 'YData', bo3)");
    }
}

void update_pls(Network * network) {
    mexEvalString("figure = packet_losses;");
    mexEvalString("axh = findobj( packet_losses, 'Type', 'Line' );");  
    if (!strcmp(file_name, "testcase0.txt")) {
        pl1[vector_index] = (double) network->all_links[0]->get_packets_dropped();
        mexEvalString("set(axh(1), 'XData', time, 'YData', pl1)");
    }
    if (!strcmp(file_name, "testcase1.txt")) {
        pl1[vector_index] = (double)network->all_links[1]->get_packets_dropped();
        mexEvalString("set(axh(1), 'XData', time, 'YData', pl1)");
        pl2[vector_index] = (double)network->all_links[2]->get_packets_dropped();
        mexEvalString("set(axh(2), 'XData', time, 'YData', pl2)");
    }
    if (!strcmp(file_name, "testcase2.txt")) {
        pl1[vector_index] = (double) network->all_links[0]->get_packets_dropped();
        mexEvalString("set(axh(1), 'XData', time, 'YData', pl1)");
        pl2[vector_index] = (double) network->all_links[1]->get_packets_dropped();
        mexEvalString("set(axh(2), 'XData', time, 'YData', pl2)");
        pl3[vector_index] = (double) network->all_links[2]->get_packets_dropped();
        mexEvalString("set(axh(3), 'XData', time, 'YData', pl3)");
    } 
}
void update_frs(Network * network) {
    mexEvalString("figure = flow_rates;");
    mexEvalString("axh = findobj( flow_rates, 'Type', 'Line' );");  
    if (!strcmp(file_name, "testcase0.txt")) {
        fr1[vector_index] = network->all_flows[0]->get_flowrate();
        mexEvalString("set(axh(1), 'XData', time, 'YData', fr1)");
    }
    if (!strcmp(file_name, "testcase1.txt")) {
        fr2[vector_index] = network->all_flows[0]->get_flowrate();
        mexEvalString("set(axh(1), 'XData', time, 'YData', fr2)");
    }
    if (!strcmp(file_name, "testcase2.txt")) {
        fr1[vector_index] = network->all_flows[0]->get_flowrate();
        mexEvalString("set(axh(1), 'XData', time, 'YData', pl1)");
        fr2[vector_index] = network->all_flows[1]->get_flowrate();
        mexEvalString("set(axh(2), 'XData', time, 'YData', pl2)");
        fr3[vector_index] = network->all_flows[2]->get_flowrate();
        mexEvalString("set(axh(3), 'XData', time, 'YData', pl3)");
    } 
}
void update_wss(Network * network) {
    mexEvalString("figure = window_sizes;");
    mexEvalString("axh = findobj( window_sizes, 'Type', 'Line' );");  
    if (!strcmp(file_name, "testcase0.txt")) {
        ws1[vector_index] = network->all_flows[0]->window_size;
        mexEvalString("set(axh(1), 'XData', time, 'YData', ws1)");
    }
    if (!strcmp(file_name, "testcase1.txt")) {
        ws1[vector_index] = network->all_flows[0]->window_size;
        mexEvalString("set(axh(1), 'XData', time, 'YData', ws1)");
    }
    if (!strcmp(file_name, "testcase2.txt")) {
        ws1[vector_index] = network->all_flows[0]->window_size;
        mexEvalString("set(axh(1), 'XData', time, 'YData', ws1)");
        ws2[vector_index] = network->all_flows[1]->window_size;
        mexEvalString("set(axh(2), 'XData', time, 'YData', ws2)");
        ws3[vector_index] = network->all_flows[2]->window_size;
        mexEvalString("set(axh(3), 'XData', time, 'YData', ws3)");
    } 
}
void update_pds(Network * network) {
    mexEvalString("figure = packet_delays;");
    mexEvalString("axh = findobj( packet_delays, 'Type', 'Line' );");  
    if (!strcmp(file_name, "testcase0.txt")) {
        pd1[vector_index] = network->all_flows[0]->rtt * 1000.0;
        mexEvalString("set(axh(1), 'XData', time, 'YData', pd1)");
    }
    if (!strcmp(file_name, "testcase1.txt")) {
        pd1[vector_index] = network->all_flows[0]->rtt * 1000.0;
        mexEvalString("set(axh(1), 'XData', time, 'YData', pd1)");
    }
    if (!strcmp(file_name, "testcase2.txt")) {
        pd1[vector_index] = network->all_flows[0]->rtt * 1000.0;;
        mexEvalString("set(axh(1), 'XData', time, 'YData', pd1)");
        pd2[vector_index] = network->all_flows[1]->rtt * 1000.0;;
        mexEvalString("set(axh(2), 'XData', time, 'YData', pd2)");
        pd3[vector_index] = network->all_flows[2]->rtt * 1000.0;
        mexEvalString("set(axh(3), 'XData', time, 'YData', pd3)");
    } 
}
// PLOT FINAL POINTS
void plot_final_points() {
    mexEvalString("figure = packet_delays;");
    mexEvalString("axh = findobj( packet_delays, 'Type', 'Line' );");  
    if (!strcmp(file_name, "testcase0.txt")) {
        pd1[vector_index] = 0;
        mexEvalString("set(axh(1), 'XData', time, 'YData', pd1)");
    }
    if (!strcmp(file_name, "testcase1.txt")) {
        pd1[vector_index] = 0;
        mexEvalString("set(axh(1), 'XData', time, 'YData', pd1)");
        pd2[vector_index] = 0;
        mexEvalString("set(axh(2), 'XData', time, 'YData', pd2)");
    }
    if (!strcmp(file_name, "testcase2.txt")) {
        pd1[vector_index] = 0;
        mexEvalString("set(axh(1), 'XData', time, 'YData', pd1)");
        pd2[vector_index] = 0;
        mexEvalString("set(axh(2), 'XData', time, 'YData', pd2)");
        pd3[vector_index] = 0;
        mexEvalString("set(axh(3), 'XData', time, 'YData', pd3)");
    }



    mexEvalString("figure = buffer_occupancies;");
    mexEvalString("axh = findobj( buffer_occupancies, 'Type', 'Line' );");  
    if (!strcmp(file_name, "testcase0.txt")) {
        bo1[vector_index] = 0;
        mexEvalString("set(axh(1), 'XData', time, 'YData', bo1)");
    }
    if (!strcmp(file_name, "testcase1.txt")) {
        bo1[vector_index] = 0;
        mexEvalString("set(axh(1), 'XData', time, 'YData', bo1)");
        bo2[vector_index] = 0;
        mexEvalString("set(axh(2), 'XData', time, 'YData', bo2)");
    }
    if (!strcmp(file_name, "testcase2.txt")) {
        bo1[vector_index] = 0;
        mexEvalString("set(axh(1), 'XData', time, 'YData', bo1)");
        bo2[vector_index] = 0;
        mexEvalString("set(axh(2), 'XData', time, 'YData', bo2)");
        bo3[vector_index] = 0;
        mexEvalString("set(axh(3), 'XData', time, 'YData', bo3)");
    }



    mexEvalString("figure = packet_losses;");
    mexEvalString("axh = findobj( packet_losses, 'Type', 'Line' );");  
    if (!strcmp(file_name, "testcase0.txt")) {
        pl1[vector_index] = 0;
        mexEvalString("set(axh(1), 'XData', time, 'YData', pl1)");
    }
    if (!strcmp(file_name, "testcase1.txt")) {
        pl1[vector_index] = 0;
        mexEvalString("set(axh(1), 'XData', time, 'YData', pl1)");
        pl2[vector_index] = 0;
        mexEvalString("set(axh(2), 'XData', time, 'YData', pl2)");
    }
    if (!strcmp(file_name, "testcase2.txt")) {
        pl1[vector_index] = 0;
        mexEvalString("set(axh(1), 'XData', time, 'YData', pl1)");
        pl2[vector_index] = 0;
        mexEvalString("set(axh(2), 'XData', time, 'YData', pl2)");
        pl3[vector_index] = 0;
        mexEvalString("set(axh(3), 'XData', time, 'YData', pl3)");
    } 


    mexEvalString("figure = flow_rates;");
    mexEvalString("axh = findobj( flow_rates, 'Type', 'Line' );");  
    if (!strcmp(file_name, "testcase0.txt")) {
        fr1[vector_index] = 0;
        mexEvalString("set(axh(1), 'XData', time, 'YData', fr1)");
    }
    if (!strcmp(file_name, "testcase1.txt")) {
        fr1[vector_index] = 0;
        mexEvalString("set(axh(1), 'XData', time, 'YData', fr1)");
    }
    if (!strcmp(file_name, "testcase2.txt")) {
        fr1[vector_index] = 0;
        mexEvalString("set(axh(1), 'XData', time, 'YData', fr1)");
        fr2[vector_index] = 0;
        mexEvalString("set(axh(2), 'XData', time, 'YData', fr2)");
        fr3[vector_index] = 0;
        mexEvalString("set(axh(3), 'XData', time, 'YData', fr3)");
    } 


    mexEvalString("figure = window_sizes;");
    mexEvalString("axh = findobj( window_sizes, 'Type', 'Line' );");  
    if (!strcmp(file_name, "testcase0.txt")) {
        ws1[vector_index] = 0;
        mexEvalString("set(axh(1), 'XData', time, 'YData', ws1)");
    }
    if (!strcmp(file_name, "testcase1.txt")) {
        ws2[vector_index] = 0;
        mexEvalString("set(axh(1), 'XData', time, 'YData', ws1)");
    }
    if (!strcmp(file_name, "testcase2.txt")) {
        ws1[vector_index] = 0;
        mexEvalString("set(axh(1), 'XData', time, 'YData', ws1)");
        ws2[vector_index] = 0;
        mexEvalString("set(axh(2), 'XData', time, 'YData', ws2)");
        ws3[vector_index] = 0;
        mexEvalString("set(axh(3), 'XData', time, 'YData', ws3)");
    } 
    
    
    // Flush graphs
    mexEvalString("pause(.00001)");
}
