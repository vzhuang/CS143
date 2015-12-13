/**
    CS-143 Network Simulator
    graphing.cpp
    Purpose: Populates Matlab-side data arrays with relevant data
    from the network simulator so that MATLAB can create plots. 

    @author Jordan Bonilla
    @version 1.0 12/11/15
*/

#include "graphing.hpp"
#define Mb_per_B 8.0 / 1000000.0
#define ms_per_s 1000.0

// Points to Matlab data arrays
static double * vector_time;
static double * lr1, * lr2, * lr3;
static double * bo1, * bo2, * bo3;
static double * pl1, * pl2, * pl3;
static double * fr1, * fr2, * fr3;
static double * ws1, * ws2, * ws3;
static double * pd1, * pd2, * pd3;
static int vector_index;
static int VECTOR_SIZE;
extern double global_time;
extern char * file_name;

// Create a Matlab plot legend appropriate for this testcase and data type
void spawn_appropriate_legend(bool link_graph){
    mexEvalString("axh = findobj( gcf, 'Type', 'Line' );");  
    // If this is a link graph
    if(link_graph == true) {
        if(!strcmp(file_name, "testcase2.txt"))
            mexEvalString("legend(axh, 'Link 1','Link 2', 'Link 3','Location','Best');");
        else if(!strcmp(file_name, "testcase1.txt"))
             mexEvalString("legend(axh(1:2), 'Link 1', 'Link 2','Location','Best');");
        else
             mexEvalString("legend(axh(1), 'Link 1','Location','Best');");
    }
    // Else this is a flow graph
    else {
        if(!strcmp(file_name, "testcase2.txt")){
            mexEvalString("legend(axh, 'Flow 1', 'Flow 2', 'Flow 3','Location','Best');");      }
        else if(!strcmp(file_name, "testcase1.txt"))
             mexEvalString("legend(axh(1),'Flow 1','Location','Best');");
        else
             mexEvalString("legend(axh(1),'Flow 1','Location','Best');");
    }
        
}
// Init components for all testcases
void init_graphs(Network * network, const mxArray **prhs) {
    
    // Monitor link1 and flow1 as in the testcase0 case
    // for network descriptor files that were not shown on the website. 
    if ( (strcmp(file_name, "testcase1.txt") != 0) && 
            (strcmp(file_name, "testcase2.txt") != 0) ) {
        file_name = (char *)"testcase0.txt";
    }
    // Create Link Rate graph
    mexEvalString("link_rates = figure;");
    mexEvalString("figure(link_rates);");
    mexEvalString("set(gcf,'numbertitle','off','name','Link Rate vs Time');");
    mexEvalString("movegui('northwest');");
    mexEvalString("plot([0], [0], 'g'); hold on; plot([0], [0], 'r');plot([0], [0], 'b');");
    mexEvalString("xlabel('Time (s)');");
    mexEvalString("ylabel('Link Rate (Mbps)');");
    spawn_appropriate_legend(true);
    // Create Buffer Occupancy graph
    mexEvalString("buffer_occupancies = figure;");
    mexEvalString("figure(buffer_occupancies);");
    mexEvalString("set(gcf,'numbertitle','off','name','Buffer Occupancy vs Time');");
    mexEvalString("movegui('southwest');");
    mexEvalString("plot([0], [0], 'g'); hold on; plot([0], [0], 'r');plot([0], [0], 'b');");
    mexEvalString("xlabel('Time (s)');");
    mexEvalString("ylabel('Buffer Occupancy (# packets)');");
    spawn_appropriate_legend(true);
    // Create Packet Loss graph
    mexEvalString("packet_losses = figure;");
    mexEvalString("figure(packet_losses);");
    mexEvalString("set(gcf,'numbertitle','off','name','Packet Loss vs Time');");
    mexEvalString("movegui('north');");
    mexEvalString("plot([0], [0], 'g'); hold on; plot([0], [0], 'r');plot([0], [0], 'b');");
    mexEvalString("xlabel('Time (s)');");
    mexEvalString("ylabel('Packet Loss (# packets)');");
    spawn_appropriate_legend(true);
    // Create Flow Rates graph
    mexEvalString("flow_rates = figure;");
    mexEvalString("figure(flow_rates);");
    mexEvalString("set(gcf,'numbertitle','off','name','Flow Rate vs Time');");
    mexEvalString("movegui('south');");
    mexEvalString("plot([0], [0], 'g'); hold on; plot([0], [0], 'r');plot([0], [0], 'b');");
    mexEvalString("xlabel('Time (s)');");
    mexEvalString("ylabel('Flow Rate (Mbps)');");
    mexEvalString("legend('Flow 3','Flow 2', 'Flow 1','Location','northwest');");
    spawn_appropriate_legend(false);
    // Create window sizes graph
    mexEvalString("window_sizes = figure;");
    mexEvalString("figure(window_sizes);");
    mexEvalString("set(gcf,'numbertitle','off','name','Window Size vs Time');");
    mexEvalString("movegui('northeast');");
    mexEvalString("plot([0], [0], 'g'); hold on; plot([0], [0], 'r');plot([0], [0], 'b');");
    mexEvalString("xlabel('Time (s)');");
    mexEvalString("ylabel('Window Size');");
    mexEvalString("legend('Flow 3','Flow 2', 'Flow 1','Location','northwest');");
    spawn_appropriate_legend(false);
    // Create Packet Delays graph
    mexEvalString("packet_delays = figure;");
    mexEvalString("figure(packet_delays);");
    mexEvalString("set(gcf,'numbertitle','off','name','Packet Delay vs Time');");
    mexEvalString("movegui('southeast');");
    mexEvalString("plot([0], [0], 'g'); hold on; plot([0], [0], 'r');plot([0], [0], 'b');");
    mexEvalString("xlabel('Time(s)');");
    mexEvalString("ylabel('Packet Delay (ms)');");
    mexEvalString("legend('Flow 3','Flow 2', 'Flow 1','Location','northwest');");    
    spawn_appropriate_legend(false);
    // Initialize counter to populate data arrays
    vector_index = 0;
    // Init data pointers
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
    // Read vector size from Matlab side to prevent data overflow
    VECTOR_SIZE = (int) mxGetScalar(prhs[22]);
    mexPrintf("Graph Initilization complete\n");
}

// Increment the data counter and populate the next iteration of data.
// Note that mexEvalString() calls switch control back over to MATLAB and
// executes the command enclosed in the braces. In this case, mexEvalString
// calls are used exclusively to update the figure values. 
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

// Update Link Rates
void update_lrs(Network * network) {
    mexEvalString("figure = link_rates;");
    mexEvalString("axh = findobj( link_rates, 'Type', 'Line' );");  
    if (!strcmp(file_name, "testcase0.txt")) {
        lr1[vector_index] = network->all_links[0]->get_flowrate()* Mb_per_B;
        mexEvalString("set(axh(1), 'XData', time, 'YData', lr1)");
    }
    if (!strcmp(file_name, "testcase1.txt")) {
        lr1[vector_index] = network->all_links[1]->get_flowrate()* Mb_per_B;
        mexEvalString("set(axh(1), 'XData', time, 'YData', lr1)");
        lr2[vector_index] = network->all_links[2]->get_flowrate()* Mb_per_B;
        mexEvalString("set(axh(2), 'XData', time, 'YData', lr2)");
    }
    if (!strcmp(file_name, "testcase2.txt")) {
        lr1[vector_index] = network->all_links[0]->get_flowrate()* Mb_per_B;
        mexEvalString("set(axh(1), 'XData', time, 'YData', lr1)");
        lr2[vector_index] = network->all_links[1]->get_flowrate()* Mb_per_B;
        mexEvalString("set(axh(2), 'XData', time, 'YData', lr2)");
        lr3[vector_index] = network->all_links[2]->get_flowrate()* Mb_per_B;
        mexEvalString("set(axh(3), 'XData', time, 'YData', lr3)");
    }
}   
// Update Buffer Occupancies
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
// Update Packet Delays
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
// Update Flow Rates
void update_frs(Network * network) {
    mexEvalString("figure = flow_rates;");
    mexEvalString("axh = findobj( flow_rates, 'Type', 'Line' );");  
    if (!strcmp(file_name, "testcase0.txt")) {
        fr1[vector_index] = network->all_flows[0]->get_flowrate();
        mexEvalString("set(axh(1), 'XData', time, 'YData', fr1)");
    }
    if (!strcmp(file_name, "testcase1.txt")) {
        fr1[vector_index] = network->all_flows[0]->get_flowrate();
        mexEvalString("set(axh(1), 'XData', time, 'YData', fr1)");
    }
    if (!strcmp(file_name, "testcase2.txt")) {
        fr1[vector_index] = network->all_flows[0]->get_flowrate();
        mexEvalString("set(axh(1), 'XData', time, 'YData', fr1)");
        fr2[vector_index] = network->all_flows[1]->get_flowrate();
        mexEvalString("set(axh(2), 'XData', time, 'YData', fr2)");
        fr3[vector_index] = network->all_flows[2]->get_flowrate();
        mexEvalString("set(axh(3), 'XData', time, 'YData', fr3)");
    } 
}
// Update Window Sizes
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
// Update Pakcet Delays
void update_pds(Network * network) {
    mexEvalString("figure = packet_delays;");
    mexEvalString("axh = findobj( packet_delays, 'Type', 'Line' );");  
    if (!strcmp(file_name, "testcase0.txt")) {
        pd1[vector_index] = network->all_flows[0]->rtt * ms_per_s;
        mexEvalString("set(axh(1), 'XData', time, 'YData', pd1)");
    }
    if (!strcmp(file_name, "testcase1.txt")) {
        pd1[vector_index] = network->all_flows[0]->rtt * ms_per_s;
        mexEvalString("set(axh(1), 'XData', time, 'YData', pd1)");
    }
    if (!strcmp(file_name, "testcase2.txt")) {
        pd1[vector_index] = network->all_flows[0]->rtt * ms_per_s;
        mexEvalString("set(axh(1), 'XData', time, 'YData', pd1)");
        pd2[vector_index] = network->all_flows[1]->rtt * ms_per_s;
        mexEvalString("set(axh(2), 'XData', time, 'YData', pd2)");
        pd3[vector_index] = network->all_flows[2]->rtt * ms_per_s;
        mexEvalString("set(axh(3), 'XData', time, 'YData', pd3)");
    } 
}

// Cut out zeros and call smooth() on vectors for asthetics
void plot_final_points() {

    // Buffer to dynamically spawn mexEvalString calls
    char command[50];
    // Shorten each graph to the length that was actually populated
    sprintf(command, "time = time(1:%d);", vector_index); 
    mexEvalString(command);
    sprintf(command, "lr1 = lr1(1:%d);", vector_index); 
    mexEvalString(command);
    sprintf(command, "lr2 = lr2(1:%d);", vector_index); 
    mexEvalString(command);
    sprintf(command, "lr3 = lr3(1:%d);", vector_index); 
    mexEvalString(command);
    sprintf(command, "pd1 = pd1(1:%d);", vector_index); 
    mexEvalString(command);
    sprintf(command, "pd2 = pd2(1:%d);", vector_index); 
    mexEvalString(command);
    sprintf(command, "pd3 = pd3(1:%d);", vector_index); 
    mexEvalString(command);
    sprintf(command, "bo1 = bo1(1:%d);", vector_index); 
    mexEvalString(command);
    sprintf(command, "bo2 = bo2(1:%d);", vector_index); 
    mexEvalString(command);
    sprintf(command, "bo3 = bo3(1:%d);", vector_index); 
    mexEvalString(command);
    sprintf(command, "pl1 = pl1(1:%d);", vector_index); 
    mexEvalString(command);
    sprintf(command, "pl2 = pl2(1:%d);", vector_index); 
    mexEvalString(command);
    sprintf(command, "pl3 = pl3(1:%d);", vector_index); 
    mexEvalString(command);
    sprintf(command, "ws1 = ws1(1:%d);", vector_index); 
    mexEvalString(command);
    sprintf(command, "ws2 = ws2(1:%d);", vector_index); 
    mexEvalString(command);
    sprintf(command, "ws3 = ws3(1:%d);", vector_index); 
    mexEvalString(command);    
    sprintf(command, "fr1 = fr1(1:%d);", vector_index); 
    mexEvalString(command);
    sprintf(command, "fr2 = fr2(1:%d);", vector_index); 
    mexEvalString(command);
    sprintf(command, "fr3 = fr3(1:%d);", vector_index); 
    mexEvalString(command);
    
	// Smooth data for final plot
    mexEvalString(  "lr1 = smooth(lr1);"
                    "lr2 = smooth(lr2);"
                    "lr3 = smooth(lr3);" );
    mexEvalString(  "bo1 = smooth(bo1);"
                    "bo2 = smooth(bo2);"
                    "bo3 = smooth(bo3);" );
    mexEvalString(  "pd1 = smooth(pd1);"
                    "pd2 = smooth(pd2);"
                    "pd3 = smooth(pd3);" );
                    
    // Proceed to update graphs
    mexEvalString("figure = link_rates;");
    mexEvalString("axh = findobj( link_rates, 'Type', 'Line' );");  
    if (!strcmp(file_name, "testcase0.txt")) {
        mexEvalString("set(axh(1), 'XData', time, 'YData', lr1)");
    }
    if (!strcmp(file_name, "testcase1.txt")) {
        mexEvalString("set(axh(1), 'XData', time, 'YData', lr1)");
        mexEvalString("set(axh(2), 'XData', time, 'YData', lr2)");
    }
    if (!strcmp(file_name, "testcase2.txt")) {
        mexEvalString("set(axh(1), 'XData', time, 'YData', lr1)");
        mexEvalString("set(axh(2), 'XData', time, 'YData', lr2)");
        mexEvalString("set(axh(3), 'XData', time, 'YData', lr3)");
    }
    
    mexEvalString("figure = packet_delays;");
    mexEvalString("axh = findobj( packet_delays, 'Type', 'Line' );");  
    if (!strcmp(file_name, "testcase0.txt")) {
        mexEvalString("set(axh(1), 'XData', time, 'YData', pd1)");
    }
    if (!strcmp(file_name, "testcase1.txt")) {
        mexEvalString("set(axh(1), 'XData', time, 'YData', pd1)");
    }
    if (!strcmp(file_name, "testcase2.txt")) {
        mexEvalString("set(axh(1), 'XData', time, 'YData', pd1)");
        mexEvalString("set(axh(2), 'XData', time, 'YData', pd2)");
        mexEvalString("set(axh(3), 'XData', time, 'YData', pd3)");
    }

    mexEvalString("figure = buffer_occupancies;");
    mexEvalString("axh = findobj( buffer_occupancies, 'Type', 'Line' );");  
    if (!strcmp(file_name, "testcase0.txt")) {
        mexEvalString("set(axh(1), 'XData', time, 'YData', bo1)");
    }
    if (!strcmp(file_name, "testcase1.txt")) {
        mexEvalString("set(axh(1), 'XData', time, 'YData', bo1)");
        mexEvalString("set(axh(2), 'XData', time, 'YData', bo2)");
    }
    if (!strcmp(file_name, "testcase2.txt")) {
        mexEvalString("set(axh(1), 'XData', time, 'YData', bo1)");
        mexEvalString("set(axh(2), 'XData', time, 'YData', bo2)");
        mexEvalString("set(axh(3), 'XData', time, 'YData', bo3)");
    }

    mexEvalString("figure = packet_losses;");
    mexEvalString("axh = findobj( packet_losses, 'Type', 'Line' );");  
    if (!strcmp(file_name, "testcase0.txt")) {
        mexEvalString("set(axh(1), 'XData', time, 'YData', pl1)");
    }
    if (!strcmp(file_name, "testcase1.txt")) {
        mexEvalString("set(axh(1), 'XData', time, 'YData', pl1)");
        mexEvalString("set(axh(2), 'XData', time, 'YData', pl2)");
    }
    if (!strcmp(file_name, "testcase2.txt")) {
        mexEvalString("set(axh(1), 'XData', time, 'YData', pl1)");
        mexEvalString("set(axh(2), 'XData', time, 'YData', pl2)");
        mexEvalString("set(axh(3), 'XData', time, 'YData', pl3)");
    } 
    
    mexEvalString("figure = flow_rates;");
    mexEvalString("axh = findobj( flow_rates, 'Type', 'Line' );");  
    if (!strcmp(file_name, "testcase0.txt")) {
        mexEvalString("set(axh(1), 'XData', time, 'YData', fr1)");
    }
    if (!strcmp(file_name, "testcase1.txt")) {
        mexEvalString("set(axh(1), 'XData', time, 'YData', fr1)");
    }
    if (!strcmp(file_name, "testcase2.txt")) {
        mexEvalString("set(axh(1), 'XData', time, 'YData', fr1)");
        mexEvalString("set(axh(2), 'XData', time, 'YData', fr2)");
        mexEvalString("set(axh(3), 'XData', time, 'YData', fr3)");
    } 

    mexEvalString("figure = window_sizes;");
    mexEvalString("axh = findobj( window_sizes, 'Type', 'Line' );");  
    if (!strcmp(file_name, "testcase0.txt")) {
        mexEvalString("set(axh(1), 'XData', time, 'YData', ws1)");
    }
    if (!strcmp(file_name, "testcase1.txt")) {
        mexEvalString("set(axh(1), 'XData', time, 'YData', ws1)");
    }
    if (!strcmp(file_name, "testcase2.txt")) {
        mexEvalString("set(axh(1), 'XData', time, 'YData', ws1)");
        mexEvalString("set(axh(2), 'XData', time, 'YData', ws2)");
        mexEvalString("set(axh(3), 'XData', time, 'YData', ws3)");
    } 
    
    // Flush graphs
    mexEvalString("pause(.00001)");
}
