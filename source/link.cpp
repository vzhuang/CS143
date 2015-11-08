#include <iostream>
#include "link.h"
#include "host.h"
#include <queue>
#include <stdio.h>
#include <cstdlib>

using namespace std;

Link::Link(int my_cap, int my_flow, void * my_ep1, void * my_ep2, int my_delay, int my_buf) {
	capacity = my_cap;
	flowrate = my_flow;
	ep1 = my_ep1;
	ep2 = my_ep2;
	delay = my_delay;
	buffersize = my_buf;
}

// Change the flowrate of the link
void Link::set_flowrate(int my_flowrate) {
	flowrate = my_flowrate;
}

//Add a packet to the buffer and record its direction
void Link::add_to_buffer(Naive_Packet packet) {
	// If the buffer is not full, enqueue the incoming packet. Else, drop it.
	if (buffer.size() < buffersize) {
		buffer.push(packet);
	} 
        // WARNING: ASSUMES A HOST IS CONNECTED. NAIVE IMPLEMENTATION. Make robust with dynamic casting. 
	string source = packet.get_source();
	string endpoint1 = ((Host *)ep1)->get_ip();
	string endpoint2 = ((Host *)ep2)->get_ip();
	// Going from ep1 to ep2.
	if(source.compare(endpoint1) == 0)
	{
		directions.push(1);
	}
	// Going from ep2 to ep1.
	else if(source.compare(endpoint2) == 0)
	{
		directions.push(-1);
	}
	// Something went wrong
	else
	{
		printf("Incoming packet did not come from a link endpoint\n");
		exit(-1);
	}
}

Naive_Packet Link::transmit_packet() {
	// The packet at the front of the buffer is transmitted.
	Naive_Packet transmission_packet = buffer.front();
	int direction = directions.front();
	// Dequeue transmitted packet from the buffer.
	buffer.pop();
	directions.pop();
	printf("Packet Dequeued from buffer.\n");
	// See which way to transmit
        // WARNING: ASSUMES A HOST IS CONNECTED. NAIVE IMPLEMENTATION. Make robust with dynamic casting. 
	string dest = transmission_packet.get_dest();
	string endpoint1 = ((Host *)ep1)->get_ip();
	string endpoint2 = ((Host *)ep2)->get_ip();
	if(direction == 1) // Packet is going from ep1 to ep2
	{
		// Check if destination is the endpoint
		if(dest.compare(endpoint2) == 0)
		{
			printf("Packet #%d recieved at host: %s\n", transmission_packet.get_index(), endpoint2.c_str());
		}
		// Check if the destination is a router
		else
		{
			// Use routing table to determine proper link to load next
		}
	}
	else // Packet is going from ep2 to ep1
	{
		// Check if destination is the endpoint
		if(dest.compare(endpoint1) == 0)
		{
			printf("Packet #%d recieved at host: %s", transmission_packet.get_index(), endpoint1.c_str());
		}
		// Check if the destination is a router
		else
		{
			// Use routing table to determine proper link to load next
		}
	}
	return transmission_packet;
}

/*
 * Calculate link cost based on static component (transmission delay) and
 * dynamic component (flow rate).
 */
int Link::calculate_cost(int delay, int flowrate) {
	int cost = delay + flowrate;
	return cost;
}


// Connect a link to the two specified devices
// WARNING THIS IS A NAIVE IMPLMENTATION. VIRTUAL FUNCTIONS WILL BE REQUIRED WHEN ROUTERS ARE USED.
void Link::connect(void * device1, void * device2) {
	ep1 = device1;
	ep2 = device2;
	((Host *)ep1)->set_link(this);
	((Host *)ep2)->set_link(this);
}
