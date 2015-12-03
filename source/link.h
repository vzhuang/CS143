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
#define EPSILON .0000000001

class Event;
class Node;
class Packet;
struct CompareEvents;
extern double global_time;
extern priority_queue<Event *, vector<Event *>, CompareEvents> event_queue;
extern priority_queue<Event *, vector<Event *>, CompareEvents> routing_queue;

using namespace std;

class Link {
	// Maximum rate in Mbps that the link may transmit
	double capacity;
	// Total flow rate through the link (bytes/s)
	double flowrate;
	// Number of bytes sent since last update of flow rate
	double bytes_sent;
	// Time of last update of flow rate
	double update_time;
	// Indicates one end point of the link
	Node * ep1;
	// Indicates second end point of the link
	Node * ep2;
	// Number of Bytes that the link can store in queue
	double buffersize;
	// Bytes currently stored in the data queue
	double bytes_stored;
	// Bytes curretnly stored in the routing queue
	double bytes_stored_r;
	// Packets currently stored in the data queue
	int packets_stored;
	// Packets currently stored in the routing queue
	int packets_stored_r;
public:
	// Queue of packets (non-routing) to be transmitted
	std::queue <Packet *> data_buffer;
	// Queue of routing packets to be transmitted
	std::queue <Packet *> routing_buffer;
	// Directions to send non-routing packets
	std::queue <int> data_directions;
	// Directions to send routing packets
	std::queue <int> routing_directions;
	// Constructor
	Link(double my_cap, Node * my_ep1, Node * my_ep2, double my_delay, double my_buf);
	// Time in ms required to transmit packet
	double delay;
	// Get the link capacity
	double get_capacity();
	// Return the earliest time that the newly added packet can be popped from the buffer
	double earliest_available_time();
	// Return the earliest time that the newly added packet can be popped from the routing buffer
	double earliest_available_time_r();
	// Get the flowrate 
	double get_flowrate();
	// Set the flowrate
	void set_flowrate();
	// Calculate the time (s) it would take to clear out everything in the data queue
	double get_queue_delay();
	// Calculate the time (s) it would take to clear out everything in the routing queue
	double get_queue_delay_r();
	// Get the endpoints of the link;
	Node * get_ep1();
	Node * get_ep2();
	// Get vector of both node endpoints of the link
	vector<Node *> get_endpoints();
	// Calculate the time (s) it would take to send an individual packet
	double get_packet_delay(Packet * packet);
	double get_bytes_stored();
	// Add a packet to the link's data buffer. Return 0 on success. -1 on fail.
	int add_to_buffer(Packet * packet, Node * source);
	// Add a packet to the link's routing buffer. Return 0 on success. -1 on fail.
	int add_to_buffer_r(Packet * packet, Node * source);
	// Return a link cost dependent on transmission delay, flow rate, and
	// flow direction.
	double calculate_cost();
	// Time of most recently assigned data free event
	double t_free;
	// Time of most recently assigned routing free event
	double t_free_r;
    //Discards the packet at the front of the buffer
    void discard_packet();
	//Move a data packet to the other side of the link
	Packet * transmit_packet();
	//Move a routing packet to the other side of the link
	Packet * transmit_packet_r();
	//Connect a router/host to each side of the link. WANRING NAIVE IMPLEMENTATION
	void connect(Node * device1, Node * device2);
	// Set to 0 for the duration of a packet transmission. 
	// Freed to 1 during a "free" event that cooresponds to
	// The duration of a packet transmission. Functions as a link lock. 
	bool is_free;	
	// Routing routines disregard the data bool. Need their own lock.
	bool is_free_r;	
	int packets_dropped;
	// Bytes stored that are moving in the forward/reverse directions
	int forward_bytes, reverse_bytes;
	
};

#endif  /* LINK_H */
