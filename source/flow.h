#ifndef FLOW_H
#define FLOW_H

#include "link.h"
#include "packet.h"
#include "node.h"
#include "flow.h"
#include <iostream>
#include <vector>
class Node;
class Host;
class Link;
#define TCP_TAHOE 0
#define TCP_RENO 1
#define TCP_FAST 2
using namespace std;



class Flow{
	Host * source;
	Host * destination;
	double start;
	double size;
	
public:
	Flow(Host * source_, Host * dest_, double data_size_, double start_);
	double get_start();
	Host * get_source();
	Host * get_destination();
	void send_stream();
	void send_data();
	void generate_packet();
	void send_packet();
};

#endif
