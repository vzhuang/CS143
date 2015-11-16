#ifndef FLOW_H
#define FLOW_H

#include "link.h"
#include "packet.h"
#include "node.h"
#include "flow.h"
#include <iostream>
#include <vector>
using namespace std;

class Node;
class Link;

class Flow{
	Node * source;
	Node * destination;
	double start;
	double size;
	
public:
	Flow(Node * source_, Node * dest_, double data_size_, double start_);
	void send_stream();
	void send_data();
	void generate_packet();
	void send_packet();
};

struct Network
{
	vector<Link *> all_links;
	vector<Node *> all_hosts;
	vector<Node *> all_routers;
	vector<Flow *> all_flows;
};

#endif
