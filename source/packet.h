
#ifndef PACKET_H
#define PACKET_H

#include <string>
#include "node.h"
#include "flow.h"
#include "link.h"
// Packet sizes in bytes
#define DATA_SIZE 1024
#define ROUTING_SIZE 1
#define ACK_SIZE 4
// Packet IDs
#define DATA_ID 0
#define ROUT_ID 1
#define ACK_ID 2

struct routing_table;
class Node;
class Flow;
class Host;


using namespace std;

/////////////// General Packet Superclass /////////////////
class Packet
{ 
    Host * source;
    Host * dest;
    int size;
public:
    Packet(Host * the_source, Host * the_dest, int the_size);
    Host * getDest();
    Host * getSource();
    int packetSize();
    virtual int getId();

};
/////////////// Data Packet /////////////////
class Data_packet : public Packet 
{
public:
	Data_packet(Host * the_source, 
		       Host * the_dest);
	int getId();
};

/////////////// Routing Packet /////////////////
class Rout_packet : public Packet 
{
public:
	Rout_packet(Host * the_source, 
		       Host * the_dest, 
		       routing_table * the_rtable);
	routing_table * getTable();
	int getId();
	
protected:
	routing_table * rtable;
};

/////////////// Ack Packet /////////////////
class Ack_packet : public Packet 
{
public:
	Ack_packet(Host * the_source, 
		       Host * the_dest, 
		       Flow * the_flow,
		       int my_index);

	Flow * getFlow();
	int getIndex();
	int getId();
		
protected:
	Flow * my_flow;
	int index;
};
#endif
