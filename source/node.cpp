#include "node.h"

using namespace std;

Node::Node() {
	ip = this;
}

/*
* Add a link to the vector of links adjacent to the Node
*/
void Node::add_link(Link * myLink) {
	links.push_back(myLink);
}

/*
* Get the IP address of the Node.
*/
Node * Node::get_ip() {
	return ip;
}

/*
* Get the first link on a node (Used to get a Host's single link)
*/
Link * Node::get_first_link()
{
	return links[0];
}
	
/*
* Get the links adjacent to the Node.
*/
vector<Link *> Node::get_links() {
	return links;
}

/*
* Send a packet along the specified link.
*/
void Node::send_packet(Packet packet, Link link) {
	link.add_to_buffer(packet);

}

/*
* This function logs the received packets and sends an acknowledgement
* packet.
*/
void Node::receive_packet(Packet packet) {
	// TODO
}

/*
* Constructor for the host subclass
*/
Host::Host()
           : Node()
{

}

/*
* Constructor for the router subclass
*/
Router::Router() 
           : Node()
{

}

/*
* Construct a routing table using the Bellman Ford algorithm.
*/
void Router::compute_routing_table() {
	//
}

void Router::build_table (routing_table table) {

}

