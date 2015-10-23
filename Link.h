#include <iostream>
#include <queue>

using namespace std;

class Link {
	int capacity;
	int flowrate;
	int delay;
	int buffersize;
	std::queue <Naive_Packet> buffer;

public:
	// Constructor
	Link();

	// Set capacity, flow rate, transmission delay, and buffer size.
	void set_values(int, int, int, int);

	// Add a packet to the link's buffer.
	void add_to_buffer(Naive_Packet packet);
	
};