#ifndef EVENT_HPP
#define EVENT_HPP

#include <string>
#include "node.hpp"
#include "flow.hpp"
#include "link.hpp"
#include "packet.hpp"

#define FLOW_START_EVENT_ID 0
#define SEND_EVENT_ID 1
#define LINK_FREE_EVENT_ID 2
#define DATA_RECEIVE_EVENT_ID 3
#define ACK_RECEIVE_EVENT_ID 4
#define ROUT_RECEIVE_EVENT_ID 5
#define TIMEOUT_EVENT_ID 6
#define RSEND_EVENT_ID 7
#define RFREE_EVENT_ID 8
#define REFRESH_RTABLES_EVENT_ID 9
#define FAST_UPDATE_EVENT_ID 10
#define SEND_NEW_PACKETS_EVENT_ID 11
class Ack_packet;
class Data_packet;
class Rout_packet;
class Network;
class Router;
using namespace std;

/////////////// General Event Superclass /////////////////
class Event { 
    double start;
    int event_ID;

public:
    Event(double start_, int event_ID_);
    int get_ID();
    virtual void handle_event();
    double get_start();
    void change_start(double new_start);
};

/////////////// Flow_Start_Event /////////////////
class Flow_Start_Event : public Event {
	Flow * flow;
	int TCP_type;
public:
	Flow_Start_Event(double start_, int TCP_type_, Flow * flow_);
	void handle_event();
	
};

////////////// Send_New_Packets_Event ////////////////
class Send_New_Packets_Event : public Event {
	Flow * flow;
public:
    Send_New_Packets_Event(double start_, Flow * flow_);
	void handle_event();
};


/////////////// Link_Send_Event /////////////////
class Link_Send_Event : public Event {
	Link * link;
public:
	Link_Send_Event(double start_, Link * link_, double packetsize_);
	void handle_event();
};

/////////////// Link_Send_Routing_Event /////////////////
class Link_Send_Routing_Event : public Event {
	Link * link;
	
public:
	Link_Send_Routing_Event(double start_, Link * link_);
	void handle_event();
};

/////////////// Link_Free_Event /////////////////
class Link_Free_Event : public Event {
	Link * link;
	int direction;
public:
	Link_Free_Event(double start_, int event_ID_, Link * link_, int direction_);
	void handle_event();
	
};

/////////////// Ack_Receive_Event /////////////////
class Ack_Receive_Event : public Event {
	Ack_packet * ack;
    double last_action_time;
	
public:
	Ack_Receive_Event(double start_, Ack_packet * ack_);
	void handle_event();
	
};

/////////////// Data_Receive_Event /////////////////
class Data_Receive_Event : public Event {
	Data_packet * data;
	
public:
	Data_Receive_Event(double start_, Data_packet * data_);
	void handle_event();
	
};

/////////////// Packet_Receive_Event /////////////////
class Packet_Receive_Event : public Event {
	Packet * packet;
	Link * link;
	Node * src;
public:
	Packet_Receive_Event(double start_, int event_ID_, Packet * packet_, Link * link_, Node * src_);
	void handle_event();
	
};

/////////////// Rout_Receive_Event /////////////////
class Rout_Receive_Event : public Event {
	Rout_packet * r_packet;
	Router * router;
	
public:
	Rout_Receive_Event(Router * router_, double start_, Rout_packet * r_packet_);
	void handle_event();
	
};
/////////////// Time_Out_Event //////////////////
class Time_Out_Event : public Event {
    Flow * flow;
    int index;
    
public:
    Time_Out_Event(double start_, Flow * flow_, int index_);
    void handle_event();
};
/////////////// Update_Rtables_Event //////////////////
class Update_Rtables_Event : public Event {
    Network * network;
public:
    Update_Rtables_Event(double start_, Network * network_);
    void handle_event();
};


///////////// Fast_Update_Event /////////////
class Fast_Update_Event : public Event {
    Flow * flow;
public:
    Fast_Update_Event(double start_, Flow * flow_);
    void handle_event();
};

// Allows priority queue to determine priority of an Event *
struct CompareEvents {
  bool operator() (Event * arg1, Event * arg2) {
    return arg1->get_start() > arg2->get_start();
  }
};

#endif
