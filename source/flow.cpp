#include "flow.h"
#include "parser.h"
extern Network network;
extern double global_time;

/**
 *  Basic flow class/TCP implementation
 */

Flow::Flow(Host * source_, Host * destination_, double data_size_, double start_) {
	source = source_;
	destination = destination_;
	size = data_size_;
	start = start_;
	algorithm = 1;
	window_size = 2;
	last_ack_received = -1;
	num_duplicates = 0;
	ss_threshold = 0;
	sent = 0;
	
	to_receive = 1;
	slow_start = true;
	fast_retransmit = true;
	fast_recovery = true;
	last_time_out = global_time;

	time_out = 100;
	b = 0.25;
	rtt_avg = 0;
	rtt_dev = 0;
	//round_trip_times(size, 0);
}

/**
 * TO DO: make sure packets are removed from sending in events
 * I.e. in general make sure sending contains list of packets pushed onto queue
 * but not yet sent.
 */ 

vector<Data_packet *> Flow::send_packets() {
	vector<Data_packet *> send_now;
	//mexPrintf("%d %f %d %d\n", (int)sending.size(), window_size, window_start, size);
	while(sending.size() < (int) window_size and sent <= size){
		int next_index;
		if(sending.size() == 0){
			next_index = to_receive;
		}
		else{
			next_index = sending.back() + 1;
		}
		if(next_index >= size){
			break;
		}
		sending.push_back(next_index);
		Data_packet * new_packet = generate_packet(next_index);
		sent += new_packet->packetSize();
		send_now.push_back(new_packet);
	}
	return send_now;
}

// void Flow::send_data_packet(Data_packet * packet) {
// 	Link * link = source->get_first_link();
// 	if(link->add_to_buffer(packet) == 0){
// 		Link_Send_Event * event = new Link_Send_Event(start, SEND_EVENT_ID, link);
// 		event_queue.push(event);
// 	}		
// }

void Flow::send_ack_packet(Ack_packet * packet) {
	
}

/**
 * Handles data packet receipt 
 */
void Flow::receive_data(Data_packet * packet) {
	if(packet->getSource() == source && packet->getDest() == destination){
		if(sending.size() > 0){
			sending.erase(sending.begin());
		}
		//mexPrintf("sending: %d\n", (int) sending.size());
		//mexPrintf("to_receive: %d\n", to_receive);
		received.push_back(packet->get_index());
		// update expected packet
		if(packet->get_index() == to_receive){
			last_ack_received = packet->get_index();
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

/**
 * TODO: create and push timeout events
 */ 

vector<Data_packet *> Flow::receive_ack(Ack_packet * packet) {
	vector<Data_packet *> send_now;
	// recursively compute timeout value
	double rtt = global_time - packet->get_time();
	if(last_ack_received == -1){
		rtt_avg = rtt;
		rtt_dev = rtt;		
	}
	else{
		rtt_avg = (1 - b) * rtt_avg + b * rtt;
		rtt_dev = (1 - b) * rtt_dev + b * abs(rtt -  rtt_avg);
		time_out = rtt_avg + 4 * rtt_dev;
	}
	//mexPrintf("%f %f %f %f\n", rtt, rtt_avg, rtt_dev, time_out);
	// If duplicate ack, go back n
	if(packet->get_index() == last_ack_received){
		num_duplicates++;
		// fast retransmit
		if(fast_retransmit && num_duplicates == 3){
			handle_time_out();
			
		}
		else{
			window_start = packet->get_index();
			window_size /= 2;
			send_now = send_packets();
		}	    
	}
	// Handle normally 
	else{	    
		num_duplicates = 0; 
		if(slow_start){
			window_start++;
			window_size++;
			send_now = send_packets();
		}
		else{
			window_start++;
			window_size += 1 / (double)window_size;
			send_now = send_packets();
		}
		  
    }
	last_ack_received = packet->get_index();
	return send_now;
}

void Flow::handle_time_out(){	
	if(sending.size() > 0){
		if(slow_start){
			slow_start = false;
			ss_threshold = window_size / 2;
			window_size = ss_threshold;
		}
		num_duplicates = 0;
		window_size /= 2;
		send_packets();
		last_time_out = global_time;
	}
}

bool Flow::received_packet(int num) {
	if(std::find(received.begin(), received.end(), num) == received.end()){
		return false;
	}
	return true;
}

Data_packet * Flow::generate_packet(int n) {
	Data_packet * packet = new Data_packet(source, destination, n, this, global_time);
	return packet; 

}

Ack_packet * Flow::generate_ack_packet() {
	Ack_packet * packet = new Ack_packet(source, destination, this, to_receive, global_time);
	return packet; 
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
