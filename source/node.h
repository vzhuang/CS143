/*
 * FILE: node.h
 *
 */

#ifndef NODE_H
#define NODE_H

#include "link.h"
#include "naivepacket.h"
#include <string>
#include <vector>
using namespace std;

class Node {
public:
	// Reference to node address
	Node * ip;
	// List of links adjacent to node
	vector<Link *> links;

	Node();
	// Get the address of the node
	Node * get_ip();
	void set_link(Link * link);
	// Get the vector of links adjacent to the node
	vector<Link *> get_links();
	// Send the specified packet through the specified link
	void send_packet(Naive_Packet packet, Link link);
	// Receive a packet
	void receive_packet(Naive_Packet packet);
};

// Hosts
class Host : public Node {
	// Vector to record received packets
	vector<int> receivedPackets;

public:
	Host(Link * myLink);
	// Get the single link associated with a host
	Link * get_associated_link();

};

struct routing_table {
	vector<int> source;
	vector<int> dest;
};

// Routers
class Router: public Node {
	//
	routing_table myTable;

public:
	Router();
	void build_table(routing_table table);
	void compute_routing_table();
};

#endif  /* NODE_H */
