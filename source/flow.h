#ifndef FLOW_H
#define FLOW_H

#include "link.h"
#include "packet.h"
#include "node.h"
#include <iostream>
class Node;

class Flow{
	Node * source;
	Node * destination;
	//int start;
	float size;
	
public:
	Flow(Node * source_, Node * dest_, int data_size);
	void send_stream();
	void send_data();
	void generate_packet();
	void send_packet();
};

#endif
