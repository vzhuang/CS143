#include <string>
#include <vector>
#include "node.h"

using namespace std;

Node::Node() {

}

/*
* Add a link to the vector of links adjacent to the node
*/
void Node::set_link(Link * myLink) {
	links.push_back(myLink);
}

/*
* Get the IP address of the node.
*/
Node * Node::get_ip() {
	return ip;
}

/*
* Get the links adjacent to the node.
*/
vector<Link *> Node::get_links() {
	return links;
}

/*
* Send a packet along the specified link.
*/
void Node::send_packet(Naive_Packet packet, Link link) {
	link.add_to_buffer(packet);

}

/*
* This function logs the received packets and sends an acknowledgement
* packet.
*/
void Node::receive_packet(Naive_Packet packet) {
	// TODO
}

/*
* Constructor for the host subclass
*/
Host::Host(Link * myLink) {
	links.push_back(myLink);
}

/*
* For a host, get the associated link (single)
*/
Link * Host::get_associated_link() {
	return links.front();
}

/*
* Constructor for the router subclass
*/
Router::Router() {

}

/*
* Construct a routing table using the Bellman Ford algorithm.
*/
void Router::compute_routing_table() {
	//
}

void Router::build_table (routing_table table) {
	table.source.push_back(0);
	table.source.push_back(1);
	table.dest.push_back(1);
	table.dest.push_back(0);
}

