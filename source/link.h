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
#include "event.h"
#include <limits>
#include <vector>

class Event;
class Node;
class Packet;
struct CompareEvents;
extern double global_time;
extern priority_queue<Event *, vector<Event *>, CompareEvents> event_queue;

using namespace std;

class Link {
	// Maximum rate in Mbps that the link may transmit
	double capacity;
	// Total flow rate through the link (bytes/s)
	double flowrate;
	// Number of bytes sent since last update of flow rate
	int bytes_sent;
	// Time of last update of flow rate
	double update_time;
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
	// Packets currently stored in the queue
	int packets_stored;
	// Directions to send non-routing packets
	std::queue <int> directions;
	// Directions to send routing packets
	std::queue <int> directions_r;
public:
	// Queue of packets (non-routing) to be transmitted
	std::queue <Packet *> buffer;
	
	// Queue of routing packets to be transmitted
	std::queue <Packet *> buffer_r;

	// Constructor
	Link(double my_cap, Node * my_ep1, Node * my_ep2, double my_delay, double my_buf);

	// Get the link capacity
	double get_capacity();

	// Return the earliest time that the newly added packet can be popped from the buffer
	double earliest_available_time();

	// Get the flowrate 
	double get_flowrate();

	// Set the flowrate
	void set_flowrate();
	
	// Calculate the time (s) it would take to clear out everything in the queue
	double get_queue_delay();

	// Get the endpoints of the link;
	Node * get_ep1();

	Node * get_ep2();

	// Get vector of both node endpoints of the link
	vector<Node *> get_endpoints();

	// Calculate the time (s) it would take to send an individual packet
	double get_packet_delay(Packet * packet);
	
	// Add a packet to the link's buffer. Return 0 on success. -1 on fail.
	int add_to_buffer(Packet * packet, Node * source);

	// Return a link cost dependent on transmission delay, flow rate, and
	// flow direction.
	double calculate_cost();

	// Time of most recently assigned free event
	double t_free;

	//Move a packet to the other side of the link
	Packet * transmit_packet();
	
	//Connect a router/host to each side of the link. WANRING NAIVE IMPLEMENTATION
	void connect(Node * device1, Node * device2);
	
	// Set to 0 for the duration of a packet transmission. 
	// Freed to 1 during a "free" event that cooresponds to
	// The duration of a packet transmission
	bool is_free;	
};

#endif  /* LINK_H */
