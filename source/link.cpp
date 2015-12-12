/**
    CS-143 Network Simulator
    link.cpp
    Purpose: Contains link routines that are used when a flow transmits
    data and ack packets between 2 hosts. These routines use the 
    data buffer of the link. They deal only with with data and ack 
    packets. 

    @author Jordan Bonilla, Vivian He
    @version 1.0 12/11/15
*/
#include "link.hpp"
#include "parser.hpp"
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
	packets_stored_r = 0;
	t_free_forward = 0.0;
	t_free_reverse = 0.0;
	t_free = 0;
	t_free_r = 0;
	t_free_forward_r = 0.0;
	t_free_reverse_r = 0.0;
	packets_dropped = 0;
	forward_bytes = 0;
	reverse_bytes = 0;
	newly_added = NULL;
}
// Get the capacity of the link
double Link::get_capacity() {
	return capacity;
}

// Get the flowrate of the link
double Link::get_flowrate() {
    set_flowrate();
	return flowrate;
}
int Link::get_packets_dropped() {
    int pd = packets_dropped;
    packets_dropped = 0;
    return pd;
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
		return max(t_free - get_packet_delay(newly_added),
			global_time + get_queue_delay() - get_packet_delay(newly_added));
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
	newly_added = packet;
	bytes_stored += packet->packetSize();
    if(packet->getId() == DATA_ID)
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
		mexErrMsgTxt("Incoming packet did not come from a link endpoint\n");
	}
	t_free = max(t_free + get_packet_delay(packet), global_time + get_queue_delay());
	return 0;
}

/* Transmit the first packet on this link's buffer, spawning an 
   appropriate event in response to the destination of the newly
   transmitted packet. */
Packet * Link::transmit_packet() {
	// Sanity check
	if(data_buffer.empty()) {
			mexPrintf("Failure. Attempted to transmit a packet on a link with an empty buffer. Global Time: %f, t_free: %f\n",
			global_time * 1000.0, t_free * 1000.0);
			mexErrMsgTxt("Exiting. \n");
	}

	if(is_free == false) {
			mexPrintf("Failure. Link was not free but a transmit was attempted. Global Time: %f, t_free: %f\n",
			global_time * 1000.0, t_free * 1000.0);
			mexErrMsgTxt("Exiting. \n");
	}

	if(global_time > t_free)
	{
			mexPrintf("Failure. Global Time: %f, t_free: %f\n",
			global_time * 1000.0, t_free * 1000.0);
			mexErrMsgTxt("Exiting. \n");
	}
	// Set the link to occupied for the duration of this transmission
	is_free = false;

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
									(Ack_packet *) transmission_packet);
			event_queue.push(ack_event);
		}
		// It must be a data packet. Create a Data_Receive_Event instead
		else {
			Data_Receive_Event * receive_event = new Data_Receive_Event(
									global_time + time_to_send + delay,
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
									SEND_EVENT_ID,
									transmission_packet,
									next_link,
									endpoint2);
		event_queue.push(pr_event);
		
	}
	// Packet succesfully sent. Remove transmitted packet from the buffer.
	data_buffer.pop();
	data_directions.pop();
	bytes_stored -= transmission_packet->packetSize();
	if(transmission_packet->getId() == DATA_ID)
        packets_stored--;

	// Increment number of packets sent across this link
	bytes_sent += transmission_packet->packetSize();
	return transmission_packet;
}

/*
 * Calculate link cost based on static component (transmission delay) and
 * dynamic component (flow rate).
 */
double Link::calculate_cost() {

	// Time to clear queue.
	return delay + flowrate;
	
}


// Connect a link to the two specified devices
void Link::connect(Node * device1, Node * device2) {
	ep1 = device1;
	ep2 = device2;
	(ep1)->add_link(this);
	(ep2)->add_link(this);
}
