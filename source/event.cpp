#include "event.h"
#include "parser.h"
extern double global_time;
extern priority_queue<Event *, vector<Event *>, CompareEvents> event_queue;
extern Network network;
/////////////////// Generic Event Superclass /////////////////////
Event::Event(double start_, int event_ID_) {
	event_ID = event_ID_;
	start = start_;
}
 
int Event::get_ID() {
	return event_ID;
}

double Event::get_start() {
	return start;
}


void Event::handle_event() {
	// VIRTUAL FUNCTION - NEVER CALLED. JUST A TEMPLATE.
}

/////////////// Flow_Start_Event /////////////////
Flow_Start_Event::Flow_Start_Event(double start_, int event_ID_, Flow * flow_)
           : Event(start_, event_ID_) {
	flow = flow_;
}

void Flow_Start_Event::handle_event() {
	global_time = this->get_start();
	int event_ID = this->get_ID();
	if(event_ID == TCP_TAHOE) {
		//TODO. Fake ass implementation. Will likely want to always
		// have duplicate packets being sent until an ack is recieved
		// and the flow object is modified appropriately in the 
		// "Ack_Receieve_Event" handler.
		Host * source = flow->get_source();
		Host * destination = flow->get_destination();
		printf("This flow is going from %s to %s\n\n",
			ip_to_english(&network, source).c_str(),
			ip_to_english(&network, destination).c_str() );
		Link * link = flow->get_source()->get_first_link();
		Data_packet * packet0 = new Data_packet(source, destination, 0, flow, global_time);
		Data_packet * packet1 = new Data_packet(source, destination, 1, flow, global_time);
		Data_packet * packet2 = new Data_packet(source, destination, 2, flow, global_time);
		// Always push packet to buffer before spawning send event
		if( link->add_to_buffer(packet0, (Node *) source) == 0) { 
			Link_Send_Event * event = 
				new Link_Send_Event(
					link->earliest_available_time(),
					SEND_EVENT_ID,
					link);
			event_queue.push(event);
		}

		// Always push packet to buffer before spawning send event
		if( link->add_to_buffer(packet1, (Node *) source) == 0) { 
			Link_Send_Event * event = 
				new Link_Send_Event(
					link->earliest_available_time(),
					SEND_EVENT_ID,
					link);
			event_queue.push(event);
		}

		// Always push packet to buffer before spawning send event
		if( link->add_to_buffer(packet2, (Node *) source) == 0) { 
			Link_Send_Event * event = 
				new Link_Send_Event(
					link->earliest_available_time(),
					SEND_EVENT_ID,
					link);
			event_queue.push(event);
		}
	}
	else if(event_ID == TCP_RENO) {
		Host * source = flow->get_source();
		Host * destination = flow->get_destination();
		printf("This flow is going from %s to %s\n\n",
			ip_to_english(&network, source).c_str(),
			ip_to_english(&network, destination).c_str() );
		Link * link = flow->get_source()->get_first_link();
		vector<Data_packet *> to_send = flow->send_packets();
		printf("%d\n", (int)to_send.size());
		for(int i = 0; i < to_send.size(); i++) {
			if( link->add_to_buffer(to_send[i], (Node *) source) == 0) { 
				Link_Send_Event * event = 
					new Link_Send_Event(
						link->earliest_available_time(),
						SEND_EVENT_ID,
						link);
				event_queue.push(event);
			}
		}
	}
	else if(event_ID == TCP_FAST) {
		//TODO
	}
	else {
		printf("Invalid event_ID: %d\n", event_ID);
		exit(-1);
	}
}

/////////////// Routing_Start_Event /////////////////
Routing_Event::Routing_Event(double start_, int event_ID_, Network * network_)
           : Event(start_, event_ID_) {
	network = network_;
}

void Routing_Event::handle_event() {
	//
	vector<Flow *> flows = network->all_flows;
	global_time = this->get_start();
	printf("Updating routing tables. Time: %f\n\n", global_time);

	// Pause all flows
	for (int i = 0; i < flows.size(); i++) {
		// TODO: Pausing
	}
	// For each link, calculate the cost and update the costs
	vector<Link *> links = network->all_links;
	for (int i = 0; i < links.size(); i++) {
		Link * link = links.at(i);
	}
	// For each router, update the distance vector and transmit routing packets
	vector<Router *> routers = network->all_routers;
	for (int i = 0; i < routers.size(); i++) {
		Router * router = routers.at(i);
		// Recompute distance vector with current costs
		router->init_distance_vector();
		// TODO: Transmit the distance vectors
		vector<Link *> adj_links = router->get_links();
		for (int j = 0; j < adj_links.size(); j++) {
			Link * link = adj_links.at(j);
		}
		// TODO: When the router receives a routing packet, update the routing table
	}

}

/////////////// Link_Send_Event /////////////////
Link_Send_Event::Link_Send_Event(double start_, int event_ID_, Link * link_)
           : Event(start_, event_ID_) {
	link = link_;
}

void Link_Send_Event::handle_event() {
	global_time = this->get_start();
	Node * endpoint1 = link->get_ep1();
	Node * endpoint2 = link->get_ep2();
	if (link->data_directions.front() == - 1) {
		Node * temp = endpoint1;
		endpoint1 = endpoint2;
		endpoint2 = temp;
	} 
	printf("Sending packet %d from %s to %s on link %s. Time: %f\n\n",
		link->data_buffer.front()->get_index(),
		ip_to_english(&network, endpoint1).c_str(),
		ip_to_english(&network, endpoint2).c_str(),
		link_to_english(&network, link).c_str(), global_time);
	link->transmit_packet();
}

/////////////// Link_Free_Event /////////////////
Link_Free_Event::Link_Free_Event(double start_, int event_ID_, Link * link_)
           : Event(start_, event_ID_) {
	link = link_;
}

void Link_Free_Event::handle_event() {
	global_time = this->get_start();
	link->is_free = 1;
	printf("Packet moved through Link %s. It is available again. Time: %f\n\n",
		link_to_english(&network, link).c_str(), global_time);
}

/////////////// Ack_Receive_Event (an ack was recieved by the source) /////////////////
Ack_Receive_Event::Ack_Receive_Event(double start_, int event_ID_, Ack_packet * ack_)
           : Event(start_, event_ID_) {
	ack = ack_;
}

void Ack_Receive_Event::handle_event() {
	global_time = this->get_start();
	printf(" ### Ack #%d recieved at host: %s at time: %f\n\n", 
		ack->get_index(),
		ip_to_english(&network, ack->getDest()).c_str(),
		global_time);
	ack->getFlow()->receive_ack(ack);
	delete ack;

}

/////////////// Data_Receive_Event (data was recieved by the dest) /////////////////
Data_Receive_Event::Data_Receive_Event(double start_, int event_ID_, Data_packet * data_)
           : Event(start_, event_ID_) {
	data = data_;
}

void Data_Receive_Event::handle_event() {
	global_time = this->get_start();
	printf(" $$$ Packet #%d recieved at host: %s at time: %f\n\n", 
			data->get_index(),
			ip_to_english(&network, data->getDest()).c_str(),
			global_time);
	// Create ack packet to send back to source
	Ack_packet * ack = new Ack_packet((Host *)data->getDest(),
									(Host *)data->getSource(),
									data->getFlow(),
									data->get_index(),
									data->get_time());
	Link * link_to_send_ack = ack->getSource()->get_first_link();
	
	// Always push packet to buffer before spawning send event
	if(link_to_send_ack->add_to_buffer(ack, ack->getSource()) == 0) {
		double start_time = link_to_send_ack->earliest_available_time();
		Link_Send_Event * event = new Link_Send_Event(
									start_time,
									SEND_EVENT_ID,
									link_to_send_ack);

		link_to_send_ack->t_free = start_time + link_to_send_ack->get_packet_delay(ack);
		event_queue.push(event);
	}
	delete data;
}

/////////////// Rout_Receive_Event (a rout packet was recieved) /////////////////
Rout_Receive_Event::Rout_Receive_Event(double start_, int event_ID_, Rout_packet * r_packet_)
           : Event(start_, event_ID_) {
	r_packet = r_packet_;
}

void Rout_Receive_Event::handle_event() {
	//global_time = this->get_start();
	//printf(" ### Ack #%d recieved at host: %s at time: %f\n\n", 
	//	ack->get_index(),
	//	ip_to_english(&network, ack->getDest()).c_str(),
	//	global_time);
	//delete ack;
	//Link * link = ack->getSource()->get_first_link();

	/* TODO: Update flow based on TCP_ID and send 1 or more
	 *  Link_Send_Events. Remember to check that packet could be added 
	 * to buffer i.e: if( link->add_to_buffer(packet) == 0) { ...
	 */

}

/////////////// Time out event (check if packet timed out) /////////////////
Time_Out_Event::Time_Out_Event(double start_, int event_ID_, Data_packet * data_)
           : Event(start_, event_ID_) {
	data = data_;	
}

void Time_Out_Event::handle_event() {
	if(data->getFlow()->received_packet(data->get_index())){
		data->getFlow()->handle_time_out();
	}
}
