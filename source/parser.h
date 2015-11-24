/*
Our network simulator takes data in the following form:

Hosts:
[int num_hosts]
Routers:
[int num_routers]
Links:
[H/R# H/R# capacity delay buffer_size]
....
[H/R# H/R# capacity delay buffer_size]
Flows:
[H# H# data_amount start]
...
[H# H# data_amount start]


To elaborate, both hosts and rotuers can be specified by a single integer 
representing the total number of hosts/routers to be in the network.

Links are specified as individual entries with specific node endpoints (1-indexed!)
and a link capacity. For example, the following entry:
H0 R1 10 20 30
specifies to create a link between the first host and the second router with
a link of capacity 10 MBps, delay of 20 ms, and buffer_size of 30 MB.

Flows are specified as individual entries with specific start and end hosts,
an amount of data to transmit, and a start time. For example, the following entry:
H1, H2, 100, 0
specifies to create a flow that goes from the first host to the second host. This
flow will have 100 MB of data at begin at t = 0 ms.

______________________
Notes:

1) The simulation is event based and is based off a global time representing the 
number of elpased miliseconds.

2) After a flow is instantiated, it will have a "flow begin" event pushed onto
a global priority queue. 

3) the initial Routing tables are calculated immediately after the creation of the 
network.
*/

#ifndef PARSER_H
#define PARSER_H
#include "flow.h"
#include "link.h"
#include "node.h"
#include "network.h"
#include <fstream>
#include <iostream>
#include <string>
class Flow;
class Link;

/* Create a network at the passed in Network * using the passed in 
   network specification file. */
void build_network(Network * network, char * network_file);

#endif  /* PARSER_H */

