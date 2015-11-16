
#ifndef NODE_H
#define NODE_H
#include "link.h"
#include "packet.h"
#include "flow.h"
#include <string>
#include <vector>
using namespace std;
class Node;
class Link;
class Packet;

struct routing_table {
	vector<Node *> source;
	vector<Node *> dest;
};


class Node {
	
	// Reference to Node address
	Node * ip;
	// List of links adjacent to Node
	vector<Link *> links;
	
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
	// Send the specified packet through the specified link
	void send_packet(Packet packet, Link link);
	// Receive a packet
	void receive_packet(Packet packet);
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
	//
	routing_table myTable;

public:
	Router();
	void build_table(routing_table table);
	void compute_routing_table();
};

#endif  /* NODE_H */
