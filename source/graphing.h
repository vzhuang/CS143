
#ifndef GRAPHING_H
#define GRAPHING_H
#include "network.h"
#include "parser.h"
class Flow;
class Link;

void init_graphs(Network * network, mxArray **plhs);
void init_testcase0(Network * network, mxArray **plhs);
void init_testcase1(Network * network, mxArray **plhs);
void update_graphs(Network * network, mxArray **plhs);
void update_testcase0(Network * network, mxArray **plhs);
void update_testcase1(Network * network, mxArray **plhs);
#endif  /* GRAPHING_H */
