/*
 * FILE: linked_list.h
 *
 */

#ifndef LINK_H
#define LINK_H

#include <iostream>
#include <queue>
#include "naivepacket.h"
#include <stdio.h>
#include <cstdlib>

using namespace std;

class Link {
	// Maximum rate in Mbps that the link may transmit
	int capacity;
	// Total flow rate through the link
	int flowrate;
	// Indicates one end point of the link
	void * ep1;
	// Indicates second end point of the link
	void * ep2;
	// Time in ms required to transmit packet
	int delay;
	// Number of packets that the link can store in queue
	int buffersize;
	// Queue of packets to be transmitted
	std::queue <Naive_Packet> buffer;
	// Directions to send thiose packets
	std::queue <int> directions;

public:
	// Constructor
	Link(int my_cap, int my_flow, void * my_ep1, void * my_ep2, int my_delay, int my_buf);

	// Set the flowrate
	void set_flowrate(int my_flowrate);

	// Add a packet to the link's buffer.
	void add_to_buffer(Naive_Packet packet);

	// Return a link cost dependent on transmission delay, flow rate, and
	// flow direction.
	int calculate_cost(int delay, int flowrate);

	//Move a packet to the other side of the link
	Naive_Packet transmit_packet();
	
	//Connect a router/host to each side of the link. WANRING NAIVE IMPLEMENTATION
	void connect(void * device1, void * device2);
};

#endif  /* LINK_H */
