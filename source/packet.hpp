/* PACKET_HPP */

#ifndef PACKET_HPP
#define PACKET_HPP

#include <map>
#include "node.hpp"
#include "flow.hpp"
#include "link.hpp"
#include <vector>
#include "mex.h"
// Packet sizes in bytes
#define DATA_SIZE 1024.0
#define ROUTING_SIZE 1.0
#define ACK_SIZE 4.0
// Packet IDs
#define DATA_ID 0
#define ROUT_ID 1
#define ACK_ID 2

struct routing_table;
class Node;
class Flow;
class Host;


using namespace std;

/////////////// General Packet Superclass //////////////////////////////
class Packet { 
    Node * source;
    Node * dest;
    double size;
    Flow * flow;
public:
    Packet(Node * source_, Node * dest_, double the_size, Flow * flow_);
    Node * get_dest();
    Node * get_source();
    Flow * get_flow();
    double packet_size();
    virtual int get_id();
    virtual int get_index();

};

/////////////// Data Packet ////////////////////////////////////////////
class Data_packet : public Packet {
	int index;
	Flow * flow;
    double time;
public:
	Data_packet(Host * source_, 
                Host * dest_, int index_, Flow * flow_, double time_);
    double get_time();
	int get_id();
	int get_index();
};

/////////////// Routing Packet /////////////////////////////////////////
class Rout_packet : public Packet {
	Node * rsrc;
	map<Node *, double> packet_vector;
public:
	Rout_packet(Node * source_, 
		       Node * dest_, 
		       map<Node *, double> distance_vector);
	int get_id();
	Node * get_router_source();
	map<Node *, double> get_packet_vector();
	
};

/////////////// Ack Packet /////////////////////////////////////////////
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
	int get_id();
		
protected:
	Flow * my_flow;
	int index;
};
#endif /* PACKET_HPP */
