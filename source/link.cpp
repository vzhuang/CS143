#include "link.h"

using namespace std;

Link::Link(double my_cap, Node * my_ep1, Node * my_ep2, double my_delay, double my_buf) {
	capacity = my_cap;
	flowrate = 0;
	ep1 = my_ep1;
	ep2 = my_ep2;
	delay = my_delay;
	buffersize = my_buf;
	is_free = 1;
	bytes_stored = 0;
}

// Change the flowrate of the link
void Link::set_flowrate(double my_flowrate) {
	flowrate = my_flowrate;
}

//Add a packet to the buffer and record its direction
void Link::add_to_buffer(Packet packet) {
	// If the buffer is not full, enqueue the incoming packet. Else, drop it.
	if (bytes_stored + packet.packetSize() <= buffersize) {
		buffer.push(packet);
		bytes_stored += packet.packetSize();
	} 
        // WARNING: ASSUMES A HOST IS CONNECTED. NAIVE IMPLEMENTATION. Make robust with dynamic casting. 
	Node * source = packet.getSource();
	Node * endpoint1 = (ep1)->get_ip();
	Node * endpoint2 = (ep2)->get_ip();
	// Going from ep1 to ep2.
	if(source == endpoint1)
	{
		directions.push(1);
	}
	// Going from ep2 to ep1.
	else if(source == endpoint2)
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

Packet Link::transmit_packet() {
	// Sanity check
	if(buffer.empty())
	{
			printf("Attempted to transmit a packet on a link with an empty buffer. Exiting. \n");
			exit(-1);
	}
	// The packet at the front of the buffer is transmitted.
	Packet transmission_packet = buffer.front();
	int direction = directions.front();
	// Dequeue transmitted packet from the buffer.
	buffer.pop();
	directions.pop();
	bytes_stored -= transmission_packet.packetSize();
	printf("Packet Dequeued from buffer.\n");
	// See which way to transmit
        // WARNING: ASSUMES A HOST IS CONNECTED. NAIVE IMPLEMENTATION. Make robust with dynamic casting. 
	Node * dest = transmission_packet.getDest();
	Node * endpoint1 = ep1->get_ip();
	Node * endpoint2 = ep2->get_ip();
	if(direction == 1) // Packet is going from ep1 to ep2
	{
		// Check if destination is the endpoint
		if(dest == endpoint2)
		{
			printf("Packet #%lu recieved at host: %lu\n", (long unsigned int)transmission_packet.getDest(), (long unsigned int)endpoint2);
			/* Indicate in endpoint2's vector that it has recieved a packet */
		}
		// Check if the destination is a router
		else
		{
			// Use routing table to determine proper link to load next
			// Node * dest = endpoint2.get_routing_table.lookup(endpoint1)
			// link * next_link = dest.get_link()
			// next_link.add_to_buffer(transmission_packet);
			// Create_event(next_link, get_transmit_time(next_link));
		}
	}
	else // Packet is going from ep2 to ep1
	{
		// Check if destination is the endpoint
		if(dest == endpoint1)
		{
			printf("Packet #%lu recieved at host: %lu", (long unsigned int)transmission_packet.getDest(), (long unsigned int)endpoint1);
		}
		// Check if the destination is a router
		else
		{
			// Use routing table to determine proper link to load next
			// Node * dest = endpoint2.get_routing_table.lookup(endpoint1)
			// link * next_link = dest.get_link()
			// next_link.add_to_buffer(transmission_packet);
			// Create_event(next_link, get_transmit_time(next_link));
		}
	}
	return transmission_packet;
}

/*
 * Calculate link cost based on static component (transmission delay) and
 * dynamic component (flow rate).
 */
double Link::calculate_cost(double delay, double flowrate) {
	double cost = delay + flowrate;
	return cost;
}


// Connect a link to the two specified devices
// WARNING THIS IS A NAIVE IMPLMENTATION. VIRTUAL FUNCTIONS WILL BE REQUIRED WHEN ROUTERS ARE USED.
void Link::connect(Node * device1, Node * device2) {
	ep1 = device1;
	ep2 = device2;
	(ep1)->add_link(this);
	(ep2)->add_link(this);
}
