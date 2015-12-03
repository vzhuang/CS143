 /* Link routines for the data_buffer and link ititialization code */

#include "link.h"
#include "parser.h"
extern Network network;
using namespace std;

Link::Link(double my_cap, Node * my_ep1, Node * my_ep2, double my_delay, double my_buf) {
	capacity = my_cap;
	flowrate = 0;
	bytes_sent = 0;
	update_time = global_time;
	ep1 = my_ep1;
	ep2 = my_ep2;
	delay = my_delay;
	buffersize = my_buf;
	is_free_forward_r = 1;
	is_free_forward = 1;
	is_free_reverse_r = 1;
	is_free_reverse = 1;
	is_free = 1;
	is_free_r = 1;
	bytes_stored = 0;
	packets_stored = 0;
	t_free_forward = 0.0;
	t_free_reverse = 0.0;
	t_free = 0;
	t_free_r = 0;
	t_free_forward_r = 0.0;
	t_free_reverse_r = 0.0;
	packets_dropped = 0;
	forward_bytes = 0;
	reverse_bytes = 0;
}
// Get the capacity of the link
double Link::get_capacity() {
	return capacity;
}

// Get the flowrate of the link
double Link::get_flowrate() {
	return flowrate;
}

// Calculate the flowrate of the link
void Link::set_flowrate() {
	// Time elapsed since last update
	double time_elapsed = global_time - update_time;
	// Flow rate is bytes sent over elapsed time (s)
	flowrate = bytes_sent / time_elapsed;
	// Reset the bytes sent and most recent update time
	bytes_sent = 0;
	update_time = global_time;
}

// Calculate the time (s) it would take to clear an individual packet from the buffer
double Link::get_packet_delay(Packet * packet)
{	
	return packet->packetSize() / capacity;
}

// Calculate the time (s) it would take to clear out everything in the buffer 
double Link::get_queue_delay() {
	return bytes_stored /capacity;
}

int Link::get_packets_stored() {
	return packets_stored;
}

Node * Link::get_ep1() {
	return ep1;
}

Node * Link::get_ep2() {
	return ep2;
}

double Link::get_bytes_stored() {
	return bytes_stored;
}

vector<Node *> Link::get_endpoints() {
	vector <Node *> endpoints;
	endpoints.push_back(ep1);
	endpoints.push_back(ep2);
	return endpoints;
}

// Return the earliest time that the newly added packet can be popped from the buffer
double Link::earliest_available_time() {
	if(!is_free)
		return t_free + get_queue_delay() - get_packet_delay(data_buffer.front());
	else
		return global_time + get_queue_delay() - get_packet_delay(data_buffer.front());

}
	
	
/* Add a packet to the buffer and record its direction. Return 0 on success
   and -1 on failure. */
int Link::add_to_buffer(Packet * packet, Node * source) {	
	// If the buffer is full, drop it.
	if (bytes_stored + packet->packetSize() > buffersize) {
		mexPrintf("Packet %d dropped attempting to join the buffer on link: %s\n",
			packet->get_index(),
			link_to_english(&network, this).c_str() );
		packets_dropped++;
		return -1;
	}
	data_buffer.push(packet);
	bytes_stored += packet->packetSize();
	packets_stored += 1;		
	Node * endpoint1 = (ep1)->get_ip();
	Node * endpoint2 = (ep2)->get_ip();
	// Going from ep1 to ep2.
	if(source == endpoint1) {
		data_directions.push(1);
	}
	// Going from ep2 to ep1.
	else if(source == endpoint2) {
		data_directions.push(-1);
	}
	// Something went wrong
	else {
		mexPrintf("Incoming packet did not come from a link endpoint\n");
		exit(-1);
	}
	return 0;
}

/**
 * Discards the packet at the front of the buffer
 */
void Link::discard_packet() {
	// Sanity check
	if(data_buffer.empty()) {
			mexPrintf("Attempted to pop an empty buffer. \n");
			exit(-1);
	}	
	int direction = data_directions.front();
	data_buffer.front()->getFlow()->sending.erase(
		data_buffer.front()->getFlow()->sending.begin()); // should always be nonempty
	bytes_stored -= data_buffer.front()->packetSize();
	data_buffer.pop();
	data_directions.pop();	

	packets_stored--;
	is_free = 1;
}

/* Transmit the first packet on this link's buffer, spawning an 
   appropriate event in response to the destination of the newly
   transmitted packet. */
Packet * Link::transmit_packet() {
	// Sanity check
	if(data_buffer.empty()) {
			mexPrintf("Attempted to transmit a packet on a link with an empty buffer. Exiting. \n");
			exit(-1);
	}
	// Check if the link is free
	/*if(!is_free) {
		//mexPrintf("Link %s was not free but a transmit was attempted. Retrying \n\n", 
		//	link_to_english(&network, this).c_str() );
		//exit(-1);
		Link_Send_Event * send_event = new Link_Send_Event(
											t_free,
											SEND_EVENT_ID,
											this);
		event_queue.push(send_event);
		
		return NULL;		
	}
	// Set the link to occupied while we send a packet
	else { 
		is_free = 0;
	}*/

	// Set the link to occupied for the transmission duration (Note that we disregard case that link is not free for now.)
	if(is_free != 0)
	{
		is_free = 0;
	}

	// The packet at the front of the buffer is transmitted.
	Packet * transmission_packet = data_buffer.front();
	int direction = data_directions.front();
	// Create some local variables for clarity
	Node * dest = transmission_packet->getDest();
	Node * endpoint1 = ep1->get_ip();
	Node * endpoint2 = ep2->get_ip();
	// Determine how long it will take to transmit this packet in seconds.
	double time_to_send = get_packet_delay(transmission_packet);
	
	// Orient our endpoints to be conistent with the direction == 1 case
	if(direction == -1) {
		Node * temp = endpoint1;
		endpoint1 = endpoint2;
		endpoint2 = temp;
	}
	
	// Check if destination is the endpoint of this link
	if(endpoint2 == dest) {
		int packet_ID = transmission_packet->getId();
		// Check if this is an ack packet so that that an Ack_Receive_Event is made
		if( packet_ID == ACK_ID )
		{
			Ack_Receive_Event * ack_event = new Ack_Receive_Event(
									global_time + time_to_send + delay,
									ACK_RECEIVE_ID,
									(Ack_packet *) transmission_packet);
			event_queue.push(ack_event);
		}
		// It must be a data packet. Create a Data_Receive_Event instead
		else {
			Data_Receive_Event * receive_event = new Data_Receive_Event(
									global_time + time_to_send + delay,
									DATA_RECEIVE_ID,
									(Data_packet *) transmission_packet);
			event_queue.push(receive_event);
		}
	}
	// If endpoint 2 is not a final destination, it must be a router
	else {
		// Use the routing table for endpoint 2 to look up next hop		
		Node * next_node = ((Router *) endpoint2)->get_routing_table().at(dest);
		// Find the link associated with the next hop and transmit the packet
		Link * next_link = ((Router *) endpoint2)->get_link(next_node);

		Packet_Receive_Event * pr_event = new Packet_Receive_Event(
									global_time + time_to_send + delay,
									-1,
									(Data_packet *) transmission_packet,
									next_link,
									endpoint2);
		event_queue.push(pr_event);
		
	}
	// Packet succesfully sent. Remove transmitted packet from the buffer.
	data_buffer.pop();
	data_directions.pop();
	bytes_stored -= transmission_packet->packetSize();
	packets_stored--;

	// Increment number of packets sent across this link
	bytes_sent += transmission_packet->packetSize();
	// Create an event to free the link at the same time that the packet
	// successfully transmits. We achieve this using epsilon.
	Link_Free_Event * free_event = 
		new Link_Free_Event(
			time_to_send + global_time - EPSILON,
			LINK_FREE_ID,
			this,
			direction);

	t_free = time_to_send + global_time;

	event_queue.push(free_event);
	return transmission_packet;
}

/*
 * Calculate link cost based on static component (transmission delay) and
 * dynamic component (flow rate).
 */
double Link::calculate_cost() {

	// Time to clear queue.
	//this->set_flowrate();
	return delay + flowrate;
	
}


// Connect a link to the two specified devices
void Link::connect(Node * device1, Node * device2) {
	ep1 = device1;
	ep2 = device2;
	(ep1)->add_link(this);
	(ep2)->add_link(this);
}
