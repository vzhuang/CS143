#include "node.h"
#include "packet.h"
#include "flow.h"
#include "link.h"
#include "parser.h"

double global_time;
double end_time;
priority_queue<double> event_queue;

int main(int argc, char *argv[]) {
	if(argc != 3)
	{
		printf("./proj [network.txt] [double simulation_time (s)]\n");
		exit(-1);
	}
	char * network_file = argv[1];
	end_time = stod(argv[2]);
	Network network;
	build_network(&network, network_file);
	
	
	// Clean up. TODO: Put in a class destructor
	int num_hosts = network.all_hosts.size();
	int num_routers = network.all_routers.size();
	int num_links = network.all_links.size();
	int num_flows = network.all_flows.size();
	
	for(int i = 0; i < num_hosts; i++)
	{
			delete network.all_hosts[i];
	}
	for(int i = 0; i < num_routers; i++)
	{
			delete network.all_routers[i];
	}
	for(int i = 0; i < num_links; i++)
	{
			delete network.all_links[i];
	}
	for(int i = 0; i < num_flows; i++)
	{
			delete network.all_flows[i];
	}
	printf("Exiting\n");
	return 0;
	
}
	/*
	// Our parser will dynamically take care of everything through line 25
	int NUM_PACKETS = 10;
	int NUM_HOSTS = 2;
	int NUM_LINKS = 1;

	printf("Creating 2 hosts\n");
	vector<Node *> all_hosts;
	all_hosts.push_back(new Host(NULL, &this));
	all_hosts.push_back(new Host(NULL, &this));

	printf("Creating a link\n");
	vector<Link *> all_links;
	all_links.push_back(new Link(100, 0, NULL, NULL, 10, 100));
	
	printf("Connecting the two hosts with the link\n");
	all_links[0]->connect((void *) all_hosts[0], (void *) all_hosts[1]);
	///////////////////// end parser code //////////////////////////////////////////



	// Will need to be implemented in a Flow class that accepts 2 hosts
	printf("Generating Packets\n");
	vector <Packet *> packets;
	for(int i = 0; i < NUM_PACKETS; i++)
	{
		Packet * new_packet = new Packet(1, i, "0.0.0.0", "0.0.0.1");
		packets.push_back(new_packet);
	}

	string source_host = packets[0]->get_source();
	Link * initial_link = ip_to_host(source_host, all_hosts)->get_link();
	printf("Sending packets from host %s\n", source_host.c_str());
	for(int i = 0; i < packets.size(); i++)
	{
		initial_link->add_to_buffer(*packets[i]);
	}
	for(int i = 0; i < packets.size(); i++)
	{
		initial_link->transmit_packet();
	}
	///////////////////////// end naive flow implementation /////////////////////


	// Clean up
	printf("Cleaning up packets\n");
	for(int i = 0; i < NUM_PACKETS; i++)
	{
		delete packets[i];
	}

	printf("Cleaning up hosts\n");
	for(int i = 0; i < NUM_HOSTS; i++)
	{
		delete all_hosts[i];
	}

	printf("Cleaning up links\n");
	for(int i = 0; i < NUM_LINKS; i++)
	{
		delete all_links[i];
	}
*/





