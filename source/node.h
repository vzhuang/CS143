
#ifndef NODE_H
#define NODE_H

#include "link.h"
#include "packet.h"
#include "flow.h"
#include <string>
#include <vector>
#include <map>

using namespace std;
class Node;
class Link;
class Flow;
class Packet;
class Rout_packet;
class Network;

class Node {
	
	// Reference to Node address
	Node * ip;
	// List of links adjacent to Node
	vector<Link *> links;
	// Distance vector
	map<Node *, double> distance_vector;
	
public:
	// Constructor
	Node();
	// Get the address of the Node
	Node * get_ip();
	// Add a link to a node
	void add_link(Link * link);
	// Get the first link a node has
	Link * get_first_link();
	// Get the vector of links adjacent to the Node
	vector<Link *> get_links();
	// Get the link joining the current node and a desired endpoint
	Link * get_link(Node * endpoint);
	// Send the specified packet through the specified link
	void send_packet(Packet * packet, Link link);
	// Receive a packet
	void receive_packet(Packet * packet);
	// Compute the distance to all adjacent nodes
	void init_distance_vector(Network * network);
	// Initialize the distance vector
	void init_distance_vector();
	// Update the distance vector when a routing packet is received
	void update_distance_vector(Rout_packet * rpacket);
	// Print the distance vector of the node (debugging)
	void print_distance_vector();
	// Get the distance vector of the node
	map<Node *, double> get_distance_vector();
	void set_distance_vector(map<Node*, double> distances);
};

// Hosts
class Host : public Node {
	// Vector to record received packets
	vector<int> receivedPackets;

public:
	Host();

};

// Routers
class Router: public Node {
	// Routing table
	map<Node *, Node *> routing_table;

public:
	Router();
	// Initialize the routing table with knowledge of the network
	void init_routing_table(Network * network);
	// Initialize the routing table with knowledge of adjacent links
	void init_routing_table();
	// Update the routing table when a routing packet is received
	void update_routing_table(Rout_packet * rpacket);
	// Get the routing table of the router
	map<Node *, Node *> get_routing_table();
	void print_routing_table();
};

#endif  /* NODE_H */
