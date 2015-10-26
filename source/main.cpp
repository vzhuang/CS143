#include <stdio.h>
#include "naivepacket.h"
#include "link.h"
#include "host.h"
#include "router.h"
#include <string>
#include <vector>
int main()
{
	printf("Creating 2 hosts\n");
	vector<Host *> all_hosts;
	all_hosts.push_back(new Host(NULL, "0.0.0.0"));
	all_hosts.push_back(new Host(NULL, "0.0.0.1"));

	printf("Creating a link to connect the 2 hosts\n");
	vector<Link *> all_links;
	all_links.push_back(new Link(100, 0, (void*) &all_hosts[0], (void *) &all_hosts[1], 10, 100));

	printf("Generating Packets\n");
	vector <Naive_Packet *> packets;
	for(int i = 0; i < 100; i++)
	{
		Naive_Packet * new_packet = new Naive_Packet(1, i, "0.0.0.0", "0.0.0.1");
		packets.push_back(new_packet);
	}

	printf("Sending packets:\n");








	for(int i = 0; i < 100; i++)
	{
		//Host * starting_host = find_host 
	}

	printf("Cleaning up packets\n");
	for(int i = 0; i < 100; i++)
	{
		delete packets[i];
	}


	printf("Exiting\n");
	return 0;
	
}
