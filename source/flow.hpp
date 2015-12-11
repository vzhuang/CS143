#ifndef FLOW_HPP
#define FLOW_HPP

#include "link.hpp"
#include "packet.hpp"
#include "node.hpp"
#include "flow.hpp"
#include <iostream>
#include <vector>
#include <deque>
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
#define FAST_DELAY 0.125
#define RESEND_TIME 0.01

using namespace std;

class Flow{
	Host * source;
	Host * destination;
	double start;
    
	int size;
    int num_duplicates; // keeps track of how many duplicate packets received
    double ss_threshold;        
    double a;
    double b;
    double rtt_avg;
    double rtt_dev;  
	
public:
    // variables
    double time_out; // time to time_out
    int to_receive; // next packet expected to receive
    int last_ack_received; // to check for duplicate acks
    vector<int> received; // received packets (by destination) 
    vector<int> sent_packets;
    vector<int> lost_packets;
    deque<int> timed_out_packets;
    int sent;
    int next_index;
    double bytes_received;
    double last_bytes_received_query;
    double window_size; // cwnd
    double last_flow_rate_query; // Time the last flow rate was queried
    double rtt_min;
    int in_flight;
    double last_ack_time;
    double rtt;
    double alpha;
    double beta;
    double ca_wnd;
	int max_ack_received;
	vector<int> sending;

    //bool first_ss;
    bool done; 
    bool fast_retransmit; // use fast retransmit?
    bool fast_recovery; // use fast recovery?
    
    // headers
	Flow(Host * source_, Host * dest_, double data_size_, double start_);
	double get_start();
	Host * get_source();
	double get_flowrate();
	Host * get_destination();
	vector<Data_packet *> send_packets();
	void send_data();
    //void reset_flow();
	Data_packet * generate_packet(int n);
    Ack_packet * generate_ack_packet();
    void receive_data(Data_packet * packet);
    
    bool timed_out(int num);
    bool acked_packet(int num);
    bool sent_packet(int num);
    bool received_packet(int num);
    void receive_ack(Ack_packet * packet);
    void handle_time_out(int index);
    void print_received();
};

#endif
