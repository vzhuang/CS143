#include "parser.hpp"

char * file_name;

/* Decode a token in the from R/H# into an actual IP address (Node *) 
 */
Node * parse_ip(string token, Network * network) {
	string node_type = token.substr(0,1);
	int node_number = stoi(token.substr(1)) - 1;

	if(node_type.compare("H") == 0) {
		return network->all_hosts[node_number]->get_ip();
	}
	else if(node_type.compare("R") == 0) {
		return network->all_routers[node_number]->get_ip();
	}
	else {
		mexPrintf( "Invalid R/H# token. Unexpected token: %s.\n", token.c_str());
		mexErrMsgTxt("Exiting\n");
	}
}
	
/* Create a network at the passed in Network * using the passed in 
   network specification file. */
void build_network(Network * network, char * network_file) {
	int num_hosts, num_routers;
	string token;
	ifstream myfile;
	file_name = network_file;
	
	// Open the input file
	myfile.open(network_file);
  	if(! myfile.is_open()) {
		mexPrintf("Could not open file: %s. Exiting.\n", network_file);
		mexErrMsgTxt("");
	}
	// Read the number of hosts
	myfile >> token;
  	if(token.compare("Hosts:") != 0) {
		mexPrintf("Invalid file format. Unexpected token: %s. Exiting.\n", token.c_str());
		mexErrMsgTxt("");
	}
	myfile >> token;
	num_hosts = stoi(token);
	
	// Read the number of routers
	myfile >> token;
  	if(token.compare("Routers:") != 0) {
		mexPrintf( "Invalid file format. Unexpected token: %s. Exiting.\n", token.c_str());
		mexErrMsgTxt("");
	}
	myfile >> token;
	num_routers = stoi(token);
	
	// Create the hosts and routers
	for(int i = 0; i < num_hosts; i++) {
		Host * host = new Host;
		network->all_hosts.push_back(host);
	}
	for(int i = 0; i < num_routers; i++) {
		Router * router = new Router;
		network->all_routers.push_back(router);
	}
	
	
	// Read and create the links, conencting them to their endpoints
	myfile >> token;
  	if(token.compare("Links:") != 0) {
		mexPrintf( "Invalid file format. Unexpected token: %s. Exiting.\n", token.c_str());
		mexErrMsgTxt("");
	}
	myfile >> token;
	while(token.compare("Flows:") != 0) {
		if(myfile.eof()) {
			mexPrintf( "Invalid file format. Exiting\n");
			mexErrMsgTxt("");
		}
		Node * ep1 = parse_ip(token, network);
		myfile >> token;
		Node * ep2 = parse_ip(token, network);
		myfile >> token;
		double capacity = 1000000.0 / 8.0 * stod(token); // Since its given in Mb/s and we want B/s
		myfile >> token;
		double delay = stod(token) / 1000.0; // Since its given in ms and we want s
		myfile >> token;
		double buffer_size = 1000.0 * stod(token);	 // Since it's given in KB	and we want B
		Link * link = new Link(capacity, ep1, ep2, delay, buffer_size);
		link->connect(ep1,ep2);
		
		network->all_links.push_back(link);
		
		myfile >> token;
	}

	// Read and create the flows
	myfile >> token;
	while(!myfile.eof()) {
		Host * source = (Host *) parse_ip(token, network);
		myfile >> token;
		Host * dest = (Host *) parse_ip(token, network);
		myfile >> token;
		double data_amount = stod(token) * 1000000.0; // Since given in MB and we want B;
		myfile >> token;
		double start = stod(token);
		Flow * flow = new Flow(source, dest, data_amount, start);
		network->all_flows.push_back(flow);
		
		myfile >> token;
	}
}

// Returns node "H/R# as specified by test case diagram"
string ip_to_english(Network * network, Node * node) {
	if(node == NULL) {
		return "NULL";
	}
	int num_r = network->all_routers.size();
	int num_h = network->all_hosts.size();
	string output;
	for(int i = 0; i < num_r; i++) {
		if(network->all_routers[i] == node) {
			output = "R";
			output += std::to_string(i + 1);
			return output; 
		}
	}
	for(int i = 0; i < num_h; i++) {
		if(network->all_hosts[i] == node) {
			output = "H";
			output += std::to_string(i + 1);
			return output; 
		}
	}
	mexPrintf("FATAL: ip: %lu not in this network\n", (long unsigned int) node);
	mexErrMsgTxt("");
}

// Returns link "L#" as specified by test case diagram
string link_to_english(Network * network, Link * link) {
	int num_l = network->all_links.size();
	string output;
	for(int i = 0; i < num_l; i++) {
		if(network->all_links[i] == link) {
			output = "L";
			if (strcmp(file_name, "testcase1.txt") == 0) {
				output += std::to_string(i);
			}
			else {
				output += std::to_string(i + 1);
			}
			return output; 
		}
	}
	mexPrintf("FATAL: link: %lu not in this network\n", (long unsigned int) link);
	mexErrMsgTxt("");
}
