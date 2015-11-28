#include "packet.h"
/////////////////// Generic Packet Superclass /////////////////////
Packet::Packet(Node * the_source, 
	           Node * the_dest, 
	           double the_size,
	           Flow * the_flow)
{
	source = the_source;
	dest = the_dest;
	size = the_size;
	flow = the_flow;
}

Node * Packet::getDest()
{
	return dest;
}

Node * Packet::getSource()
{
	return source;
}

Flow * Packet::getFlow()
{
	return flow;
}

double Packet::packetSize()
{
	return size;
}

int Packet::getId()
{
	// VIRTUAL FUNCTION - NEVER CALLED. JUST A TEMPLATE.
	return -1;
}

int Packet::get_index()
{
	// VIRTUAL FUNCTION - NEVER CALLED. JUST A TEMPLATE.
	return -1;
}	
/////////////// Data Packet /////////////////
Data_packet::Data_packet(Host * the_source, 
						 Host * the_dest, int the_index, Flow * the_flow, double time_)
           : Packet(the_source, the_dest, DATA_SIZE, the_flow)
{
	flow = the_flow;
	index = the_index;
	time = time_;
}

double Data_packet::get_time() {
	return time;
}

int Data_packet::getId()
{
	return DATA_ID;
}

int Data_packet::get_index()
{
	return index;
}
/////////////////// Routing Packet /////////////////////
Rout_packet::Rout_packet(Node * the_source, 
	                   Node * the_dest, 
	                   map<Node *, double> distance_vector)
           : Packet(the_source, the_dest, ROUTING_SIZE, NULL)
{
	packet_vector = distance_vector;
}

routing_table * Rout_packet::getTable()
{
	return rtable;
}

int Rout_packet::getId()
{
	return ROUT_ID;
}

Node * Rout_packet::get_router_source() {
	return rsrc;
}

map<Node *, double> Rout_packet::get_packet_vector() {
	return packet_vector;
}
/////////////////// Ack Packet /////////////////////
Ack_packet::Ack_packet(Host * the_source, 
	                  Host * the_dest, 
	                  Flow * the_flow,
					  int my_index,
					  double time_)
           : Packet(the_source, the_dest, ACK_SIZE, the_flow)
{
	my_flow = the_flow;
	index = my_index;
	time = time_;
}

double Ack_packet::get_time()
{
	return time;
}

int Ack_packet::get_index()
{
	return index;
}

int Ack_packet::getId()
{
	return ACK_ID;
}
