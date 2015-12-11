#include "event.h"
#include "parser.h"
#include <assert.h>
extern double global_time;
extern priority_queue<Event *, vector<Event *>, CompareEvents> event_queue;
extern priority_queue<Event *, vector<Event *>, CompareEvents> routing_queue;
extern double end_time;
extern int TCP_ID;
extern Network network;

//////////////////////// GENERIC EVENT SUPERCLASS /////////////////////////
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

/////////////////////////// FLOW EVENTS ///////////////////////////////

// Flow start event
Flow_Start_Event::Flow_Start_Event(double start_, int event_ID_, Flow * flow_)
           : Event(start_, event_ID_) {
	flow = flow_;
}

void Flow_Start_Event::handle_event() {
	global_time = this->get_start();
	int event_ID = this->get_ID();
	if(event_ID == TCP_TAHOE) {
		flow->fast_recovery = false;	   
	}
	else if(event_ID == TCP_RENO) {
		flow->fast_retransmit = true;
		flow->fast_recovery = true;
	}
	else if(event_ID == TCP_FAST) {
		flow->fast_retransmit = false;
		flow->fast_recovery = false;
	    Fast_Update_Event * fast_update =
			new Fast_Update_Event(
				1 + FAST_DELAY,
			    FAST_UPDATE_ID,
				flow);
		event_queue.push(fast_update);
	}
	else {
		mexPrintf("Invalid event_ID: %d\n", event_ID);
		mexErrMsgTxt("");
	}
	Host * source = flow->get_source();
	Host * destination = flow->get_destination();
	mexPrintf("This flow is going from %s to %s. Time (ms): %f\n\n",
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
					link,
					DATA_SIZE);
			event_queue.push(event);
		}
	}
}

// Ack receive event
Ack_Receive_Event::Ack_Receive_Event(double start_, int event_ID_, Ack_packet * ack_)
           : Event(start_, event_ID_) {
	ack = ack_;
}

void Ack_Receive_Event::handle_event() {
	global_time = this->get_start();
	ack->getFlow()->last_ack_time = global_time;
	mexPrintf(" ### Ack #%d received at host: %s at time (ms): %f\n\n", 
	   ack->get_index(),
	   ip_to_english(&network, ack->getDest()).c_str(),
	   global_time * 1000.0);
	// send new packets
	Host * source = ack->getFlow()->get_source();
	Link * link = source->get_first_link();
	vector<Data_packet *> to_send = ack->getFlow()->receive_ack(ack);
	//if(ack->getFlow()->sending.size() <= ack->getFlow()->window_size){
	for(int i = 0; i < to_send.size(); i++) {
		//mexPrintf("Attempting to send packet %d\n", to_send[i]->get_index());
		if(link->add_to_buffer(to_send[i], (Node *) source) == 0) {
			//mexPrintf("Packet %d will be sent\n", to_send[i]->get_index());
			Link_Send_Event * event = 
				new Link_Send_Event(
					link->earliest_available_time(),
					SEND_EVENT_ID,
					link,
					DATA_SIZE);

			event_queue.push(event);
		}		
        Time_Out_Event * timeout =
	        new Time_Out_Event(
			    global_time + ack->getFlow()->time_out,
			    TIMEOUT_EVENT_ID,
			    ack->getFlow(),
				to_send[i]->get_index());
	    event_queue.push(timeout);
	}
	delete ack;		
}

// Data receive event
Data_Receive_Event::Data_Receive_Event(double start_, int event_ID_, Data_packet * data_)
           : Event(start_, event_ID_) {
	data = data_;
}

void Data_Receive_Event::handle_event() {
	global_time = this->get_start();
	mexPrintf(" $$$ Packet #%d received at host: %s at time (ms): %f\n\n", 
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
	if(link_to_send_ack->add_to_buffer(ack, ack->getSource()) == 0) {
		Link_Send_Event * event = new Link_Send_Event(
									link_to_send_ack->earliest_available_time(),
									SEND_EVENT_ID,
									link_to_send_ack,
									ACK_SIZE);
		event_queue.push(event);
	}	
	delete data;
}

// Packet receive event
// occurs when packet is received by a router
Packet_Receive_Event::Packet_Receive_Event(double start_, int event_ID_, Packet * packet_, Link * link_, Node * src_)
           : Event(start_, event_ID_) {
	packet = packet_;
	link = link_;
	src = src_;
}

void Packet_Receive_Event::handle_event() {
	global_time = this->get_start();
	//mexPrintf(" Packet #%d received at the intended router at time (ms): %f\n\n", 
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
									link,
									packet->packetSize());
			event_queue.push(send_event);
		}
	}
}

// Time out event
// Occurs when no packet gets sent/received for time_out time
Time_Out_Event::Time_Out_Event(double start_, int event_ID_, Flow * flow_, int index_)
           : Event(start_, event_ID_) {
	flow = flow_;
	index = index_;
}

void Time_Out_Event::handle_event() {
	global_time = this->get_start();
	if(!flow->acked_packet(index)){
		global_time = this->get_start();
		mexPrintf("global_time %f\n", global_time);
		mexPrintf("Packet %d timed out\n", index);
		Host * source = flow->get_source();
		Link * link = source->get_first_link();
		vector<Data_packet *> to_send = flow->handle_time_out(index);
		for(int i = 0; i < to_send.size(); i++) {
			if(link->add_to_buffer(to_send[i], (Node *) source) == 0) {
				Link_Send_Event * event = 
					new Link_Send_Event(
						link->earliest_available_time(),
						SEND_EVENT_ID,
						link,
						DATA_SIZE);
				event_queue.push(event);
			}	   
			Time_Out_Event * timeout =
				new Time_Out_Event(
					global_time + flow->time_out,
					TIMEOUT_EVENT_ID,
				    to_send[i]->getFlow(),
					to_send[i]->get_index());
			event_queue.push(timeout);

		}
		//flow->print_received();
	}	
}

// Fast update event
// Updates window size for fast TCP
Fast_Update_Event::Fast_Update_Event(double start_, int event_ID_, Flow * flow_)
	    : Event(start_, event_ID_) {
	flow = flow_;
}

void Fast_Update_Event::handle_event() {
	global_time = this->get_start();
	if(!flow->done){
		Fast_Update_Event * fast_update =
			new Fast_Update_Event(
				global_time + FAST_DELAY,
			    FAST_UPDATE_ID,
				flow);
		event_queue.push(fast_update);
	}
	mexPrintf("Fast TCP window size updated to %f\n", flow->window_size);
	// adjust window size
	if(flow->rtt > 0){
		flow->window_size = min(2 * flow->window_size, flow->rtt_min * flow->window_size / flow->rtt + flow->alpha);
	}	
}

////////////////////////////// LINK EVENTS ////////////////////////////////////

// Link send event

Link_Send_Event::Link_Send_Event(double start_, int event_ID_, Link * link_, double packetsize)
           : Event(start_, event_ID_) {
	link = link_;
	// Create an event to free the link at the same time that the packet
	// successfully transmits. We achieve this using epsilon.
	double time_to_send = packetsize / link->get_capacity();
	Link_Free_Event * free_event = 
		new Link_Free_Event(
			start_ + time_to_send - EPSILON,
			LINK_FREE_ID,
			link_,
			-1);
	event_queue.push(free_event);
	
}

void Link_Send_Event::handle_event() {
	global_time = this->get_start();
	int ind = link->data_buffer.front()->get_index();
	bool is_ack = (link->data_buffer.front()->getId() == ACK_ID);
	//mexPrintf("Link sending %d %d\n", ind,  link->data_buffer.front()->getFlow()->last_ack_received);
	Node * endpoint1 = link->get_ep1();
	Node * endpoint2 = link->get_ep2();
	if (link->data_directions.front() == - 1) {
		Node * temp = endpoint1;
		endpoint1 = endpoint2;
		endpoint2 = temp;
	} 
	mexPrintf("Sending packet %d from %s to %s on link %s. Time (ms): %f\n\n",
	   link->data_buffer.front()->get_index(),
	   ip_to_english(&network, endpoint1).c_str(),
	   ip_to_english(&network, endpoint2).c_str(),
	   link_to_english(&network, link).c_str(), global_time * 1000.0);
	link->transmit_packet();
}

// Link send routing event
Link_Send_Routing_Event::Link_Send_Routing_Event(double start_, int event_ID_, Link * link_)
           : Event(start_, event_ID_) {
	link = link_;
	// Create an event to free the link at the same time that the packet
	// successfully transmits. We achieve this using epsilon.
	double time_to_send = ROUTING_SIZE / link_->get_capacity();
	Link_Free_Event * free_event = 
		new Link_Free_Event(
			time_to_send + start_ - EPSILON,
			RFREE_EVENT_ID,
			link_,
			-1);
	routing_queue.push(free_event);
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

	mexPrintf("ROUTING: Sending packet %d from %s to %s on link %s. Time (ms): %f\n\n",
	 	link->routing_buffer.front()->get_index(),
	 	ip_to_english(&network, endpoint1).c_str(),
	 	ip_to_english(&network, endpoint2).c_str(),
	 	link_to_english(&network, link).c_str(), global_time * 1000.0);
	link->transmit_packet_r();
}										

// Link free event
Link_Free_Event::Link_Free_Event(double start_, int event_ID_, Link * link_, int direction_)
           : Event(start_, event_ID_) {
	link = link_;
	direction = direction_;
}

void Link_Free_Event::handle_event() {
	global_time = this->get_start();
	// Check if we are freeing for a routing send or a data send
	if(get_ID() == RFREE_EVENT_ID) {
		link->is_free_r = 1;
			mexPrintf("Routing: Packet cleared from buffer on Link %s. Time (ms): %f\n\n",
		link_to_english(&network, link).c_str(), global_time * 1000.0);
	}
	else {
		link->is_free = 1;
			mexPrintf("Packet cleared from buffer on Link %s. Time (ms): %f\n\n",
		link_to_english(&network, link).c_str(), global_time * 1000.0);
	}

}

////////////////////////////// ROUTING EVENTS /////////////////////////////////

// Rout receive event
// Routing packet was received
Rout_Receive_Event::Rout_Receive_Event(Router * router_, double start_, int event_ID_, Rout_packet * r_packet_)
           : Event(start_, event_ID_) {
	r_packet = r_packet_;
	router = router_;
}

void Rout_Receive_Event::handle_event() {
	global_time = this->get_start();
	 mexPrintf(" ROUTING: $$$ Packet #%d received at host: %s at time (ms): %f\n\n", 
	 		r_packet->get_index(),
	 		ip_to_english(&network, r_packet->getDest()).c_str(),
	 		global_time);
	// Update the routers' distance vector and routing table
	router->receive_routing_packet(r_packet);
}

// Update rtables event
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
	// Allows isntantaneous routing table updates.
	global_time = t_0;

	cout << "Done with Update Rtables Event" << "\n";
}
