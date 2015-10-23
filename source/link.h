/*
 * FILE: linked_list.h
 *
 */

#ifndef LINK_H
#define LINK_H

#include <iostream>
#include <queue>
#include "naivepacket.h"

using namespace std;

class Link {
	// Maximum rate in Mbps that the link may transmit
	int capacity;
	// Total flow rate through the link
	int flowrate;
	// Indicates the direction of the transmission
	bool direction;
	// Time in ms required to transmit packet
	int delay;
	// Number of packets that the link can store in queue
	int buffersize;
	// Queue of packets to be transmitted
	std::queue <Naive_Packet> buffer;

public:
	// Constructor
	Link();

	// Set capacity, flow rate, transmission delay, and buffer size.
	void set_values(int, int, int, int);

	// Add a packet to the link's buffer.
	void add_to_buffer(Naive_Packet packet);

	// Return a link cost dependent on transmission delay, flow rate, and
	// flow direction.
	int calculate_cost(int delay, int flowrate, bool direction);

	Naive_Packet transmit_packet();
};

#endif  /* LINK_H */