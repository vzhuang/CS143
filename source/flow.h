#ifndef FLOW_H
#define FLOW_H

#include "link.h"
#include "packet.h"
#include "node.h"
#include "flow.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
class Node;
class Host;
class Link;
class Packet;
class Data_packet;
class Ack_packet;

#define TCP_TAHOE 0
#define TCP_RENO 1
#define TCP_FAST 2

using namespace std;

class Flow{
	Host * source;
	Host * destination;
	double start;
	int size;
    int algorithm; // TCP algorithm 
    vector<int> received; // received packets (by destination)    

    double window_size;
    int window_start; 
    
    int num_duplicates; // keeps track of how many duplicate packets received
    int ss_threshold; // W_0/2

    bool done;
    
    bool slow_start; // in slow start phase?    
    bool fast_retransmit; // use fast retransmit?
    bool fast_recovery; // use fast recovery?
    double last_time_out;
    
    
    vector<double> times;
    double a;
    double b;
    double rtt_avg;
    double rtt_dev;
    vector<int> round_trip_times; 
  
	
public:
    // variables
    double time_out;
    int to_receive; // next packet expected to receive
    int last_ack_received; // to check for duplicate acks
    vector<int> sending; // packets currently sent but not acked
    vector<int> sent_packets;
    vector<int> acked_packets;
    int sent;
    int next_index;
    int bytes_received;

    // headers
	Flow(Host * source_, Host * dest_, double data_size_, double start_);
	double get_start();
	Host * get_source();
	Host * get_destination();
	vector<Data_packet *> send_packets();
	void send_data();
	Data_packet * generate_packet(int n);
    Ack_packet * generate_ack_packet();
    void receive_data(Data_packet * packet);
    bool acked_packet(int num);
    bool sent_packet(int num);
    bool received_packet(int num);
    vector<Data_packet *> receive_ack(Ack_packet * packet);
    void handle_time_out();
    void print_sending();
};

#endif
