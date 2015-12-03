
#ifndef GRAPHING_H
#define GRAPHING_H
#include "network.h"
#include "parser.h"
class Flow;
class Link;

void init_graphs(Network * network, const mxArray **prhs);
void update_graphs(Network * network, const mxArray **prhs);
// Update flowrate
void update_lrs(Network * network);
// Update buffer occupancy
void update_bos(Network * network);
// Update packet loss
void update_pls(Network * network);
// Update buffer flow rates
void update_frs(Network * network);
// Update window sizes
void update_wss(Network * network);
// Update packet delays
void update_pds(Network * network);
#endif  /* GRAPHING_H */
