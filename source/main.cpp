#include "node.hpp"
#include "packet.hpp"
#include "flow.hpp"
#include "link.hpp"
#include "parser.hpp"
#include "event.hpp"
#include "network.hpp"
#include "graphing.hpp"
#include <vector>
#include "mex.h"
#define SAMPLING_RATE 0.06
#define REFRESH_RATE 5.0
double global_time;
double end_time;
Network network;
	
/* Global variable that describes the type of congestion control.
   Different IDs are defioned in flow.h  */
int TCP_ID; 
/* Defines a priotiy queue that uses Event *'s. CompareEvents is defined
   in event.h and allows the priority to know which events get priority */
priority_queue<Event *, vector<Event *>, CompareEvents> event_queue;
priority_queue<Event *, vector<Event *>, CompareEvents> routing_queue;


void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, mxArray const *prhs[]) {
    printf("Inside\n");
	if(nrhs != 23)
	{
        mexPrintf("nrhs: %d\n", nrhs);
        mexErrMsgTxt("./proj {data arrays} [network.txt] [double simulation_time (s)] [TCP_ID]\n");
	}
	char * network_file = mxArrayToString(prhs[19]);
	end_time = (double) mxGetScalar(prhs[20]);
	TCP_ID = (int) mxGetScalar(prhs[21]);
	
	// Build network by parsing the input network file
	build_network(&network, network_file);

	// Create an event to update the routing tables before any flows begin
	Update_Rtables_Event * event = new Update_Rtables_Event(0 - EPSILON, &network);
	event_queue.push(event);

	init_graphs(&network, prhs);
	// Push a "Flow_Start_Event" for every flow in the network
	int num_flows = network.all_flows.size();	
	for(int i = 0; i < num_flows; i++)
	{
		Flow * this_flow = network.all_flows[i];
		double start = this_flow->get_start();
		Flow_Start_Event * event = new Flow_Start_Event(start, TCP_ID, this_flow);
		event_queue.push(event);
	}
    // Times of the last data plot/routing table update
	double prev_data_query = 0.0;
	double prev_routing_query = 0.0;
	// Keep dequeuing events and handling them
	while( (!event_queue.empty()) && (global_time <= end_time) )
	{
		double data_sampler = global_time - prev_data_query;
		if(data_sampler > SAMPLING_RATE) {
			prev_data_query = global_time;
			update_graphs(&network, prhs);
		}
		double refresh_sampler = global_time - prev_routing_query;
		if(refresh_sampler > REFRESH_RATE) {
			// Set the distance vector to reflect the most recent link costs
			cout << "Time: " << global_time << "\n";
			for (int i = 0; i < network.all_links.size(); i++) {
				Link * link = network.all_links.at(i);
				link->set_flowrate();
				cout << "Cost for link " << link_to_english(&network, link) << ":  "<< link->calculate_cost() << "\n";
			}

			prev_routing_query = global_time;
			Update_Rtables_Event * event = new Update_Rtables_Event(global_time, &network);
			event_queue.push(event);
		}

		Event * to_handle = event_queue.top();
		event_queue.pop();
		to_handle->handle_event();
		delete to_handle;
	}
	// Flush out the graphs
    plot_final_points();
	mexPrintf("end time: %f, global_time: %f\n", end_time, global_time);
	mexPrintf("Exiting\n");
	
}
