#include "event.h"
#include "parser.h"
extern double global_time;
extern priority_queue<Event *, vector<Event *>, CompareEvents> event_queue;
extern priority_queue<Event *, vector<Event *>, CompareEvents> routing_queue;
extern double end_time;
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

void Event::change_start(double new_start) {
	start = new_start;
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

/////////////// Link_Send_Routing_Event /////////////////
Link_Send_Routing_Event::Link_Send_Routing_Event(double start_, int event_ID_, Link * link_)
           : Event(start_, event_ID_) {
	link = link_;
}
void Link_Send_Routing_Event::handle_event() {
	global_time = this->get_start();
	Node * endpoint1 = link->get_ep1();
	Node * endpoint2 = link->get_ep2();
	if (link->routing_directions.front() == - 1) {
		Node * temp = endpoint1;
		endpoint1 = endpoint2;
		endpoint2 = temp;
	} 
	printf("Routing: Sending packet %d from %s to %s on link %s. Time: %f\n\n",
		link->routing_buffer.front()->get_index(),
		ip_to_english(&network, endpoint1).c_str(),
		ip_to_english(&network, endpoint2).c_str(),
		link_to_english(&network, link).c_str(), global_time);
	link->transmit_packet_r();
}										

/////////////// Link_Free_Event /////////////////
Link_Free_Event::Link_Free_Event(double start_, int event_ID_, Link * link_)
           : Event(start_, event_ID_) {
	link = link_;
}

void Link_Free_Event::handle_event() {
	global_time = this->get_start();
	// Check if we are freeing for a routing send or a data send
	if(get_ID() == RFREE_EVENT_ID)
		link->is_free_r = 1;
	else
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
	// send new packets
	Host * source = ack->getFlow()->get_source();
	Link * link = source->get_first_link();
	vector<Data_packet *> to_send = ack->getFlow()->receive_ack(ack);
	for(int i = 0; i < to_send.size(); i++) {
		if(link->add_to_buffer(to_send[i], (Node *) source) == 0) { 
			Link_Send_Event * event = 
				new Link_Send_Event(
					link->earliest_available_time(),
					SEND_EVENT_ID,
					link);
			event_queue.push(event);
		}
	}
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
	data->getFlow()->receive_data(data);
	delete data;
}

/////////////// Rout_Receive_Event (a rout packet was recieved) /////////////////
Rout_Receive_Event::Rout_Receive_Event(Router * router_, double start_, int event_ID_, Rout_packet * r_packet_)
           : Event(start_, event_ID_) {
	r_packet = r_packet_;
	router = router_;
}

void Rout_Receive_Event::handle_event() {
	global_time = this->get_start();

	printf(" $$$ Packet #%d recieved at router: %s at time: %f\n\n", 
			r_packet->get_index(),
			ip_to_english(&network, r_packet->getDest()).c_str(),
			global_time);
	// Update the routers' distance vector and routing table
	router->receive_routing_packet(r_packet);
}

/////////////// Update_Rtables_Event /////////////////
Update_Rtables_Event::Update_Rtables_Event(double start_, int event_ID_, Network * network_)
           : Event(start_, event_ID_) {
	network = network_;
}
void Update_Rtables_Event::handle_event() {
	global_time = this->get_start();
	double t_0 = global_time;

	// See the event Rout_Receive_Event to adjust what happens when a rotuing
	// packet arrives at its intended destination.
	for (int i = 0; i < network->all_routers.size(); i++) {
		// For each router in the network, initialize the distance vector and
		// routing table to include information about its neighbors
		Router * router = network->all_routers.at(i);
		router->init_distance_vector();
		router->init_routing_table();
	}

	// Iterate N-1 times, where N = number of nodes in the network, to make 
	// sure that the routing table converges
	for (int i = 1; i < network->all_routers.size() + network->all_hosts.size(); i++) {
		// Each router sends its distance vector to all known nodes
		for (int j = 0; j < network->all_routers.size(); j++) {
			Router * router = network->all_routers.at(j);
			map<Node *, Node *> r_table = router->get_routing_table();
			// Create a routing packet and send it to each router in its routing table
			for (map<Node *, Node *>::iterator it = r_table.begin(); it != r_table.end(); it++) {
				Node * dest = it->first;
				// Send a routing packet only if the destination is a router
				if (dest->is_router()) {
					//
					Node * next_node = it->second;
					Rout_packet * r_packet = new Rout_packet(router, dest, 
											router->get_distance_vector());
					Link * link_to_send_r_packet = router->get_link(next_node);
					// Push the packet to the routing buffer
					if (link_to_send_r_packet->add_to_buffer_r(r_packet, router) == 0) {
						// If successfully added to buffer, create a send event
						Link_Send_Routing_Event * send_event = new Link_Send_Routing_Event(
											link_to_send_r_packet->earliest_available_time_r(),
											RSEND_EVENT_ID, link_to_send_r_packet);
						routing_queue.push(send_event);
					} 
				}
			}
		}
		
		// In each iteration, empty the routing queue of the events sending
		// and receiving the last iteration's distance vectors
		while((!routing_queue.empty()) && (global_time <= end_time)) {
			Event * to_handle = routing_queue.top();
			routing_queue.pop();
			to_handle->handle_event();
			delete to_handle;
		}
	}

	double t_f = global_time;
	double elapsed_time = t_f - t_0;
	// Increment the start times of all events in event_queue.
	queue <Event *> temp_queue;
	Event * temp_event;
	int num_events = event_queue.size();
	for(int i = 0; i < num_events; i++)
	{
		temp_event = event_queue.top();
		event_queue.pop();
		double new_start = temp_event->get_start() + elapsed_time;
		temp_event->change_start(new_start);
		temp_queue.push(temp_event);
	}
	// Refill event queue with the updated events
	for(int i = 0; i < num_events; i++)
	{
		temp_event = temp_queue.front();
		temp_queue.pop();
		event_queue.push(temp_event);
	}
	// Done.
}

/////////////// Time out event (check if packet timed out) /////////////////
Time_Out_Event::Time_Out_Event(double start_, int event_ID_, Data_packet * data_)
           : Event(start_, event_ID_) {
	data = data_;	
}

void Time_Out_Event::handle_event() {
	printf("Time out\n");
	if(data->getFlow()->received_packet(data->get_index())){
		data->getFlow()->handle_time_out();
	}
}
