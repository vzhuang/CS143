#include <string>
#include "naivepacket.h"

using namespace std;

// Constructor
Naive_Packet::Naive_Packet(int my_data, int my_index, string my_source, string my_dest) {
	data = my_data;
	index = my_index;
	source = my_source;
	dest = my_dest;
}

// Return source	
string Naive_Packet::get_source() {
	return source;
}

// Return dest
string Naive_Packet::get_dest() {
	return dest;
}

// Return index	
int Naive_Packet::get_index() {
	return index;
}
