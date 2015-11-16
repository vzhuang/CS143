#include "packet.h"
/////////////////// Generic Packet Superclass /////////////////////
Packet::Packet(Host * the_source, 
	           Host * the_dest, 
	           double the_size,
	           Flow * the_flow)
{
	source = the_source;
	dest = the_dest;
	size = the_size;
	flow = the_flow;
}

Host * Packet::getDest()
{
	return dest;
}

Host * Packet::getSource()
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
	                   Host * the_dest, int the_index, Flow * the_flow)
           : Packet(the_source, the_dest, DATA_SIZE, the_flow)
{
	flow = the_flow;
	index = the_index;
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
Rout_packet::Rout_packet(Host * the_source, 
	                   Host * the_dest, 
	                   routing_table * the_rtable)
           : Packet(the_source, the_dest, ROUTING_SIZE, NULL)
{
	rtable = the_rtable;
}

routing_table * Rout_packet::getTable()
{
	return rtable;
}

int Rout_packet::getId()
{
	return ROUT_ID;
}
/////////////////// Ack Packet /////////////////////
Ack_packet::Ack_packet(Host * the_source, 
	                   Host * the_dest, 
	                   Flow * the_flow,
	                   int my_index)
           : Packet(the_source, the_dest, ACK_SIZE, the_flow)
{
	my_flow = the_flow;
	index = my_index;
}

int Ack_packet::get_index()
{
	return index;
}

int Ack_packet::getId()
{
	return ACK_ID;
}
