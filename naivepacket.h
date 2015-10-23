#include <string>
using namespace std;

// Packet Class
class Naive_Packet {
	int data;
	int index;
	string source;
	string dest;

public:
	Naive_Packet(int my_data, int my_index, string my_source, string my_dest);

};