#include "packet.h"
/////////////////// Generic Packet Superclass /////////////////////
Packet::Packet(Node * source_, 
	           Node * dest_, 
	           double size_,
	           Flow * flow_) {
	source = source_;
	dest = dest_;
	size = size_;
	flow = flow_;
}

Node * Packet::getDest() {
	return dest;
}

Node * Packet::getSource() {
	return source;
}

Flow * Packet::getFlow() {
	return flow;
}

double Packet::packetSize() {
	return size;
}

int Packet::getId() {
	// VIRTUAL FUNCTION - NEVER CALLED. JUST A TEMPLATE.
	return -1;
}

int Packet::get_index() {
	// VIRTUAL FUNCTION - NEVER CALLED. JUST A TEMPLATE.
	return -1;
}	

/////////////// Data Packet /////////////////
Data_packet::Data_packet(Host * source_, 
						 Host * dest_, int the_index, Flow * flow_, double time_)
           : Packet(source_, dest_, DATA_SIZE, flow_) {
	flow = flow_;
	index = the_index;
	time = time_;
}

double Data_packet::get_time() {
	return time;
}

int Data_packet::getId() {
	return DATA_ID;
}

int Data_packet::get_index() {
	return index;
}
/////////////////// Routing Packet /////////////////////
Rout_packet::Rout_packet(Node * source_, 
	                   Node * dest_, 
	                   map<Node *, double> distance_vector)
           : Packet(source_, dest_, ROUTING_SIZE, NULL) {
	packet_vector = distance_vector;
}

int Rout_packet::getId() {
	return ROUT_ID;
}

Node * Rout_packet::get_router_source() {
	return rsrc;
}

map<Node *, double> Rout_packet::get_packet_vector() {
	return packet_vector;
}
/////////////////// Ack Packet /////////////////////
Ack_packet::Ack_packet(Host * source_, 
	                  Host * dest_, 
	                  Flow * flow_,
					  int index_,
					  double time_)
           : Packet(source_, dest_, ACK_SIZE, flow_)
{
	my_flow = flow_;
	index = index_;
	time = time_;
}

double Ack_packet::get_time() {
	return time;
}

int Ack_packet::get_index() {
	return index;
}

int Ack_packet::getId() {
	return ACK_ID;
}
