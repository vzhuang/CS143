#include "node.hpp"
#include "parser.hpp"
#include "network.hpp"

extern Network network;
extern priority_queue<Event *, vector<Event *>, CompareEvents> routing_queue;

using namespace std;

Node::Node() {
	ip = this;
	// Distance from node to istelf is 0
	distance_vector[ip] = 0.0;
}

/*
* Add a link to the vector of links adjacent to the Node
*/
void Node::add_link(Link * link_) {
	links.push_back(link_);
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
* Get the link connecting the current node to the desired endpoint
*/
Link * Node::get_link(Node * endpoint_) {
	vector<Link *> links = this->get_links();
	for (int i = 0; i < links.size(); i++) 
	{
		Link * link = links.at(i);
		if (link->get_ep1() == endpoint_ || link->get_ep2() == endpoint_)
		{
			return link;
		}
	}
	
	mexPrintf("FATAL: Router could not find link to requested endpoint: %s\n",
		ip_to_english(&network, endpoint_).c_str() );
	mexErrMsgTxt("");
}

/**
* Initialize the node's distance vector to adjacent links
*/
void Node::init_distance_vector() {
	// Clear the previous distance vector
	distance_vector.clear();
	// Distance to self is 0
	distance_vector[ip] = 0.0;
	vector <Link *> adj_links = this->get_links();
	// Node only knows it can reach nodes through its links.
	for (int i = 0; i < adj_links.size(); i++) {
		Link * link = adj_links.at(i);
		Node * ep1 = link->get_ep1();
		Node * ep2 = link->get_ep2();
		// Current node is one of the endpoints. Add the other to the distance vector.
		if (ip == ep1) {
			distance_vector[ep2] = link->calculate_cost();
		}
		else {
			distance_vector[ep1] = link->calculate_cost();
		}
	}
}


map<Node *, double> Node::get_distance_vector() {
	return distance_vector;
}

// USED FOR DEBUGGING
void Node::print_distance_vector() {
	
	cout << "Distance vector for source: " << ip_to_english(&network, this) << "\n";
	for (map<Node *, double>::iterator it=distance_vector.begin(); it!=distance_vector.end(); ++it) {
		//
		cout <<"	"<< ip_to_english(&network, it->first) << " " << it->second << "\n";
	}
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


/**
* Initialize the routing table without external knowledge of the network.
*/ 
void Router::init_routing_table() {
	routing_table.clear();
	vector<Link *> adj_links = this->get_links();
	// Initially the router only knows about adjacent nodes.
	for (int i = 0; i < adj_links.size(); i++) {
		Link * link = adj_links.at(i);
		Node * ep1 = link->get_ep1();
		Node * ep2 = link->get_ep2();
		// Current node is one of the endpoints. Add the other to the routing table.
		if (this == ep1) {
			routing_table[ep2] = ep2;
		}
		else {
			routing_table[ep1] = ep1;
		}

	}
}

/*
* Update a routing table dynamically when a routing packet is received.
*/
void Router::update_routing_table(Rout_packet * r_packet_) {
	Rout_packet * r_packet = r_packet_;
	// Router and corresponding distance vector from packet
	Node * rsrc = r_packet->getSource();
	map<Node *, double> packet_vector = r_packet->get_packet_vector();

	// Distance from current node to router that sent packet
	double dist_to_rsrc = distance_vector.at(rsrc);
	// Iterate over nodes in the packet vector
	for (map<Node *, double>::iterator it = packet_vector.begin(); it != packet_vector.end(); it++) {
		// Destination reachable from rnode and its distance
		Node * dst = it->first;
		double dist_from_rsrc = it->second;
		if (dst == this) {
			// If the destination is the current router, do not add to routing table.
		}
		// If dest node is not in the routing table, add it
		else if (routing_table.find(dst) == routing_table.end()) {
			distance_vector[dst] = dist_to_rsrc + dist_from_rsrc;
			// Add the node to the routing table. Since the shortest path 
			// goes through rsrc
			routing_table[dst] = routing_table.at(rsrc);

		}
		else {
			// If a shorter path has been found, update distance vector
			double prev_dist = distance_vector.at(dst);
			if (dist_to_rsrc + dist_from_rsrc < prev_dist) {
 				distance_vector[dst] = dist_to_rsrc + dist_from_rsrc;
				routing_table[dst] = routing_table[rsrc];
			}
			// If a path of equal length has been found, update next hop to 
			// the lower-indexed of the two routers
			else if (dist_to_rsrc + dist_from_rsrc == prev_dist) {
				if (ip_to_english(&network, routing_table.at(rsrc)) < ip_to_english(&network, routing_table.at(dst))) {
					routing_table[dst] = routing_table.at(rsrc);
				}
			}
		}

	}
}


map<Node *, Node *> Router::get_routing_table() {
	return routing_table;
}

//USED FOR DEBUGGING: Prints the contents of the routing table
void Router::print_routing_table() {
	map<Node *, Node *> rtable = this->get_routing_table();
	cout << "Routing table for source: " << ip_to_english(&network, this) << "\n";
	for (map<Node *, Node *>::iterator it=rtable.begin(); it!=rtable.end(); ++it) {
		cout <<"	"<<  ip_to_english(&network, it->first) << " " << ip_to_english(&network, it->second) << "\n";
	}
}

/**
* Router receives a routing packet and updates the distance vector and routing
* table.
*/
void Router::receive_routing_packet(Rout_packet * r_packet_) {
	Rout_packet * r_packet = r_packet_;
	// Update the distance vector and routing table
	this->update_routing_table(r_packet);
}

/**
* Used for routing: router sends its distance vector to all known nodes.
*/
void Router::send_distance_vector() {
	// For every node in the routing table, send the distance vector
	for (map<Node *, Node *>::iterator it=routing_table.begin(); it!=routing_table.end(); ++it) {
		Node * dest = it->first;
		Node * next_node = it->second;
		map<Node *, double> d_vector = this->get_distance_vector();
		// Create a routing packet with the distance vector
		Rout_packet r_packet = Rout_packet(this, dest, d_vector);
		// Find the link associated with the next hop and transmit the packet
		Link * next_link = this->get_link(next_node);
	}
}

/**
* Virtual function to check if a node is a router.
*/
bool Node::is_router() {
	return false;
}

/**
* Returns true. Overwrites Node::is_router() if the node is a router.
*/
bool Router::is_router() {
	return true;
}

