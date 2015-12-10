#include "flow.h"
#include "parser.h"
extern Network network;
extern double global_time;
extern int TCP_ID;

// Basic flow class/TCP implementation

Flow::Flow(Host * source_, Host * destination_, double data_size_, double start_) {
	source = source_;
	destination = destination_;
	size = data_size_;
	start = start_;
	algorithm = 1;
	window_size = 2;
	last_ack_received = 1;
	num_duplicates = 0;
	ss_threshold = 0;
	sent = 0;
	bytes_received = 0;
	last_bytes_received_query = 0;
	next_index = 1;
	last_flow_rate_query = start;
	done = false;
	
	to_receive = 1;
	slow_start = true;
	fast_retransmit = true;
	fast_recovery = true;
	in_flight = 0;
	last_ack_time = 0;

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
vector<Data_packet *> Flow::send_packets(bool duplicate) {
	// duplicate: true if retransmitting one packet
	vector<Data_packet *> send_now;
	// mexPrintf("sending: %d window size: %f last_ack: %d\n", (int)sending.size(), window_size, last_ack_received);
	// print_sending();
	if(duplicate){
		Data_packet * retransmit = generate_packet(last_ack_received);
		in_flight++;
		send_now.push_back(retransmit);
	}
	else{
		while(in_flight < window_size and !done){
			if(!acked_packet(next_index)){
				Data_packet * new_packet = generate_packet(next_index);
				send_now.push_back(new_packet);
				in_flight++;
			}
			next_index++;
		}
	}
	
	mexPrintf("window_size: %f\n", window_size);
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
		in_flight--;
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
vector<Data_packet *> Flow::receive_ack(Ack_packet * packet) {
	acked_packets.push_back(packet->get_index() - 1);
	vector<Data_packet *> send_now;
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
	time_out = 2; // FIX THIS LATER
	//mexPrintf("%f %f %f %f\n", rtt, rtt_avg, rtt_dev, time_out);
	// If duplicate ack, go back n
	if(TCP_ID == TCP_FAST){
		send_now = send_packets(false);
	}
	else if(packet->get_index() == last_ack_received){
		num_duplicates++;
		// fast recovery
		if(fast_retransmit && fast_recovery && num_duplicates == 3){
			if(!lost_packet(packet->get_index() - 1)){
				lost_packets.push_back(packet->get_index() - 1);	
				slow_start = false;
				ss_threshold = in_flight / 2;			
				if(ss_threshold < 2){
					ss_threshold = 2;
				}				
				// window inflation
				send_now = send_packets(true);
				window_size = ss_threshold + 3;				
			}									
		}
		else if(fast_retransmit && fast_recovery && num_duplicates > 3){
			window_size += 1;
			send_now = send_packets(false);
		}
	}	
	// Handle normally 
	else{		
		last_ack_received = packet->get_index();
		// window deflation
		if(fast_retransmit && fast_recovery && num_duplicates > 3){
			num_duplicates = 0;
			window_size = ss_threshold;
		}

		// slow start
		if(slow_start){
			window_size++;
			if(window_size == ss_threshold){
				slow_start = false;
			}
			send_now = send_packets(false);
		}
		// congestion avoidance
		else{
			window_size += 1 / (double)(int)window_size;
			send_now = send_packets(false);
		}		  
    }
	last_ack_received = packet->get_index();
	return send_now;
}

// Handles flow time out
vector<Data_packet *> Flow::handle_time_out(){
	ss_threshold = window_size / 2;
	window_size = 1;
	slow_start = true;
	in_flight = 0;
	next_index = last_ack_received;
    return send_packets(false);
}

// Check if packet was lost
bool Flow::lost_packet(int n){
	for(int i = 0; i < lost_packets.size(); i++){
		if(lost_packets[i] == n){
			return true;
		}
	}
	return false;	
}

// CHeck if packet has been acked
bool Flow::acked_packet(int n){
	for(int i = 0; i < acked_packets.size(); i++){
		if(acked_packets[i] == n){
			return true;
		}
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
	mexPrintf("received: ");
	for(int i = 0; i < received.size(); i++){
		mexPrintf("%d ", received[i]);
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
