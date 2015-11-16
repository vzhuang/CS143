#include "event.h"
extern double global_time;
extern priority_queue<Event *, vector<Event *>, CompareEvents> event_queue;

/////////////////// Generic Event Superclass /////////////////////
Event::Event(double start_, int event_ID_)
{
	event_ID = event_ID_;
	start = start_;
}

int Event::get_ID()
{
	return event_ID;
}

double Event::get_start() const
{
	return start;
}


void Event::handle_event()
{
	// VIRTUAL FUNCTION - NEVER CALLED. JUST A TEMPLATE.
}

/////////////// Flow_Start_Event /////////////////
Flow_Start_Event::Flow_Start_Event(double start_, int event_ID_, Flow * flow_)
           : Event(start_, event_ID_)
{
	flow = flow_;
}

void Flow_Start_Event::handle_event()
{
	global_time = this->get_start();
	int event_ID = this->get_ID();
	if(event_ID == TCP_TAHOE)
	{
		//TODO. Fake ass implementation
		double start = flow->get_start();
		Host * source = flow->get_source();
		Host * destination = flow->get_destination();
		Link * link = flow->get_source()->get_first_link();
		Data_packet * packet = new Data_packet(source, destination);
		link->add_to_buffer(*packet); 
		Link_Send_Event * event = new Link_Send_Event(start, SEND_EVENT_ID, link);
		event_queue.push(event);
	}
	else if(event_ID == TCP_RENO)
	{
		//TODO
	}
	else if(event_ID == TCP_FAST)
	{
		//TODO
	}
	else
	{
		printf("Invalid event_ID: %d\n", event_ID);
		exit(-1);
	}

}

/////////////// Link_Send_Event /////////////////
Link_Send_Event::Link_Send_Event(double start_, int event_ID_, Link * link_)
           : Event(start_, event_ID_)
{
	link = link_;
}

void Link_Send_Event::handle_event()
{
	global_time = this->get_start();
	// TODO
}


/////////////// Link_Receive_Event /////////////////
Link_Receive_Event::Link_Receive_Event(double start_, int event_ID_, Host * host_)
           : Event(start_, event_ID_)
{
	host = host_;
}

void Link_Receive_Event::handle_event()
{
	global_time = this->get_start();
	// TODO
}

/////////////// Ack_Receive_Event /////////////////
Ack_Receive_Event::Ack_Receive_Event(double start_, int event_ID_, Ack_packet * ack_)
           : Event(start_, event_ID_)
{
	ack = ack_;
}

void Ack_Receive_Event::handle_event()
{
	global_time = this->get_start();
	// TODO
}
