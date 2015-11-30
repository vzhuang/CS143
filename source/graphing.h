
#ifndef GRAPHING_H
#define GRAPHING_H
#include "network.h"
class Flow;
class Link;

void init_graphs(Network * network, mxArray **plhs);
void update_graphs(Network * network, mxArray **plhs);


#endif  /* GRAPHING_H */
