#include "event.h"
#include "parser.h"
#include <assert.h>
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
		 printf("This flow is going from %s to %s. Time (ms): %f\n\n",
		 	ip_to_english(&network, source).c_str(),
		 	ip_to_english(&network, destination).c_str(), get_start() * 1000.0);
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


/////////////// Link_Drop_Event /////////////////
Link_Drop_Event::Link_Send_Event(double start_, int event_ID_, Link * link_)
           : Event(start_, event_ID_) {
	link = link_;
}

void Link_Drop_Event::handle_event() {
	to_send[i]->getFlow()->sending.pop_back(); // not exact but works
	to_send[i]->getFlow()->sent_packets.pop_back();
	to_send[i]->getFlow()->sent -= DATA_SIZE;
	to_send[i]->getFlow()->next_index--;
}   


/////////////// Link_Send_Event /////////////////
Link_Send_Event::Link_Send_Event(double start_, int event_ID_, Link * link_)
           : Event(start_, event_ID_) {
	link = link_;
}

void Link_Send_Event::handle_event() {
	global_time = this->get_start();
	// only send packet if not yet acked
	int ind = link->data_buffer.front()->get_index();
	bool is_ack = (link->data_buffer.front()->getId() == ACK_ID);
	if(is_ack || ind >= link->data_buffer.front()->getFlow()->last_ack_received){
		Node * endpoint1 = link->get_ep1();
		Node * endpoint2 = link->get_ep2();
		if (link->data_directions.front() == - 1) {
			Node * temp = endpoint1;
			endpoint1 = endpoint2;
			endpoint2 = temp;
		} 
		/* printf("Sending packet %d from %s to %s on link %s. Time: %.20f\n\n",
		   link->data_buffer.front()->get_index(),
		   ip_to_english(&network, endpoint1).c_str(),
		   ip_to_english(&network, endpoint2).c_str(),
		   link_to_english(&network, link).c_str(), global_time);*/
		link->transmit_packet();
	}
	else{
		link->discard_packet();
	}
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

	printf("ROUTING: Sending packet %d from %s to %s on link %s. Time (ms): %f\n\n",
	 	link->routing_buffer.front()->get_index(),
	 	ip_to_english(&network, endpoint1).c_str(),
	 	ip_to_english(&network, endpoint2).c_str(),
	 	link_to_english(&network, link).c_str(), global_time * 1000.0);
	link->transmit_packet_r();
}										

/////////////// Link_Free_Event /////////////////
Link_Free_Event::Link_Free_Event(double start_, int event_ID_, Link * link_, int direction_)
           : Event(start_, event_ID_) {
	link = link_;
	direction = direction_;
}

void Link_Free_Event::handle_event() {
	global_time = this->get_start();
	// Check if we are freeing for a routing send or a data send
	if(get_ID() == RFREE_EVENT_ID) {
		if(direction == 1) {link->is_free_forward_r = 1;}
		else {link->is_free_reverse_r = 1;}
	}
	else {
		if(direction == 1) {link->is_free_forward = 1;}
		else {link->is_free_reverse = 1;}
	}
	printf("Packet cleared from buffer on Link %s. Time (ms): %f\n\n",
		link_to_english(&network, link).c_str(), global_time * 1000.0);
}

/////////////// Ack_Receive_Event (an ack was recieved by the source) /////////////////
Ack_Receive_Event::Ack_Receive_Event(double start_, int event_ID_, Ack_packet * ack_)
           : Event(start_, event_ID_) {
	ack = ack_;
}

void Ack_Receive_Event::handle_event() {
	global_time = this->get_start();
	 printf(" ### Ack #%d received at host: %s at time (ms): %f\n\n", 
	 	ack->get_index(),
	 	ip_to_english(&network, ack->getDest()).c_str(),
	 	global_time * 1000.0);
	 // send new packets
	Host * source = ack->getFlow()->get_source();
	Link * link = source->get_first_link();
	if(ack->getFlow()->sending.size() >= 0){
		vector<Data_packet *> to_send = ack->getFlow()->receive_ack(ack);
		for(int i = 0; i < to_send.size(); i++) {
			if(link->add_to_buffer(to_send[i], (Node *) source) == 0) { 
				Link_Send_Event * event = 
					new Link_Send_Event(
						link->earliest_available_time(),
						SEND_EVENT_ID,
						link);
				Time_Out_Event * timeout =
					new Time_Out_Event(
						link->earliest_available_time() + to_send[i]->getFlow()->time_out,
						TIMEOUT_EVENT_ID,
						to_send[i]->getFlow(),
						to_send[i]->get_index());
				event_queue.push(event);
				event_queue.push(timeout);
			}
			else{
				// packet was dropped so get rid of it after all acks arrive
				Link_Drop_Event * event = 
					new Link_Drop_Event(
						link->earliest_available_time(),
						DROP_EVENT_ID,
						link);
			    event_queue.push(event);
			}
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
	printf(" $$$ Packet #%d received at host: %s at time (ms): %f\n\n", 
	 		data->get_index(),
	 		ip_to_english(&network, data->getDest()).c_str(),
	 		global_time * 1000.0);
	data->getFlow()->receive_data(data);
	 // Create ack packet to send back to source
	Ack_packet * ack = new Ack_packet((Host *)data->getDest(),
									(Host *)data->getSource(),
									data->getFlow(),
									data->getFlow()->to_receive,
									data->get_time());
	Link * link_to_send_ack = ack->getSource()->get_first_link();
	// Always push packet to buffer before spawning send event
	if(link_to_send_ack->add_to_buffer(ack, ack->getSource()) == 0) {
		Link_Send_Event * event = new Link_Send_Event(
									link_to_send_ack->earliest_available_time(),
									SEND_EVENT_ID,
									link_to_send_ack);
		event_queue.push(event);
	}	
	delete data;
}

/////////////// Packet_Receive_Event (packet was recieved by a router) /////////////////
Packet_Receive_Event::Packet_Receive_Event(double start_, int event_ID_, Packet * packet_, Link * link_, Node * src_)
           : Event(start_, event_ID_) {
	packet = packet_;
	link = link_;
	src = src_;
}

void Packet_Receive_Event::handle_event() {
	global_time = this->get_start();
	//printf(" Packet #%d received at the intended router at time (ms): %f\n\n", 
	// 		packet->get_index(),
	// 		global_time * 1000.0);
	// A packet was received during a routing table update (so we want to use routing buffer)
	if(get_ID() == RSEND_EVENT_ID) {
		if (link->add_to_buffer_r(packet, src) == 0) {
			// If successfully added to buffer, create a routing send event (uses routing buffer)
			Link_Send_Routing_Event * send_event = new Link_Send_Routing_Event(
								link->earliest_available_time_r(),
								RSEND_EVENT_ID, link);
			routing_queue.push(send_event);
		} 	
	}
	// A packet was received during normal flow progression (so we want to use data buffer)
	else {
		if( link->add_to_buffer(packet, src) == 0)
		{ 
			// A packet was received during normal flow progression (so we want to use data buffer)
			Link_Send_Event * send_event = new Link_Send_Event(
									link->earliest_available_time(),
									SEND_EVENT_ID,
									link);
			event_queue.push(send_event);
		}
	}
}

/////////////// Rout_Receive_Event (a rout packet was recieved) /////////////////
Rout_Receive_Event::Rout_Receive_Event(Router * router_, double start_, int event_ID_, Rout_packet * r_packet_)
           : Event(start_, event_ID_) {
	r_packet = r_packet_;
	router = router_;
}

void Rout_Receive_Event::handle_event() {
	global_time = this->get_start();
	 printf(" ROUTING: $$$ Packet #%d received at host: %s at time (ms): %f\n\n", 
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
	cout << "Update Rtables Event" << "\n";
	global_time = this->get_start();
	double t_0 = global_time;
		while((!routing_queue.empty()) && (global_time <= end_time)) {
			Event * to_handle = routing_queue.top();
			routing_queue.pop();
			to_handle->handle_event();
			delete to_handle;
		}

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
			// In each iteration, empty the routing queue of the events sending
		// and receiving the last iteration's distance vectors
		while((!routing_queue.empty()) && (global_time <= end_time)) {
			Event * to_handle = routing_queue.top();
			routing_queue.pop();
			to_handle->handle_event();
			delete to_handle;
		}

	double t_f = global_time;
	double elapsed_time = t_f - t_0;
	// Increment the start times of all events in event_queue.
	queue <Event *> temp_queue;
	Event * temp_event;
	int num_events = routing_queue.size();
	for(int i = 0; i < num_events; i++)
	{
		temp_event = routing_queue.top();
		routing_queue.pop();
		double new_start = temp_event->get_start() + elapsed_time;
		temp_event->change_start(new_start);
		temp_queue.push(temp_event);
	}
	// Refill event queue with the updated events
	for(int i = 0; i < num_events; i++)
	{
		temp_event = temp_queue.front();
		temp_queue.pop();
		routing_queue.push(temp_event);
	}
	// Done.
	for (int i = 0; i < network->all_routers.size(); i++) {
		Router * router_ = network->all_routers.at(i);

		// Print out the updated routing table
		//router_->print_distance_vector();
		router_->print_routing_table();
	}	
	cout << "Done with Update Rtables Event" << "\n";
}

/////////////// Time out event (check if packet timed out) /////////////////
Time_Out_Event::Time_Out_Event(double start_, int event_ID_, Flow * flow_, int n)
           : Event(start_, event_ID_) {
	flow = flow_;
	index = n;
}

void Time_Out_Event::handle_event() {
	if(!flow->received_packet(index)){
		printf("Time out\n");
		flow->handle_time_out();
	}
}
