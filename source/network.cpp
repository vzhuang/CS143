/**
    CS-143 Network Simulator
    network.cpp
    Purpose: Encapsulates all Link, Host, Router, and Flow obejcts
    in a network for organizational purposes.

    @author Vivian He, Jordan Bonilla, Vincent Zhuang
    @version 1.0 12/11/15
*/
#include "network.hpp"

// Network constructor
Network::Network() {
	
}

// Network destructor
Network::~Network() {
	int num_hosts = all_hosts.size();
	int num_routers = all_routers.size();
	int num_links = all_links.size();
	int num_flows = all_flows.size();
	for(int i = 0; i < num_hosts; i++) {
			delete all_hosts[i];
	}
	for(int i = 0; i < num_routers; i++) {
			delete all_routers[i];
	}
	for(int i = 0; i < num_links; i++) {
			delete all_links[i];
	}
	for(int i = 0; i < num_flows; i++) {
			delete all_flows[i];
	}
	mexPrintf("Network cleaned up\n");
}
