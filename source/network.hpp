/* NETWORK_HPP */

#ifndef NETWORK_HPP
#define NETWORK_HPP
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
#endif /* NETWORK_HPP */
