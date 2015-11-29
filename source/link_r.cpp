
// Link routines for the routing_buffer

#include "link.h" 
#include "parser.h"
extern Network network;
using namespace std;

// Calculate the time (s) it would take to clear out everything in the routing queue
double Link::get_queue_delay_r() {
	double total_delay = 0.0;
	// Contribution from prop delay
	total_delay += delay * packets_stored_r;
	// Contribution from the size of the data to be transmitted
	total_delay += bytes_stored_r / capacity; 
	return total_delay;
}

// Return the earliest time that the newly added packet can be popped from the routing buffer
double Link::earliest_available_time_r() {
	double proposed_time = global_time + get_queue_delay_r() - get_packet_delay(routing_buffer.front());
	if(t_free_r > proposed_time)
	{
		return t_free_r;
	}
	else
	{
		return proposed_time;
	}
}
	
/* Add a packet to the routing buffer and record its direction.
   Return 0 on success and -1 on failure. */
int Link::add_to_buffer_r(Packet * packet, Node * source) {
	
	// If the buffer is full, drop it.
	if (bytes_stored_r + packet->packetSize() > buffersize) {
		printf("Routing: Packet dropped attempting to join the buffer on link: %s\n",
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
		printf("Routing: Incoming packet did not come from a link endpoint\n");
		exit(-1);
	}
	return 0;
}

/* Transmit the first packet on this link's buffer, spawning an 
   appropriate event in response to the destination of the newly
   transmitted packet. */
Packet * Link::transmit_packet_r() {
	// Sanity check
	if(routing_buffer.empty()) {
			printf("Routing: Attempted to transmit a packet on a link with an empty buffer. Exiting. \n");
			exit(-1);
	}
	// Check if the link is free
	if(!is_free_r) {
		printf("Routing: Link %s was not free but a transmit was attempted. Retrying \n\n", 
			link_to_english(&network, this).c_str() );
		cout << t_free_r << "\n";

		// Link_Send_Routing_Event * send_event = new Link_Send_Routing_Event(
		// 									t_free_r,
		// 									RSEND_EVENT_ID,
		// 									this);
		// routing_queue.push(send_event);

		return NULL;
	}
	// Set the link to occupied while we send a packet
	else { 
		is_free_r = 0;
	}
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
									global_time + time_to_send,
									ROUT_RECEIVE_ID,
									(Rout_packet *) transmission_packet);
		routing_queue.push(rr_event);
	}
	// If endpoint 2 is not a final destination, forward it to the next router
	else {
		// Use the routing table for endpoint 2 to look up next hop
		Node * next_node = ((Router *) endpoint2)->get_routing_table().at(dest);
		// Find the link associated with the next hop and transmit the packet
		Link * next_link = ((Router *) endpoint2)->get_link(next_node);
		// Account for what's already in the link buffer
		time_to_send += next_link->get_queue_delay_r();
		// Account for bottlenecks upstream
		if(next_link->t_free_r > time_to_send + global_time)
		{
			time_to_send = next_link->t_free_r - global_time;
		}
		// Always push packet to buffer before spawning send event
		if( next_link->add_to_buffer_r(transmission_packet, endpoint2) == 0)
		{ 
			Link_Send_Routing_Event * send_event = new Link_Send_Routing_Event(
											time_to_send + global_time,
											RSEND_EVENT_ID,
											next_link);
			
			routing_queue.push(send_event);
			// Preemptively set the t_free_r on the next link to prevent directional scheduling conflicts
			next_link->t_free = time_to_send + global_time + next_link->get_packet_delay(transmission_packet);
		}
	}
	// Packet succesfully sent. Remove transmitted packet from the buffer.
	routing_buffer.pop();
	routing_directions.pop();
	bytes_stored_r -= transmission_packet->packetSize();
	packets_stored_r--;
	// Increment number of packets sent across this link
	bytes_sent += transmission_packet->packetSize();
	// Create an event to free the link at the same time that the packet
	// successfully transmits. We achieve this using epsilon.
	Link_Free_Event * free_event = 
		new Link_Free_Event(
			get_packet_delay(transmission_packet) + global_time - std::numeric_limits<double>::epsilon(),
			RFREE_EVENT_ID,
			this);
	t_free_r = get_packet_delay(transmission_packet) + global_time;
	routing_queue.push(free_event);
	return transmission_packet;
}



