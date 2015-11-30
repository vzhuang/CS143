#ifndef NETWORK_H
#define NETWORK_H
#include "link.h"
#include "node.h"
#include "flow.h"
#include "mex.h"
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
