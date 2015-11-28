#ifndef EVENT_H
#define EVENT_H

#include <string>
#include "node.h"
#include "flow.h"
#include "link.h"
#include "packet.h"

#define SEND_EVENT_ID 0
#define FLOW_START_ID 1
#define LINK_FREE_ID 2
#define DATA_RECEIVE_ID 3
#define ACK_RECEIVE_ID 4

class Ack_packet;
class Data_packet;
class Network;
using namespace std;

/////////////// General Event Superclass /////////////////
class Event
{ 
    double start;
    int event_ID;

public:
    Event(double start_, int event_ID_);
    int get_ID();
    virtual void handle_event();
    double get_start() const;
};

/////////////// Flow_Start_Event /////////////////
class Flow_Start_Event : public Event 
{
	Flow * flow;
	
public:
	Flow_Start_Event(double start_, int event_ID_, Flow * flow_);
	void handle_event();
	
};

/////////////// Routing_Start_Event /////////////////
class Routing_Event : public Event 
{
	Network * network;
public:
	Routing_Event(double start_, int event_ID_, Network * network);
	void handle_event();
	
};

/////////////// Link_Send_Event /////////////////
class Link_Send_Event : public Event 
{
	Link * link;
	
public:
	Link_Send_Event(double start_, int event_ID_, Link * link_);
	void handle_event();
	
};

/////////////// Link_Free_Event /////////////////
class Link_Free_Event : public Event 
{
	Link * link;
	
public:
	Link_Free_Event(double start_, int event_ID_, Link * link_);
	void handle_event();
	
};

/////////////// Ack_Receive_Event /////////////////
class Ack_Receive_Event : public Event 
{
	Ack_packet * ack;
	
public:
	Ack_Receive_Event(double start_, int event_ID_, Ack_packet * ack_);
	void handle_event();
	
};

/////////////// Data_Receive_Event /////////////////
class Data_Receive_Event : public Event 
{
	Data_packet * data;
	
public:
	Data_Receive_Event(double start_, int event_ID_, Data_packet * data_);
	void handle_event();
	
};

/////////////// Time_Out_Event //////////////////
class Time_Out_Event : public Event {
    Data_packet * data;
public:
    Time_Out_Event(double start_, int event_ID_, Data_packet * data_);
    void handle_event();
};

// Allows priority queue to determine priority of an Event *
struct CompareEvents {
  bool operator() (Event * arg1, Event * arg2) {
    return arg1->get_start() > arg2->get_start();
  }
};

#endif
