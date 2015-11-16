#ifndef EVENT_H
#define EVENT_H

#include <string>
#include "node.h"
#include "flow.h"
#include "link.h"
#include "packet.h"

#define SEND_EVENT_ID 0
#define RECIEVE_EVENT_ID 1
#define ACK_RECIEVE_ID 2
#define FLOW_START_ID 3

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


/////////////// Link_Send_Event /////////////////
class Link_Send_Event : public Event 
{
	Link * link;
	
public:
	Link_Send_Event(double start_, int event_ID_, Link * link_);
	void handle_event();
	
};

/////////////// Link_Receive_Event /////////////////
class Link_Receive_Event : public Event 
{
	Host * host;
	
public:
	Link_Receive_Event(double start_, int event_ID_, Host * host_);
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

// Allows priority queue to determine priority of an Event *
struct CompareEvents {
  bool operator() (Event * arg1, Event * arg2) {
    return arg1->get_start() > arg2->get_start();
  }
};

#endif
