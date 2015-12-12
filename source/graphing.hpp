/* GRAPHING_HPP */
#ifndef GRAPHING_HPP
#define GRAPHING_HPP
#include "network.hpp"
#include "parser.hpp"
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
// Write a zero as the last data value to all graphs
void plot_final_points();
#endif  /* GRAPHING_HPP */
