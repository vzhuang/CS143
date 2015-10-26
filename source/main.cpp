#include <stdio.h>
#include "naivepacket.h"
#include "link.h"
#include "host.h"
#include "router.h"
#include <string>
#include <vector>
#include <cstdlib>
// Convert from an ip address (string) to a Host * object
Host * ip_to_host(string ip, vector<Host *> all_hosts)
{
	printf("Host1 ip: %s\n", all_hosts[0]->get_ip().c_str());
/*
	for(unsigned int i = 0; i < all_hosts.size(); i++)
	{
		if( ip.compare( (all_hosts[i])->get_ip() ) == 0 )
		{
			return all_hosts[i];
		}
		else
		{
			printf("ip: %s was not a match to %s\n", all_hosts[i]->get_ip().c_str(), ip.c_str());
		}
	}
	// Something went wrong
	printf("ip address: %s does not coorespond to a host. Exiting\n", ip.c_str());
*/
	exit(-1);
}

int main()
{
	// Our parser will dynamically take care of everything through line 25
	int NUM_PACKETS = 100;
	int NUM_HOSTS = 2;
	int NUM_LINKS = 1;

	printf("Creating 2 hosts\n");
	vector<Host *> all_hosts;
	all_hosts.push_back(new Host(NULL, "0.0.0.0"));
	all_hosts.push_back(new Host(NULL, "0.0.0.1"));
	printf("Host1 ip: %s\n", (all_hosts)[0]->get_ip().c_str());
	printf("Creating a link\n");
	vector<Link *> all_links;
	all_links.push_back(new Link(100, 0, NULL, NULL, 10, 100));
	
	printf("Connecting the two hosts with the link\n");
	all_links[0]->connect((void *) &all_hosts[0], (void *) &all_hosts[1]);
	///////////////////// end parser code //////////////////////////////////////////



	// Will need to be implemented in a Flow class that accepts 2 hosts
	printf("Generating Packets\n");
	vector <Naive_Packet *> packets;
	for(int i = 0; i < NUM_PACKETS; i++)
	{
		Naive_Packet * new_packet = new Naive_Packet(1, i, "0.0.0.0", "0.0.0.1");
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

	printf("Exiting\n");
	return 0;
	
}


