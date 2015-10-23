#include <iostream>
#include "link.h"
#include <queue>

using namespace std;

Link::Link() {
}

void Link::set_values(int cap, int flow, int delay, int buf) {
	capacity = cap;
	flowrate = flow;
	delay = delay;
	buffersize = buf;
}

void Link::add_to_buffer(Naive_Packet::Naive_Packet packet) {
	// If the buffer is not full, enqueue the incoming packet.
	if (buffer.size() < buffersize) {
		buffer.push(packet);
	}
	// Else, the buffer is full. The packet is dropped.
}

Naive_Packet::Naive_Packet Link::transmit_packet() {
	// The packet at the front of the buffer is transmitted.
	Naive_Packet:Naive_Packet transmitted_packet = buffer.front();
	// Dequeue transmitted packet from the buffer.
	buffer.pop();
	printf("Dequeued from buffer.\n");
	return transmitted_packet;
}

/*
* Calculate link cost based on static component (transmission delay) and
* dynamic component (flow rate).
*/
int Link::calculate_cost(int delay, int flowrate, bool direction) {
	int cost = delay + flowrate;
	return cost;
}