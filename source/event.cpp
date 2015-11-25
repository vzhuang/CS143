#include "event.h"
#include "parser.h"
extern double global_time;
extern priority_queue<Event *, vector<Event *>, CompareEvents> event_queue;
extern Network network;
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
		//TODO. Fake ass implementation. Will likely want to always
		// have duplicate packets being sent until an ack is recieved
		// and the flow object is modified appropriately in the 
		// "Ack_Receieve_Event" handler.
		double start = flow->get_start();
		Host * source = flow->get_source();
		Host * destination = flow->get_destination();
		printf("This flow is going from %s to %s\n\n",
			ip_to_english(&network, source).c_str(),
			ip_to_english(&network, destination).c_str() );
		Link * link = flow->get_source()->get_first_link();
		Data_packet * packet = new Data_packet(source, destination, 0, flow);
		// Always push packet to buffer before spawning send event
		if( link->add_to_buffer(packet, (Node *) source) == 0)
		{ 
			Link_Send_Event * event = new Link_Send_Event(start, SEND_EVENT_ID, link);
			event_queue.push(event);
		}
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
	printf("Sending packet on link %s. Time: %f\n\n",
		link_to_english(&network, link).c_str(), global_time);
	link->transmit_packet();
}

/////////////// Link_Free_Event /////////////////
Link_Free_Event::Link_Free_Event(double start_, int event_ID_, Link * link_)
           : Event(start_, event_ID_)
{
	link = link_;
}

void Link_Free_Event::handle_event()
{
	global_time = this->get_start();
	link->is_free = 1;
	printf("Packet moved through Link %s. It is available again. Time: %f\n\n",
		link_to_english(&network, link).c_str(), global_time);
}

/////////////// Ack_Receive_Event (an ack was recieved by the source) /////////////////
Ack_Receive_Event::Ack_Receive_Event(double start_, int event_ID_, Ack_packet * ack_)
           : Event(start_, event_ID_)
{
	ack = ack_;
}

void Ack_Receive_Event::handle_event()
{
	global_time = this->get_start();
	printf("Ack #%d recieved at host: %s at time: %f\n\n", 
		ack->get_index(),
		ip_to_english(&network, ack->getDest()).c_str(),
		global_time);
	Link * link = ack->getSource()->get_first_link();

	/* TODO: Update flow based on TCP_ID and send 1 or more
	 *  Link_Send_Events. Remember to check that packet could be added 
	 * to buffer i.e: if( link->add_to_buffer(packet) == 0) { ...
	 */
	 delete ack;
}

/////////////// Data_Receive_Event (data was recieved by the dest) /////////////////
Data_Receive_Event::Data_Receive_Event(double start_, int event_ID_, Data_packet * data_)
           : Event(start_, event_ID_)
{
	data = data_;
}

void Data_Receive_Event::handle_event()
{
	global_time = this->get_start();
	printf("Packet #%d recieved at host: %s at time: %f\n\n", 
			data->get_index(),
			ip_to_english(&network, data->getDest()).c_str(),
			global_time);
	// Create ack packet to send back to source
	Ack_packet * ack = new Ack_packet(data->getDest(),
									data->getSource(),
									data->getFlow(),
									data->get_index());
	Link * link_to_send_ack = ack->getSource()->get_first_link();
	// Always push packet to buffer before spawning send event
	if(link_to_send_ack->add_to_buffer(ack, ack->getSource()) == 0)
	{
		Link_Send_Event * event = new Link_Send_Event(
									global_time,
									SEND_EVENT_ID,
									link_to_send_ack);
		event_queue.push(event);
	}
	delete data;
}
