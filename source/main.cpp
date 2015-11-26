#include "node.h"
#include "packet.h"
#include "flow.h"
#include "link.h"
#include "parser.h"
#include "event.h"
#include "network.h"
#include <vector>

double global_time;
double end_time;
Network network;
	
/* Global variable that describes the type of congestion control.
   Different IDs are defioned in flow.h  */
int TCP_ID; 
/* Defines a priotiy queue that uses Event *'s. CompareEvents is defined
   in event.h and allows the priority to know which events get priority */
priority_queue<Event *, vector<Event *>, CompareEvents> event_queue;

int main(int argc, char *argv[]) {
	if(argc != 4)
	{
		printf("./proj [network.txt] [double simulation_time (s)] [TCP_ID]\n");
		exit(-1);
	}
	char * network_file = argv[1];
	end_time = stod(argv[2]);
	TCP_ID = stod(argv[3]);
	// Build network by parsing the input network file
	build_network(&network, network_file);
	// Iterate over all routers and create routing tables
	for (int i = 0; i < network.all_routers.size(); i++) {
		Router * source = network.all_routers.at(i);
		source->init_distance_vector(&network);
		source->init_routing_table(&network);
		/* USED FOR DEBUGGING
		vector<Link *> links = source->get_links();
		cout << "Links at router " << i << ": \n";
		for (int j = 0; j < links.size(); j++) {
			cout << "	Link " << j << " ep1: ";  cout << ip_to_english(&network, links.at(j)->get_ep1()) << "\n";
			cout << "	Link " << j << " ep2: ";  cout << ip_to_english(&network, links.at(j)->get_ep2()) << "\n";
		}
		* */
		source->print_distance_vector();
		source->print_routing_table();

	}
	
	// Push a "Flow_Start_Event" for every flow in the network
	int num_flows = network.all_flows.size();	
	for(int i = 0; i < num_flows; i++)
	{
		Flow * this_flow = network.all_flows[i];
		double start = this_flow->get_start();
		Flow_Start_Event * event = new Flow_Start_Event(start, TCP_ID, this_flow);
		event_queue.push(event);
	}
	
	// Keep dequeuing events and handling them
	while(!event_queue.empty() || global_time >= end_time)
	{
		Event * to_handle = event_queue.top();
		event_queue.pop();
		to_handle->handle_event();
		delete to_handle;
	}
	
	printf("Exiting\n");
	return 0;
	
}





