#include "flow.h"

Flow::Flow(Host * source_, Host * destination_, double data_size_, double start_){
	source = source_;
	destination = destination_;
	size = data_size_;
	start = start_;
}

Host * Flow::get_source(){
	return source;
}

Host * Flow::get_destination(){
	return destination;
}

double Flow::get_start(){
	return start;
}

void Flow::send_stream(){
	
}

void Flow::send_data(){
	//int n_packets;
}

void Flow::generate_packet(){
	
}

void Flow::send_packet(){
	
}
