#ifndef PACKET_H
#define PACKET_H

#include <map>
#include "node.h"
#include "flow.h"
#include "link.h"
#include <vector>
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
class Packet { 
    Node * source;
    Node * dest;
    double size;
    Flow * flow;
public:
    Packet(Node * source_, Node * dest_, double the_size, Flow * flow_);
    Node * getDest();
    Node * getSource();
    Flow * getFlow();
    double packetSize();
    virtual int getId();
    virtual int get_index();

};

/////////////// Data Packet /////////////////
class Data_packet : public Packet {
	int index;
	Flow * flow;
    double time;
public:
	Data_packet(Host * source_, 
                Host * dest_, int index_, Flow * flow_, double time_);
    double get_time();
	int getId();
	int get_index();
};

/////////////// Routing Packet /////////////////
class Rout_packet : public Packet {
	Node * rsrc;
	map<Node *, double> packet_vector;
public:
	Rout_packet(Node * source_, 
		       Node * dest_, 
		       map<Node *, double> distance_vector);
	int getId();
	Node * get_router_source();
	map<Node *, double> get_packet_vector();
	
};

/////////////// Ack Packet /////////////////
class Ack_packet : public Packet {
    double time;
public:
	Ack_packet(Host * source_,
              Host * dest_,
              Flow * flow_,
              int my_index,
              double time_);
    double get_time();
	int get_index();
	int getId();
		
protected:
	Flow * my_flow;
	int index;
};
#endif
