#ifndef NAIVEPACKET_H
#define NAIVEPACKET_H

#include <string>
using namespace std;

// Packet Class
class Naive_Packet {
	int data;
	int index;
	string source;
	string dest;

public:
	//Constructor
	Naive_Packet(int my_data, int my_index, string my_source, string my_dest);
	//Return IP address of source
	string get_source();
	//Return IP address of dest
	string get_dest();
	//Return index of packet in flow
	int get_index();
};

#endif
