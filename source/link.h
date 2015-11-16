/*
 * FILE: linked_list.h
 *
 */

#ifndef LINK_H
#define LINK_H

#include "flow.h"
#include <iostream>
#include <queue>
#include "packet.h"
#include <stdio.h>
#include <cstdlib>
#include "node.h"
class Node;
class Packet;

using namespace std;

class Link {
	// Set to 0 for the duration of a packet transmission. 
	// Freed to 1 during a "free" event that cooresponds to
	// The duration of a packet transmission
	bool is_free;	
	// Maximum rate in Mbps that the link may transmit
	double capacity;
	// Total flow rate through the link
	double flowrate;
	// Indicates one end point of the link
	Node * ep1;
	// Indicates second end point of the link
	Node * ep2;
	// Time in ms required to transmit packet
	double delay;
	// Number of Bytes that the link can store in queue
	double buffersize;
	// Bytes curretnly stored in the queue
	double bytes_stored;
	// Queue of packets to be transmitted
	std::queue <Packet> buffer;
	// Directions to send thiose packets
	std::queue <int> directions;

public:
	// Constructor
	Link(double my_cap, Node * my_ep1, Node * my_ep2, double my_delay, double my_buf);

	// Set the flowrate
	void set_flowrate(double my_flowrate);

	// Add a packet to the link's buffer.
	void add_to_buffer(Packet packet);

	// Return a link cost dependent on transmission delay, flow rate, and
	// flow direction.
	double calculate_cost(double delay, double flowrate);

	//Move a packet to the other side of the link
	Packet transmit_packet();
	
	//Connect a router/host to each side of the link. WANRING NAIVE IMPLEMENTATION
	void connect(Node * device1, Node * device2);
};

#endif  /* LINK_H */
