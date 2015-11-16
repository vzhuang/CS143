#ifndef PARSER_H
#define PARSER_H
#include "flow.h"
#include "link.h"
#include "node.h"
#include <fstream>
#include <iostream>
#include <string>

class Flow;
class Link;

/* Create a network at the passed in Network * using the passed in 
   network specification file. */
void build_network(Network * network, char * network_file);

#endif  /* PARSER_H */
