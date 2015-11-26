#include "node.h"
#include <map>
#include <vector>
#include "parser.h"
#include "network.h"
extern Network network;

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
* Get the link connecting the current node to the desired endpoint
*/
Link * Node::get_link(Node * endpoint) {
	vector<Link *> links = this->get_links();
	for (int i = 0; i < links.size(); i++) 
	{
		Link * link = links.at(i);
		if (link->get_ep1() == endpoint || link->get_ep2() == endpoint)
		{
			return link;
		}
	}
	
	printf("FATAL: Router could not find link to requested endpoint: %s\n",
		ip_to_english(&network, endpoint).c_str() );
	exit(-1);
}

/*
* Send a packet along the specified link.
*/
void Node::send_packet(Packet * packet, Link link) {
	//link.add_to_buffer(packet);

}

/*
* This function logs the received data packet and sends an acknowledgement
* packet.
*/
void Node::receive_packet(Packet * packet) {
	// TODO
}

/* Initialize the distance vector of distances between the source node and all
* other nodes in the network.
*/
void Node::init_distance_vector(Network * network) {
	Node * src  = get_ip();
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

void Node::update_distance_vector(Rout_packet * rpacket) {
	Node * rnode = rpacket->get_router_source();
	// Distance from the source to the router that sent the routing packet
	map<Node *, double> distance_vector = this->get_distance_vector();
	double dist_to_rsrc = distance_vector.at(rnode);
	map<Node *, double> packet_vector = rpacket->get_packet_vector();
	for (map<Node *, double>::iterator it = distance_vector.begin(); it != distance_vector.end(); it++) {
		Node * dst = it->first;
		double prev_dist = it->second;
		// Distance from the routing packet source node to the destination
		double rsrc_to_dst = packet_vector.at(dst);
		// If the distance going through rnode is less than the previously 
		// known shortest distance, update
		if (dist_to_rsrc + rsrc_to_dst > prev_dist) {
			distance_vector[dst] = dist_to_rsrc + rsrc_to_dst;
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
* Initialize a routing table using the Bellman Ford algorithm.
*/
void Router::init_routing_table(Network * network) {
	//
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


/*
* Update a routing table dynamically when a routing packet is received.
*/
void Router::update_routing_table(Rout_packet * rpacket) {
	//
	Node * rnode = rpacket->get_router_source();
	map<Node *, Node *> next;
	// Distance from the source to the router that sent the routing packet
	map<Node *, double> distance_vector = this->get_distance_vector();
	double dist_to_rsrc = distance_vector.at(rnode);
	map<Node *, double> packet_vector = rpacket->get_packet_vector();
	// Iterate over all destination nodes
	for (map<Node *, double>::iterator it = distance_vector.begin(); it != distance_vector.end(); it++) {
		Node * dst = it->first;
		double prev_dist = it->second;
		// Distance from the routing packet source node to the destination
		double rsrc_to_dst = packet_vector.at(dst);
		// If the distance going through rnode is less than the previously 
		// known shortest distance, update routing table
		if (dist_to_rsrc + rsrc_to_dst > prev_dist) {
			distance_vector[dst] = dist_to_rsrc + rsrc_to_dst;
			routing_table[dst] = rnode;
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






