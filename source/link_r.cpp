// Link routines for the routing_buffer

#include "link.hpp" 
#include "parser.hpp"
extern Network network;
using namespace std;

// Calculate the time (s) it would take to clear out everything in the routing queue
double Link::get_queue_delay_r() {

	return bytes_stored_r / capacity;
	
}

// Return the earliest time that the newly added packet can be popped from the routing buffer
double Link::earliest_available_time_r() {
		
		return max(t_free_r - ROUTING_SIZE / capacity,
			global_time + get_queue_delay_r() - ROUTING_SIZE / capacity);
}
	
/* Add a packet to the routing buffer and record its direction.
   Return 0 on success and -1 on failure. */
int Link::add_to_buffer_r(Packet * packet, Node * source) {
	
	// If the buffer is full, drop it.
	if (bytes_stored_r + packet->packetSize() > buffersize) {
		mexPrintf("Routing: Packet dropped attempting to join the buffer on link: %s\n",
			link_to_english(&network, this).c_str() );
		return -1;
	}
	
	routing_buffer.push(packet);
	bytes_stored_r += packet->packetSize();
	packets_stored_r += 1;		
	Node * endpoint1 = (ep1)->get_ip();
	Node * endpoint2 = (ep2)->get_ip();
	// Going from ep1 to ep2.
	if(source == endpoint1) {
		routing_directions.push(1);
	}
	// Going from ep2 to ep1.
	else if(source == endpoint2) {
		routing_directions.push(-1);
	}
	// Something went wrong
	else {
		mexPrintf("Routing: Incoming packet did not come from a link endpoint\n");
		mexErrMsgTxt("");
	}
	t_free_r = max(t_free_r + ROUTING_SIZE / capacity, global_time + get_queue_delay_r());
	return 0;
	
}

/* Transmit the first packet on this link's buffer, spawning an 
   appropriate event in response to the destination of the newly
   transmitted packet. */
Packet * Link::transmit_packet_r() {
	// Sanity check
	if(routing_buffer.empty()) {
			mexPrintf("Routing: Attempted to transmit a packet on a link with an empty buffer. Exiting. \n");
			mexErrMsgTxt("");
	}	
	if(is_free_r == false) {
			mexPrintf("Routing: Failure. Link was not free but a transmit was attempted. Global Time: %f, t_free_r: %f\nExiting. \n",
			global_time * 1000.0, t_free_r * 1000.0);
			mexErrMsgTxt("");
	}

	if(global_time > t_free_r)
	{
			mexPrintf("Routing: Failure. Global Time: %f, t_free_r: %f\nExiting. \n",
			global_time * 1000.0, t_free_r * 1000.0);
			mexErrMsgTxt("");
	}

	// Set the link to occupied for the duration of this transmission
	is_free_r = false;
	
	// The packet at the front of the buffer is transmitted.
	Packet * transmission_packet = routing_buffer.front();
	int direction = routing_directions.front();
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
		// If yes, have the destination router receive the packet
		Rout_Receive_Event * rr_event = new Rout_Receive_Event(
									(Router *) endpoint2,
									global_time + time_to_send + delay,
									(Rout_packet *) transmission_packet);
		routing_queue.push(rr_event);
	}
	// If endpoint 2 is not a final destination, forward it to the next router
	else {
		// Use the routing table for endpoint 2 to look up next hop		
		Node * next_node = ((Router *) endpoint2)->get_routing_table().at(dest);
		// Find the link associated with the next hop and transmit the packet
		Link * next_link = ((Router *) endpoint2)->get_link(next_node);

		Packet_Receive_Event * pr_event = new Packet_Receive_Event(
									global_time + time_to_send + delay,
									RSEND_EVENT_ID,
									transmission_packet,
									next_link,
									endpoint2);
		routing_queue.push(pr_event);
	}
	// Packet succesfully sent. Remove transmitted packet from the buffer.
	routing_buffer.pop();
	routing_directions.pop();
	bytes_stored_r -= transmission_packet->packetSize();
	packets_stored_r--;

	return transmission_packet;
}
