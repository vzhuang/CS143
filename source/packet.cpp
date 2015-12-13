/**
    CS-143 Network Simulator
    packet.cpp
    Purpose: Creates packet obejcts which are used to transmit data
    across the network links.

    @author Luciana Cendon, Jordan Bonilla, Vivian He, Vincent Zhuang
    @version 1.0 12/11/15
*/


#include "packet.hpp"
/////////////////// Generic Packet Superclass //////////////////////////
Packet::Packet(Node * source_, 
	           Node * dest_, 
	           double size_,
	           Flow * flow_) {
	source = source_;
	dest = dest_;
	size = size_;
	flow = flow_;
}

Node * Packet::get_dest() {
	return dest;
}

Node * Packet::get_source() {
	return source;
}

Flow * Packet::get_flow() {
	return flow;
}

double Packet::packetSize() {
	return size;
}

int Packet::get_id() {
	return -1;
}

int Packet::get_index() {
	return -1;
}	

/////////////// Data Packet ////////////////////////////////////////////
Data_packet::Data_packet(Host * source_, 
						 Host * dest_, int the_index, 
						 Flow * flow_, double time_)
           : Packet(source_, dest_, DATA_SIZE, flow_) {
	flow = flow_;
	index = the_index;
	time = time_;
}

double Data_packet::get_time() {
	return time;
}

int Data_packet::get_id() {
	return DATA_ID;
}

int Data_packet::get_index() {
	return index;
}
/////////////////// Routing Packet /////////////////////////////////////
Rout_packet::Rout_packet(Node * source_, 
	                   Node * dest_, 
	                   map<Node *, double> distance_vector)
           : Packet(source_, dest_, ROUTING_SIZE, NULL) {
	packet_vector = distance_vector;
}

int Rout_packet::get_id() {
	return ROUT_ID;
}

Node * Rout_packet::get_router_source() {
	return rsrc;
}

map<Node *, double> Rout_packet::get_packet_vector() {
	return packet_vector;
}
/////////////////// Ack Packet /////////////////////////////////////////
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

int Ack_packet::get_id() {
	return ACK_ID;
}
