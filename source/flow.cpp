#include "flow.hpp"
#include "parser.hpp"
extern Network network;
extern double global_time;
extern priority_queue<Event *, vector<Event *>, CompareEvents> event_queue;
extern int TCP_ID;

// Basic flow class/TCP implementation

Flow::Flow(Host * source_, Host * destination_, double data_size_, double start_) {
	source = source_;
	destination = destination_;
	size = data_size_;
	start = start_;
	window_size = 1;
	last_ack_received = 1;
	num_duplicates = 0;
	ss_threshold = 1000000;
	sent = 0;
	bytes_received = 0;
	last_bytes_received_query = 0;
	next_index = 1;
	last_flow_rate_query = start;
	done = false;
	
	to_receive = 1;
	fast_retransmit = true;
	fast_recovery = true;
	in_flight = 0;
	last_ack_time = 0;
	max_ack_received = 0;
	
	ca_wnd = 0;
	time_out = 1;
	b = 0.25;
	rtt_min = 1000000; 
	rtt_avg = 0;
	rtt_dev = 0;
	rtt = 0;
	sent_packets.push_back(0);
	alpha = 50;
}

// Sends packets
// Called whenever flow needs to send new packets
// E.g. ack receipt, flow time out, etc.
vector<Data_packet *> Flow::send_packets() {
	// duplicate: true if retransmitting one packet
	vector<Data_packet *> send_now;
	// mexPrintf("sending: %d window size: %f last_ack: %d\n", (int)sending.size(), window_size, last_ack_received);
	// always resend first packet
	mexPrintf("before: cwnd: %f inflight: %d\n", window_size, in_flight);
	// if(timed_out_packets.size() > 0){
	// 	int num = timed_out_packets.front();
	// 	mexPrintf("retransmitting %d\n", num);
	// 	timed_out_packets.pop_front();
	// 	in_flight--;
	// 	if(!acked_packet(num)){
	// 		Data_packet * retransmit = generate_packet(num);
	// 	    in_flight++;
	// 	    send_now.push_back(retransmit);
	// 	}
	// }
	// // try to send timed out packets first
	// while(in_flight < window_size and timed_out_packets.size() > 0 and !done){
	// 	int num = timed_out_packets.front();
	// 	timed_out_packets.pop_front();
	// 	if(!acked_packet(num)){
	// 		Data_packet * retransmit = generate_packet(num);
	// 	    in_flight++;
	// 	    send_now.push_back(retransmit);
	// 	}		
	// }
	while(sending.size() < window_size and !done){
		if(!acked_packet(next_index)){
			Data_packet * new_packet = generate_packet(next_index);
			send_now.push_back(new_packet);
			sending.push_back(next_index);
			in_flight = sending.size();
		}
		//timed_out_packets.erase(remove(timed_out_packets.begin(), timed_out_packets.end(), next_index), timed_out_packets.end());
		next_index++;
		
	}
	print_received();
	mexPrintf("after: cwnd: %f inflight: %d\n", window_size, in_flight);
	//mexPrintf("ss threshold: %d\n", ss_threshold);    
	return send_now;
}

// Handles data packet receipt 
void Flow::receive_data(Data_packet * packet) {
	if(packet->getSource() == source && packet->getDest() == destination){
		if(!received_packet(packet->get_index())){
			received.push_back(packet->get_index());
			sort(received.begin(), received.end());
			bytes_received += DATA_SIZE;
			if(bytes_received > size){
				done = true;
			}
		}	
		// update expected packet
		if(packet->get_index() == to_receive){
			to_receive++;
			// find next expected packet
			while(received_packet(to_receive)){
				to_receive++;
			}
		}
	}
	else{
		mexPrintf("Wrong packet received");
	}
}

// Handles ack packet receipt
void Flow::receive_ack(Ack_packet * packet) {
	int index = packet->get_index() - 1;
	sending.erase(remove_if(sending.begin(), sending.end(), [index](const int& x) {
		   	return x <= index; 
	   	}), sending.end());
	//mexPrintf("%f %f %f %f\n", rtt, rtt_avg, rtt_dev, time_out);
	// If duplicate ack, go back n
	if(TCP_ID == TCP_FAST){
	    
	}
	else if(packet->get_index() == last_ack_received){
		num_duplicates++;
		// fast recovery
		if(fast_retransmit && fast_recovery and num_duplicates == 3){
			// Link * link = source->get_first_link();
			// Data_packet * retransmit = generate_packet(last_ack_received);
			// if(link->add_to_buffer(retransmit, (Node *) source) == 0) {
			// 	Link_Send_Event * event = 
			// 		new Link_Send_Event(
			// 			link->earliest_available_time(),
			// 			link,
			// 			DATA_SIZE);
			// 	event_queue.push(event);
			// 	Time_Out_Event * timeout =
			// 		new Time_Out_Event(
			// 			global_time + time_out,
			// 			this,
			// 			last_ack_received);
			// 	event_queue.push(timeout);
			// }
			ss_threshold = window_size / 2;
			if(ss_threshold < 2){
				ss_threshold = 2;
			}				
			next_index = last_ack_received;
			// Send_New_Packets_Event * send =
			// 	new Send_New_Packets_Event(
			// 		global_time + RESEND_TIME,
			// 		this);
		    //if(!timed_out(last_ack_received)){
			//	timed_out_packets.push_back(last_ack_received);		
			//}
			sending.erase(remove(sending.begin(), sending.end(), last_ack_received), sending.end());
			in_flight = sending.size();
			// window inflation			
			window_size = ss_threshold + 3;

		}
		else if(fast_retransmit && fast_recovery and num_duplicates > 3){
			// if(window_size >= ss_threshold){
			// 	window_size += 1 / window_size;
			// }
			// else{
			// 	window_size++;					
			// }
			window_size++;
		}
				
	}	
	// Handle normally 
	else{
		if(!acked_packet(packet->get_index() - 1)){
			if(packet->get_index() > max_ack_received){
		  		max_ack_received = packet->get_index();
	   		}
			// recursively compute timeout value
			rtt = global_time - packet->get_time();
			if(rtt < rtt_min){
				rtt_min = rtt; 
			}
			// initialize rtt;
			if(last_ack_received == 0){
				rtt_avg = rtt;
				rtt_dev = rtt;		
			}
			else{
				rtt_avg = (1 - b) * rtt_avg + b * rtt;
				rtt_dev = (1 - b) * rtt_dev + b * abs(rtt -  rtt_avg);
				time_out = rtt_avg + 4 * rtt_dev;
				if(time_out < 1){
					time_out = 1; // avoids small time_out errors
				}
			}
			last_ack_received = packet->get_index();
			// window deflation
			if(fast_retransmit && fast_recovery && num_duplicates > 3){			
				window_size = ss_threshold;
			}
			num_duplicates = 0;
			// slow start
			if(window_size <= ss_threshold){
				window_size++;
			}
			// congestion avoidance
			else{
				window_size += 1 / window_size;
			}
		}
				  
    }
	last_ack_received = packet->get_index();
}

// // resets flow after initial unbounded slow start
// void Flow::reset_flow(){
// 	mexPrintf("RESET\n");
// 	next_index = last_ack_received;
// 	mexPrintf("%f\n", window_size);
// 	ss_threshold = in_flight / 2; //window_size / 4;
// 	if(ss_threshold < 2){
// 		ss_threshold = 2;
// 	}
// 	mexPrintf("ssthresh: %f\n", ss_threshold);
// 	window_size = 1;
// 	slow_start = true;
// 	//in_flight = 0;
// }

// Handles packet time out
void Flow::handle_time_out(int index){
	//if(!timed_out(index)){
	//	timed_out_packets.push_back(index);		
	//}
	ss_threshold = window_size / 2;
	if(ss_threshold < 2){
		ss_threshold = 2;
	}
	sending.erase(remove(sending.begin(), sending.end(), index), sending.end());
	in_flight = sending.size();
	next_index = last_ack_received;
	mexPrintf("ssthresh: %f\n", ss_threshold);
	window_size = 1;
}

// Check if packet was lost
bool Flow::timed_out(int n){
	for(int i = 0; i < timed_out_packets.size(); i++){
		if(timed_out_packets[i] == n){
			return true;
		}
	}
	return false;	
}

// CHeck if packet has been acked
bool Flow::acked_packet(int n){
    if(max_ack_received > n){
		return true;
	}
	return false;	
}

// Check if packet has been sent
bool Flow::sent_packet(int n) {
	for(int i = 0; i < sent_packets.size(); i++){
		if(sent_packets[i] == n){
			return true;
		}
	}
	return false;
}

// Check if packet has been received
bool Flow::received_packet(int n) {
	for(int i = 0; i < received.size(); i++){
		if(received[i] == n){
			return true;
		}
	}
	return false;
}

// Generates a data packet
Data_packet * Flow::generate_packet(int n) {
	Data_packet * packet = new Data_packet(source, destination, n, this, global_time);
	return packet; 

}

// Generates an ack packet
Ack_packet * Flow::generate_ack_packet() {
	Ack_packet * packet = new Ack_packet(source, destination, this, to_receive, global_time);
	return packet; 
}

double Flow::get_flowrate() {
	double t0 = last_flow_rate_query;
	double tf = global_time;
	double bytes0 = last_bytes_received_query;
	double bytesf = bytes_received;
	double bytes = bytesf - bytes0;
	last_flow_rate_query = global_time;
	last_bytes_received_query = bytes_received;
	return bytes / (tf - t0);
	
}

void Flow::print_received(){
	mexPrintf("sending: ");
	for(int i = 0; i < sending.size(); i++){
		mexPrintf("%d ", sending[i]);
	}
	mexPrintf("\n");
}

Host * Flow::get_source() {
	return source;
}

Host * Flow::get_destination()
 {
	return destination;
}

double Flow::get_start() {
	return start;
}
