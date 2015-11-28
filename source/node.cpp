#include "node.h"
#include <map>
#include <vector>
#include "parser.h"
#include "network.h"
extern Network network;
extern priority_queue<Event *, vector<Event *>, CompareEvents> routing_queue;

using namespace std;

Node::Node() {
	ip = this;
	// Distance from node to istelf is 0
	distance_vector[ip] = 0.0;
	//init_distance_vector(network);
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
	
	printf("FATAL: Router could not find link to requested endpoint: %s\n",
		ip_to_english(&network, endpoint_).c_str() );
	exit(-1);
}

/*
* Send a packet along the specified link.
*/
void Node::send_packet(Packet * packet_, Link link_) {
	//link.add_to_buffer(packet);

}

/*
* This function logs the received data packet and sends an acknowledgement
* packet.
*/
void Node::receive_packet(Packet * packet_) {
	// TODO
}

/* Initialize the distance vector of distances between the source node and all
* other nodes in the network.
*/
void Node::init_distance_vector(Network * network_) {
	Node * src  = get_ip();
	Network * network = network_;
	map<Node *, double> distances;

	vector<Host *> hosts = network->all_hosts;
	vector<Router *> routers = network->all_routers; 

	vector<Node *> nodes;
	nodes.reserve(hosts.size() + routers.size());
	nodes.insert(nodes.end(), hosts.begin(), hosts.end());
	nodes.insert(nodes.end(), routers.begin(), routers.end());

	// Initialize the maps
	for (int i = 0; i < nodes.size(); i++) {
		Node * node = nodes.at(i);
		if (node == src) {
			// Distance from source to itself is 0
			distances[node] = 0;
		}
		else {
			// Initialize other distances to infinity
			distances.insert(pair<Node *, double>(node, numeric_limits<double>::infinity()));
		}
	}
	// For adjacent nodes, update distances 
	vector<Link *> links = src->get_links();
	for (int i = 0; i < links.size(); i++) {
		Link * link = links.at(i);
		Node * ep1 = link->get_ep1();
		Node * ep2 = link->get_ep2();
		// Once of the endpoints is the source; set distance of other to link cost
		if (ep1 == src) {
			distances[ep2] = link->calculate_cost();
		}
		else {
			distances[ep1] = link->calculate_cost();
		}
	}
	distance_vector = distances;
}

/**
* Initialize the node's distance vector to adjacent links
*/
void Node::init_distance_vector() {
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

// Update the distance vector as routing packets are received
void Node::update_distance_vector(Rout_packet * r_packet_) {
	Rout_packet * r_packet = r_packet_;
	// Router and corresponding distance vector from packet
	Node * rnode = r_packet->get_router_source();
	map<Node *, double> packet_vector = r_packet->get_packet_vector();
	// Distance from current node to router that sent packet
	double dist_to_rsrc = distance_vector.at(rnode);
	// Iterate over nodes in the packet vector
	for (map<Node *, double>::iterator it = packet_vector.begin(); it != packet_vector.end(); it++) {
		// Destination reachable from rnode and its distance
		Node * dst = it->first;
		double dist_from_rsrc = it->second;
		// If dest node is not in distance vector, add it
		if (distance_vector.find(dst) == distance_vector.end()) {
			distance_vector.insert(pair<Node *, double>(dst, dist_to_rsrc + dist_from_rsrc));
		}
		else {
			// If a shorter path has been found, update distance vector
			double prev_dist = distance_vector.at(dst);
			if (dist_to_rsrc + dist_from_rsrc < prev_dist) {
				distance_vector[dst] = dist_to_rsrc + dist_from_rsrc;
			}
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

void Node::set_distance_vector(map<Node *, double> distances) {
	//
	for (map<Node *, double>::iterator it = distances.begin(); it != distances.end(); it++) {
		Node * node = it->first;
		double distance = it->second;
		distance_vector[node] = distance;
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

/*
* Initialize a static routing table using the Bellman Ford algorithm.
*/
void Router::init_routing_table(Network * network_) {
	Network * network = network_;
	Node * src  = get_ip();
	map<Node *, double> distances;
	map<Node *, Node *> next;

	vector<Host *> hosts = network->all_hosts;
	vector<Router *> routers = network->all_routers; 
	vector<Link *> links = network->all_links;

	vector<Node *> nodes;
	nodes.reserve(hosts.size() + routers.size());
	nodes.insert(nodes.end(), hosts.begin(), hosts.end());
	nodes.insert(nodes.end(), routers.begin(), routers.end());

	// Initialize the maps
	for (int i = 0; i < nodes.size(); i++) {
		Node * node = nodes.at(i);
		if (node == src) {
			// Distance from source to itself is 0
			distances[node] = 0;
			next[node] = NULL;
		}
		else {
			// Initialize other distances to infinity
			distances[node] = numeric_limits<double>::infinity();
			next[node] = NULL;
		}
	}

	// Update costs based on existing links
	for (int j = 0; j < nodes.size() - 1; j++) {
		for (int k = 0; k < links.size(); k++) {
			Link * link = links.at(k);
			Node * ep1 = link->get_ep1();
			Node * ep2 = link->get_ep2();
			if (distances[ep1] + link->calculate_cost() < distances[ep2]) {
				distances[ep2] = distances[ep1] + link->calculate_cost();
				next[ep2] = ep1;
			}
			else if (distances[ep2] + link->calculate_cost() < distances[ep1]) {
				distances[ep1] = distances[ep2] + link->calculate_cost();
				next[ep1] = ep2;
			}
		}
	}
	this->set_distance_vector(distances);

	// SHOULD MAKE THIS ITERATIVE
	// Populate the routing table with the information
	for (int i = 0; i < nodes.size(); i++) {
		Node * node = nodes.at(i);
		// There's a direct link between the src and the node
		if (next[node] == src) {
			routing_table[node] = node;
		}

		// Seek a node adjacent to source 
		else if (next[next[node]] == src) {
			routing_table[node] = next[node];
		}
		else {
			routing_table[node] = next[next[node]];
		}
	}

}

/**
* Initialize the routing table without external knowledge of the network.
*/ 
void Router::init_routing_table() {
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
	Node * rsrc = r_packet->get_router_source();
	map<Node *, double> packet_vector = r_packet->get_packet_vector();
	map<Node *, double> distances = this->get_distance_vector();
	// Distance from current node to router that sent packet
	double dist_to_rsrc = distances.at(rsrc);
	// Iterate over nodes in the packet vector
	for (map<Node *, double>::iterator it = packet_vector.begin(); it != packet_vector.end(); it++) {
		// Destination reachable from rnode and its distance
		Node * dst = it->first;
		double dist_from_rsrc = it->second;
		// If dest node is not in distance vector, add it
		if (distances.find(dst) == distances.end()) {
			distances.insert(pair<Node *, double>(dst, dist_to_rsrc + dist_from_rsrc));
			// Add the node to the routing table. Since the shortest path 
			// goes through rsrc
			routing_table.insert(pair<Node *, Node *>(dst, routing_table.at(rsrc)));
		}
		else {
			// If a shorter path has been found, update distance vector
			double prev_dist = distances.at(dst);
			if (dist_to_rsrc + dist_from_rsrc < prev_dist) {
				distances[dst] = dist_to_rsrc + dist_from_rsrc;
				routing_table.insert(pair<Node *, Node *>(dst, routing_table.at(rsrc)));
			}
		}

	}
}


map<Node *, Node *> Router::get_routing_table() {
	return routing_table;
}

//USED FOR DEBUGGING
void Router::print_routing_table() {
	//
	cout << "Routing table for source: " << ip_to_english(&network, this) << "\n";
	for (map<Node *, Node *>::iterator it=routing_table.begin(); it!=routing_table.end(); ++it) {
		//
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
	this->update_distance_vector(r_packet);
	this->update_routing_table(r_packet);
	// TODO
}

/**
* Used for routing: router sends its distance vector to all known nodes.
*/
void Router::send_distance_vector() {
	// For every node in the routing table, send the distance vector
	for (map<Node *, Node *>::iterator it=routing_table.begin(); it!=routing_table.end(); ++it) {
		Node * dst = it->first;
		Node * next_node = it->second;
		map<Node *, double> d_vector = this->get_distance_vector();
		// Create a routing packet with the distance vector
		Rout_packet r_packet = Rout_packet(this, dst, d_vector);
		// Find the link associated with the next hop and transmit the packet
		Link * next_link = this->get_link(next_node);
		// TODO
		// Add a routing packet send event to the routing queue.
	}
	// 
}

/**
* The link processes all incoming distance vectors before sending its updated
* version.
*/
void Router::process_incoming_vectors() {
	// Wait for all routing receive events to be processed
	while (1) {
		if (routing_queue.empty()) {
			break;
		}
	}

	// TODO
	
}



