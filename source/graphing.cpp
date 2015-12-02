#include "graphing.h"
static mxArray * inplot[3];
static mxArray * inplot_redraw[5];
static double * vector_time;
static double * vector_flow_rate1;
static double * vector_flow_rate2;
static double * vector_bo1;
static double * vector_bo2;
static int vector_index;
extern double global_time;
extern char * file_name;
void init_graphs(Network * network, mxArray **plhs) {
    mexEvalString("close all; clear all;");
	if (strcmp(file_name, "testcase0.txt") == 0) {
		init_testcase0(network, plhs);
	}
	else if (strcmp(file_name, "testcase1.txt") == 0) {
		init_testcase1(network, plhs);
	}
	else {
		mexPrintf("graphing.cpp: unrecognized file: %s\n. Exiting.\n", file_name);
		exit(-1);
	}
    // Flow rate graph
    mexEvalString("fig1 = figure;");
    mexEvalString("set(gcf,'numbertitle','off','name','Flowrate (Mbps) vs time (s)')");
    mexEvalString("movegui(fig1, 'northwest')");
    // Buffer ocupancy graph
    mexEvalString("fig2 = figure;");
    mexEvalString("set(gcf,'numbertitle','off','name','Buffer Occupancy (# packets) vs time (s)')");
    mexEvalString("movegui(fig2, 'north')");
    //Flush graphs
    mexEvalString("pause(.01)");
    inplot[2] = mxCreateString("--");
}
void init_testcase0(Network * network, mxArray **plhs) {
    vector_index = 0;
	plhs[0] = mxCreateNumericMatrix(1,100000,mxDOUBLE_CLASS,mxREAL); //time
	plhs[1] = mxCreateNumericMatrix(1,100000,mxDOUBLE_CLASS,mxREAL); //flowrate
	plhs[2] = mxCreateNumericMatrix(1,100000,mxDOUBLE_CLASS,mxREAL); //buffer occupancy
    vector_time = (double *) mxGetPr(plhs[0]); 
    vector_flow_rate1 = (double *) mxGetPr(plhs[1]); 
    vector_bo1 = (double *) mxGetPr(plhs[2]); 
}

void init_testcase1(Network * network, mxArray **plhs) {
    vector_index = 0;
	plhs[0] = mxCreateNumericMatrix(1,100000,mxDOUBLE_CLASS,mxREAL);//time
	plhs[1] = mxCreateNumericMatrix(1,100000,mxDOUBLE_CLASS,mxREAL);//flowrate1
	plhs[2] = mxCreateNumericMatrix(1,100000,mxDOUBLE_CLASS,mxREAL);//flowrate2
	plhs[3] = mxCreateNumericMatrix(1,100000,mxDOUBLE_CLASS,mxREAL); //buffer occupancy1
  	plhs[4] = mxCreateNumericMatrix(1,100000,mxDOUBLE_CLASS,mxREAL); //buffer occupancy2
    vector_time = (double *) mxGetPr(plhs[0]); 
    vector_flow_rate1 = (double *) mxGetPr(plhs[1]); 
    vector_flow_rate2 = (double *) mxGetPr(plhs[2]); 
    vector_bo1 = (double *) mxGetPr(plhs[3]); 
    vector_bo2 = (double *) mxGetPr(plhs[4]); 
}

void update_graphs(Network * network, mxArray **plhs) {
    vector_index++;
	if (strcmp(file_name, "testcase0.txt") == 0) {
		update_testcase0(network, plhs);
	}
	else if (strcmp(file_name, "testcase1.txt") == 0) {
		update_testcase1(network, plhs);
	}
	else {
		mexPrintf("graphing.cpp: unrecognized file: %s\n. Exiting.\n", file_name);
		exit(-1);
	}
    // Flush graphs
    mexEvalString("pause(.00001)");
}

void update_testcase0(Network * network, mxArray **plhs)
{
    vector_time[vector_index] = global_time;
    vector_flow_rate1[vector_index] = network->all_links[0]->get_flowrate() * 8.0 / 1000000.0;
    vector_bo1[vector_index] = network->all_links[0]->packets_stored;
    // Refresh flow rate graph
    mexEvalString("figure(fig1)");
    inplot[0] = plhs[0]; // time
    inplot[1] = plhs[1]; // set x axis to link1 flowrate
    mexCallMATLAB(0, NULL, 3, inplot, "plot");
    // Refresh buffer occupancy rate graph
    mexEvalString("figure(fig2)");
    inplot[1] = plhs[2]; // set y axis to link1 buffer occupancy
    mexCallMATLAB(0, NULL, 2, inplot, "plot");
}   

void update_testcase1(Network * network, mxArray **plhs)
{
    vector_time[vector_index] = global_time;
    vector_flow_rate1[vector_index] = network->all_links[1]->get_flowrate()* 8.0 / 1000000.0;
    vector_flow_rate2[vector_index] = network->all_links[2]->get_flowrate()* 8.0 / 1000000.0;
    vector_bo1[vector_index] = network->all_links[1]->packets_stored;
    vector_bo2[vector_index] = network->all_links[2]->packets_stored;
    // Refresh flow rate graph
    mexEvalString("figure(fig1)");
    inplot[0] = plhs[0]; // time
    inplot[1] = plhs[1]; // set x axis to link1 flowrate
    mexCallMATLAB(0, NULL, 3, inplot, "plot");
    mexEvalString("hold on");
    inplot[1] = plhs[2]; //set y axis to link2 flowrate
    mexCallMATLAB(0, NULL, 3, inplot, "plot");
    mexEvalString("hold off");
    // Refresh buffer occupancy rate graph
    mexEvalString("figure(fig2)");
    inplot[1] = plhs[3]; // set y axis to link1 buffer occupancy
    mexCallMATLAB(0, NULL, 2, inplot, "plot");
    mexEvalString("hold on");
    inplot[1] = plhs[4]; // set y axis to link2 buffer occupancy
    mexCallMATLAB(0, NULL, 2, inplot, "plot");
    mexEvalString("hold off");
}
