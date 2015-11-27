#include "network.h"
Network::Network()
{
	
}
Network::~Network()
{
	printf("Network cleaned up\n");
	int num_hosts = all_hosts.size();
	int num_routers = all_routers.size();
	int num_links = all_links.size();
	int num_flows = all_flows.size();
	for(int i = 0; i < num_hosts; i++)
	{
			delete all_hosts[i];
	}
	for(int i = 0; i < num_routers; i++)
	{
			delete all_routers[i];
	}
	for(int i = 0; i < num_links; i++)
	{
			delete all_links[i];
	}
	for(int i = 0; i < num_flows; i++)
	{
			delete all_flows[i];
	}
}
