/**
    CS-143 Network Simulator
    flow.cpp
    Purpose: Contains implementations of TCP RENO and FAST TCP for 
    use in transmitting packets between 2 hosts in the network.

    @author Vincent Zhuang
    @version 1.0 12/11/15
*/

#include "flow.hpp"
#include "parser.hpp"
extern Network network;
extern double global_time;
extern priority_queue<Event *, vector<Event *>, CompareEvents> event_queue;
extern int TCP_ID;

// Flow class/TCP implementation
Flow::Flow(Host * source_, Host * destination_, double data_size_, double start_) {
	source = source_;
	destination = destination_;
	size = data_size_;
	start = start_;
	
	window_size = 1;		
	ss_threshold = 1000;
	bytes_sent = 0;
	last_bytes_sent_query = 0;
	next_index = 1;
	last_flow_rate_query = start;
	done = false;

	num_duplicates = 0;
	last_ack_received = 1;
	to_receive = 1;
	fast_retransmit = true;
	fast_recovery = true;
	in_flight = 0;
	max_ack_received = 0;
	
	time_out = 1;
	b = 0.25;
	rtt_min = 1; 
	rtt_avg = 0;
	rtt_dev = 0;
	rtt = 0; 
	alpha = 25;
	gamma = 0.8;
}

// Sends packets
// Called whenever flow needs to send new packets
// E.g. ack receipt, flow time out, etc.
vector<Data_packet *> Flow::send_packets() {
	// duplicate: true if retransmitting one packet
	vector<Data_packet *> send_now;
	mexPrintf("before: cwnd: %f inflight: %d\n", window_size, in_flight);
	while(sending.size() < window_size and !done){
		if(!acked_packet(next_index)){
			Data_packet * new_packet = generate_packet(next_index);
			send_now.push_back(new_packet);
			sending.push_back(next_index);
			in_flight = sending.size();
			bytes_sent += DATA_SIZE;
		}
		if(next_index <= size / DATA_SIZE){
			next_index++;
		}		
	}
	mexPrintf("after: cwnd: %f inflight: %d\n", window_size, in_flight);
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
				end();
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

// resets variables when flow ends:
void Flow::end() {
	rtt = 0;
	window_size = 0;
}

// Handles ack packet receipt
void Flow::receive_ack(Ack_packet * packet) {
	int index = packet->get_index() - 1;
	sending.erase(remove_if(sending.begin(), sending.end(), [index](const int& x) {
		   	return x <= index; 
	   	}), sending.end());
	// recursively compute timeout value
    if(global_time - packet->get_time() < time_out and !done){
		rtt = global_time - packet->get_time();
	}
	mexPrintf("new rtt: %f, rtt_avg: %f rtt_dev: %f\n", rtt, rtt_avg, rtt_dev);
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
	//mexPrintf("%f %f %f %f\n", rtt, rtt_avg, rtt_dev, time_out);
	// If duplicate ack, go back n
	if(TCP_ID == TCP_FAST){
		// assume loss after one duplicate ack
		if(packet->get_index() > max_ack_received){
			max_ack_received = packet->get_index();
	   	}
	}
	else if(packet->get_index() == last_ack_received){
		num_duplicates++;
		// fast recovery
		if(fast_retransmit && fast_recovery and num_duplicates == 3){
			ss_threshold = window_size / 2;
			if(ss_threshold < 2){
				ss_threshold = 2;
			}				
			next_index = last_ack_received;
			sending.erase(remove(sending.begin(), sending.end(), last_ack_received), sending.end());
			in_flight = sending.size();
			// window inflation			
			window_size = ss_threshold + 3;
		}
		else if(fast_retransmit && fast_recovery and num_duplicates > 3){
			window_size++;
		}				
	}	
	// Handle normally 
	else{
		if(!acked_packet(packet->get_index() - 1)){
			if(packet->get_index() > max_ack_received){
		  		max_ack_received = packet->get_index();
	   		}			
			// window deflation
			if(fast_retransmit && fast_recovery && num_duplicates > 3){		   
				window_size = ss_threshold;
			}
			num_duplicates = 0;
			update_window();
		}				  
    }
	last_ack_received = packet->get_index();
}

// Updates window size
void Flow::update_window(){
	if(TCP_ID == TCP_FAST){
		if(rtt > 0){
			window_size = min(2 * window_size,
						  gamma * (rtt_min * window_size / rtt + alpha) + (1 - gamma) * window_size);
		}
		else{
			window_size *= 2;
		}
		
	}
	else if(TCP_ID == TCP_TAHOE or TCP_ID == TCP_RENO){
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

// Handles packet time out
void Flow::handle_time_out(int index){
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

// Check if packet has been acked
bool Flow::acked_packet(int n){
    if(max_ack_received > n){
		return true;
	}
	return false;	
}

// Check if packet has been already been received at destination
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

// computes flowrate by rate of amount of non-unique bytes being sent
double Flow::get_flowrate() {
	double t0 = last_flow_rate_query;
	double tf = global_time;
	double bytes0 = last_bytes_sent_query;
	double bytesf = bytes_sent;
	double bytes = bytesf - bytes0;
	last_flow_rate_query = global_time;
	last_bytes_sent_query = bytes_sent;
	return bytes / (tf - t0);	
}

// Getter for source
Host * Flow::get_source() {
	return source;
}

// Getter fpr destination
Host * Flow::get_destination()
 {
	return destination;
}

// Getter for start time
double Flow::get_start() {
	return start;
}
