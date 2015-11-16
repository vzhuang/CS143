#include "packet.h"
/////////////////// Generic Packet Superclass /////////////////////
Packet::Packet(Host * the_source, 
	           Host * the_dest, 
	           int the_size)
{
	source = the_source;
	dest = the_dest;
	size = the_size;
}

Host * Packet::getDest()
{
	return dest;
}

Host * Packet::getSource()
{
	return source;
}

int Packet::packetSize()
{
	return size;
}

int Packet::getId()
{
	// VIERTUAL FUNCTION - NEVER CALLED. JUST A TEMPLATE.
	return -1;
}
	
	
/////////////// Data Packet /////////////////
Data_packet::Data_packet(Host * the_source, 
	                   Host * the_dest)
           : Packet(the_source, the_dest, DATA_SIZE)
{
	// Everything initialized in superclass constructor
}

int Data_packet::getId()
{
	return DATA_ID;
}

/////////////////// Routing Packet /////////////////////
Rout_packet::Rout_packet(Host * the_source, 
	                   Host * the_dest, 
	                   routing_table * the_rtable)
           : Packet(the_source, the_dest, ROUTING_SIZE)
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
           : Packet(the_source, the_dest, ACK_SIZE)
{
	my_flow = the_flow;
	index = my_index;
}

Flow * Ack_packet::getFlow()
{
	return my_flow;
}

int Ack_packet::getIndex()
{
	return index;
}

int Ack_packet::getId()
{
	return ACK_ID;
}
