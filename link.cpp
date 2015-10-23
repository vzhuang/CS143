#include <iostream>
#include "link.h"
#include <queue>

using namespace std;

Link::Link() {
	// Initialize queue in here?
}

void Link::set_values(int cap, int flow, int delay, int buf) {
	capacity = cap;
	flowrate = flow;
	delay = delay;
	buffersize = buf;
}

void Link::add_to_buffer(Naive_Packet packet) {
	// If the buffer is not full, enqueue the incoming packet.
	if (buffer.size() < buffersize) {
		buffer.push(packet);
	}
	// Else, the buffer is full. The packet is dropped.
}

int Link::calculate_cost(int delay, int flowrate, bool direction) {
	int cost = delay + flowrate;
	return cost;
}