#include "link.h"

using namespace std;

Link::Link(double my_cap, Node * my_ep1, Node * my_ep2, double my_delay, double my_buf) {
	capacity = my_cap;
	flowrate = 0;
	packets_sent = 0;
	update_time = global_time;
	ep1 = my_ep1;
	ep2 = my_ep2;
	delay = my_delay;
	buffersize = my_buf;
	is_free = 1;
	bytes_stored = 0;
	packets_stored = 0;
}

// Calculate the flowrate of the link
void Link::set_flowrate() {

	// Time elapsed since last update
	double time_elapsed = global_time - update_time;

	// Flow rate is number of packets sent over elapsed time
	flowrate = packets_sent / time_elapsed;

	// Reset the packets sent and most recent update time
	packets_sent = 0;
	update_time = global_time;
}

// Calculate the time (s) it would take to send an individual packet
double Link::get_packet_delay(Packet * packet)
{	
	// Account for propagation delay
	double total_delay = delay;
	// Account for size of packet
	total_delay += packet->packetSize() / capacity;
	//printf("packet delay: %f\n", total_delay);
	return total_delay;
}

// Calculate the time (s) it would take to clear out everything in the queue
double Link::get_queue_delay() {
	double total_delay = 0.0;
	// Contribution from prop delay
	total_delay += delay * packets_stored;
	// Contribution from the size of the data to be transmitted
	total_delay += bytes_stored / capacity; 
	//printf("queue delay: %f\n", total_delay);
	return total_delay;
}

/* Add a packet to the buffer and record its direction. Return 0 on success
   and -1 on failure. */
int Link::add_to_buffer(Packet * packet) {
	// If the buffer is full, drop it.
	if (bytes_stored + packet->packetSize() > buffersize) {
		printf("Packet dropped attempting to join the buffer on link: %lu\n", (long unsigned int)this);
		return -1;
	}
	
	printf("Packet added to the buffer on link: %lu at time: %f\n", (long unsigned int)this, global_time);
	buffer.push(packet);
	bytes_stored += packet->packetSize();
	packets_stored += 1;
		
	Node * source = packet->getSource();
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
	return 0;
}

Packet * Link::transmit_packet() {
	// Sanity check
	if(buffer.empty())
	{
			printf("Attempted to transmit a packet on a link with an empty buffer. Exiting. \n");
			exit(-1);
	}
	// Check if the link is free
	if(!is_free)
	{
		printf("Link %lu was not free but a push was attempted\n", (long unsigned int)this);
		return NULL;
	}
	// The packet at the front of the buffer is transmitted.
	Packet * transmission_packet = buffer.front();
	int direction = directions.front();
	// Increment number of packets sent
	packets_sent += 1;
	// Dequeue transmitted packet from the buffer.
	buffer.pop();
	directions.pop();
	bytes_stored -= transmission_packet->packetSize();
	packets_stored -= 1;
	// Create some local variables for clarity
	Node * dest = transmission_packet->getDest();
	Node * endpoint1 = ep1->get_ip();
	Node * endpoint2 = ep2->get_ip();
	// Determine how long it will take to transmit this packet in seconds.
	double time_to_send = get_packet_delay(transmission_packet);
	// Account for what's already in the link buffer
	time_to_send += this->get_queue_delay();
	// Designate the link as occupied
	is_free = 0;
	/* Create an event to free the link at the same time that the packet
	   successfully transmits. We achieve this using epsilon. */
	Link_Free_Event * event = new Link_Free_Event(
								time_to_send + global_time - numeric_limits<double>::epsilon(),
								LINK_FREE_ID,
								this);
	event_queue.push(event);
	
	if(direction == 1) // Packet is going from ep1 to ep2
	{	
		// Check if destination is the endpoint
		if(dest == endpoint2)
		{
			int packet_ID = transmission_packet->getId();
			// Check if this is an ack packet so that that an Ack_Receive_Event is made
			if( packet_ID == ACK_ID)
			{
				Ack_Receive_Event * ack_event = 
							new Ack_Receive_Event(time_to_send + global_time,
									ACK_RECEIVE_ID,
									(Ack_packet *) transmission_packet);
				event_queue.push(ack_event);
			}
			
			// It must be a data packet. Create a Data_Receive_Event instead
			else if( packet_ID == DATA_ID)
			{

				Data_Receive_Event * receive_event = 
							new Data_Receive_Event(time_to_send + global_time,
									DATA_RECEIVE_ID,
									(Data_packet *) transmission_packet);
				event_queue.push(receive_event);
			}
			// Sanity check
			else
			{
				printf("Unexpected packet ID: %d reached its endpoint\n", packet_ID);
				exit(-1);
			}

		}
		// Destination must be a router since it isn't a host
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
			int packet_ID = transmission_packet->getId();
			// Check if this is an ack packet so that that an Ack_Receive_Event is made
			if( packet_ID == ACK_ID)
			{
				Ack_Receive_Event * ack_event = 
							new Ack_Receive_Event(time_to_send + global_time,
									ACK_RECEIVE_ID,
									(Ack_packet *) transmission_packet);
				event_queue.push(ack_event);
			}
			
			// It must be a data packet. Create a Data_Receive_Event instead
			else if( packet_ID == DATA_ID)
			{

				Data_Receive_Event * receive_event = 
							new Data_Receive_Event(time_to_send + global_time,
									DATA_RECEIVE_ID,
									(Data_packet *) transmission_packet);
				event_queue.push(receive_event);
			}
			// Sanity check
			else
			{
				printf("Unexpected packet ID: %d reached its endpoint\n", packet_ID);
				exit(-1);
			}
	}
		// Destination must be a router since it isn't a host
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
void Link::connect(Node * device1, Node * device2) {
	ep1 = device1;
	ep2 = device2;
	(ep1)->add_link(this);
	(ep2)->add_link(this);
}
