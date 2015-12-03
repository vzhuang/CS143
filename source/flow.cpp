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
	last_time_out = global_time;

	time_out = 1000;
	b = 0.25;
	rtt_avg = 0;
	rtt_dev = 0;
	sent_packets.push_back(0);
	//round_trip_times(size, 0);
}

void Flow::print_sending(){
	printf("sending: ");
	for(int i = 0; i < sending.size(); i++){
		printf("%d ", sending[i]);
	}
	printf("\n");
}

/**
 * TO DO: make sure packets are removed from sending in events
 * I.e. in general make sure sending contains list of packets pushed onto queue
 * but not yet sent.
 */ 

vector<Data_packet *> Flow::send_packets(bool duplicate) {
	// duplicate: true if retransmitting one packet
	vector<Data_packet *> send_now;
	printf("sending: %d window size: %f last_ack: %d\n", (int)sending.size(), window_size, last_ack_received);
	print_sending();
	if(duplicate){
		sending.push_back(last_ack_received);
		Data_packet * retransmit = generate_packet(last_ack_received);
		send_now.push_back(retransmit);
	}
	//printf("ss threshold: %d\n", ss_threshold);
	while(sending.size() < (int) window_size and !done){
		// if(sending.size() == 0){
		// 	next_index = sent_packets.back() + 1;//last_ack_received;
		// }
		// else{
		// 	next_index = sending.back() + 1;
		// }
		// if(duplicate){
		// 	next_index = last_ack_received;
		// }
		// if(next_index >= size){
		// 	break;
		// }
		if(!acked_packet(next_index)){
			sending.push_back(next_index);
			Data_packet * new_packet = generate_packet(next_index);
			if(!sent_packet(next_index)){
				sent += new_packet->packetSize();
				sent_packets.push_back(next_index);
			}		
			send_now.push_back(new_packet);
		}		
		next_index++;
	}
	printf("after: sending: %d window size: %f last_ack: %d\n", (int)sending.size(), window_size, last_ack_received);
	print_sending();
	return send_now;
}

/**
 * Handles data packet receipt 
 */
void Flow::receive_data(Data_packet * packet) {
	if(packet->getSource() == source && packet->getDest() == destination){
		if(!received_packet(packet->get_index())){
			received.push_back(packet->get_index());
			bytes_received += DATA_SIZE;
			if(bytes_received > size){
				done = true;
			}
		}
		if(sending.size() > 0){
			sending.erase(remove(sending.begin(), sending.end(), packet->get_index()), sending.end());
		}
		//printf("sending: %d\n", (int) sending.size());
		//printf("to_receive: %d\n", to_receive);
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
		printf("Wrong packet received");
	}
}

/**
 * TODO: create and push timeout events
 */ 

vector<Data_packet *> Flow::receive_ack(Ack_packet * packet) {
	acked_packets.push_back(packet->get_index() - 1);
	vector<Data_packet *> send_now;
	// recursively compute timeout value
	double rtt = global_time - packet->get_time();
	// initialize rtt;
	if(last_ack_received == 0){
		rtt_avg = rtt;
		rtt_dev = rtt;		
	}
	else{
		rtt_avg = (1 - b) * rtt_avg + b * rtt;
		rtt_dev = (1 - b) * rtt_dev + b * abs(rtt -  rtt_avg);
		time_out = rtt_avg + 4 * rtt_dev;
		if(time_out < 2000){
			time_out = 2000; // avoids small time_out errors
		}
	}
	// stop sending all unnecessary packetsn
	// for(vector<int>::iterator iter = sending.begin(); iter != sending.end();){
	//	if(*iter < packet->get_index()){
	//		iter = sending.erase(iter);
	//	}
	//	else{
	//		iter++;
	//	}
	//}
	//printf("%f %f %f %f\n", rtt, rtt_avg, rtt_dev, time_out);
	// If duplicate ack, go back n
	if(packet->get_index() == last_ack_received){
		num_duplicates++;
		// fast retransmit
		if(fast_retransmit and fast_recovery){
			printf("%d\n", num_duplicates);
			if(num_duplicates == 3){
				num_duplicates = 0;
				send_packets(true);
				slow_start = true;
				ss_threshold = window_size / 2;
				if(ss_threshold < 2){
					ss_threshold = 2;
				}
				//window_start = to_receive;
				window_size = 1;
				last_time_out = global_time;
				if(fast_recovery){
					window_size = ss_threshold + 3;
				}
			}			
		}
	}	
	// Handle normally 
	else{		
		last_ack_received = packet->get_index();
		num_duplicates = 0;
		// slow start
		if(slow_start){
			//window_start++;
			window_size++;
			if(window_size == ss_threshold){
				slow_start = false;
			}
			send_now = send_packets(false);
		}
		// congestion avoidance
		else{
			//window_start++;
			window_size += 1 / window_size;
			send_now = send_packets(false);
		}		  
    }
	last_ack_received = packet->get_index();
	return send_now;
}

void Flow::handle_time_out(int n){
	sending.erase(remove(sending.begin(), sending.end(), n), sending.end());
	sent_packets.erase(remove(sent_packets.begin(), sent_packets.end(), n), sent_packets.end());
	sent -= DATA_SIZE;
}

bool Flow::acked_packet(int num){
	if(find(acked_packets.begin(), acked_packets.end(), num) == acked_packets.end()){
		return false;
	}
	return true;
}

bool Flow::sent_packet(int num) {
	if(find(sent_packets.begin(), sent_packets.end(), num) == sent_packets.end()){
		return false;
	}
	return true;
}

bool Flow::received_packet(int num) {
	if(find(received.begin(), received.end(), num) == received.end()){
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

double Flow::get_flowrate() {
	double t0 = last_flow_rate_query;
	double tf = global_time;
	double bytes0 = last_bytes_received_query;
	double bytesf = bytes_received;
	double bytes = bytesf = bytes0;
	last_flow_rate_query = global_time;
	last_bytes_received_query = bytes_received;
	return bytes / (tf - t0);
	
}
