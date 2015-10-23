#include <string>
#include "naivepacket.h"

using namespace std;

Naive_Packet::Naive_Packet(int my_data, int my_index, string my_source, string my_dest) {
	data = my_data;
	index = my_index;
	source = my_source;
	destination = my_destination;
}
	
