#ifndef NETWORK_H
#define NETWORK_H
#include "link.hpp"
#include "node.hpp"
#include "flow.hpp"

using namespace std;

class Network { 
public:
	vector<Link *> all_links;
	vector<Host *> all_hosts;
	vector<Router *> all_routers;
	vector<Flow *> all_flows;
    Network();
    ~Network();

};
#endif
