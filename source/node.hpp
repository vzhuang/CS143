
#ifndef NODE_HPP
#define NODE_HPP

#include "link.hpp"
#include "packet.hpp"
#include "flow.hpp"
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
	//map<Node *, double> distance_vector;
	
public:
	map<Node *, double> distance_vector;
	// Constructor
	Node();
	// Get the address of the Node
	Node * get_ip();
	// Add a link to a node
	void add_link(Link * link_);
	// Get the first link a node has
	Link * get_first_link();
	// Get the vector of links adjacent to the Node
	vector<Link *> get_links();
	// Get the link joining the current node and a desired endpoint
	Link * get_link(Node * endpoint_);
	// Send the specified packet through the specified link
	void send_packet(Packet * packet_, Link link_);
	// Receive a packet
	void receive_packet(Packet * packet_);
	// Initialize the distance vector
	void init_distance_vector();
	// Update the distance vector when a routing packet is received
	void update_distance_vector(Rout_packet * r_packet_);
	// Print the distance vector of the node (debugging)
	void print_distance_vector();
	// Get the distance vector of the node
	map<Node *, double> get_distance_vector();
	void set_distance_vector(map<Node*, double> distance_vector_);
	// Returns true if the node is a router
	virtual bool is_router();
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
	// Initialize the routing table with knowledge of adjacent links
	void init_routing_table();
	// Update the routing table when a routing packet is received
	void update_routing_table(Rout_packet * r_packet_);
	// Get the routing table of the router
	map<Node *, Node *> get_routing_table();
	// Used for debugging
	void print_routing_table();
	// Receive a routing packet
	void receive_routing_packet(Rout_packet * r_packet_);
	// Sends distance vector to all known nodes
	void send_distance_vector();
	bool is_router();
};

#endif  /* NODE_HPP */
