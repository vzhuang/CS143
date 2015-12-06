#include "node.h"
#include "packet.h"
#include "flow.h"
#include "link.h"
#include "parser.h"
#include "event.h"
#include "network.h"
#include "graphing.h"
#include <vector>
#include "mex.h"
#define SAMPLING_RATE 0.1
#define REFRESH_RATE 10.0
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
	if(nrhs != 21)
	{
        mexPrintf("nrhs: %d\n", nrhs);
		mexPrintf("./proj {data arrays} [network.txt] [double simulation_time (s)] [TCP_ID]\n");
	}
	char * network_file = mxArrayToString(prhs[19]);
	end_time = (double) mxGetScalar(prhs[20]);
	TCP_ID = (int) mxGetScalar(prhs[21]);
	
	// Build network by parsing the input network file
	build_network(&network, network_file);

	// Create an event to update the routing tables
	Update_Rtables_Event * event = new Update_Rtables_Event(0 - EPSILON, TCP_ID, &network);
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
	double prev_time0 = 0.0;
	double prev_time1 = 0.0;
	// Keep dequeuing events and handling them
	while( (!event_queue.empty()) && (global_time <= end_time) )
	{
		double data_sampler = global_time - prev_time0;
		if(data_sampler > SAMPLING_RATE) {
			prev_time0 = global_time;
			update_graphs(&network, prhs);
		}
		double refresh_sampler = global_time - prev_time1;
		if(refresh_sampler > REFRESH_RATE) {
			// Set the distance vector to reflect the most recent link costs

			cout << "Time: " << global_time << "\n";
			for (int i = 0; i < network.all_links.size(); i++) {
				Link * link = network.all_links.at(i);
				link->set_flowrate();
				cout << "Cost for link " << link_to_english(&network, link) << ":  "<< link->calculate_cost() << "\n";
			}

			prev_time1 = global_time;
			Update_Rtables_Event * event = new Update_Rtables_Event(global_time, TCP_ID, &network);
			event_queue.push(event);
		}

		Event * to_handle = event_queue.top();
		event_queue.pop();
		to_handle->handle_event();
		delete to_handle;


	}
    plot_final_points();
	mexPrintf("end time: %f, global_time: %f\n", end_time, global_time);
	mexPrintf("Exiting\n");
	
}