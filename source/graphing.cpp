static double * vector_time;
static double * vector_flow_rate1;
static double * vector_flow_rate2;
static int vector_index;
extern double global_time;
extern char * file_name;
#include "graphing.h"
void init_graphs(Network * network, mxArray **plhs) {
    mexEvalString("close all; clear all;");
	if (strcmp(file_name, "testcase0.txt") == 0) {
		init_testcase0(network, plhs);
	}
	else if (strcmp(file_name, "testcase1.txt") == 0) {
		init_testcase1(network, plhs);
	}
	else {
		printf("graphing.cpp: unrecognized file: %s\n. Exiting.\n", file_name);
		exit(-1);
	}
}
void init_testcase0(Network * network, mxArray **plhs) {
    vector_index = 0;
	plhs[0] = mxCreateNumericMatrix(1,100000,mxDOUBLE_CLASS,mxREAL); //time
	plhs[1] = mxCreateNumericMatrix(1,100000,mxDOUBLE_CLASS,mxREAL); //flowrate
    mxArray *inplot[2];
    vector_time = (double *) mxGetPr(plhs[0]); 
    vector_flow_rate1 = (double *) mxGetPr(plhs[1]); 
    mexEvalString("fig1 = figure;");
    mexEvalString("set(gcf,'numbertitle','off','name','Flowrate (Bps) vs time (s)')");
    mexEvalString("movegui(fig1, 'northwest')");
    mexEvalString("pause(.00001)");
    /*
    mexEvalString("fig2 = figure;");
    mexEvalString("movegui(fig2, 'north')");
    */
}

void init_testcase1(Network * network, mxArray **plhs) {
    vector_index = 0;
	plhs[0] = mxCreateNumericMatrix(1,100000,mxDOUBLE_CLASS,mxREAL);//time
	plhs[1] = mxCreateNumericMatrix(1,100000,mxDOUBLE_CLASS,mxREAL);//flowrate1
	plhs[2] = mxCreateNumericMatrix(1,100000,mxDOUBLE_CLASS,mxREAL);//flowrate2
    mxArray *inplot[2];
    vector_time = (double *) mxGetPr(plhs[0]); 
    vector_flow_rate1 = (double *) mxGetPr(plhs[1]); 
    vector_flow_rate2 = (double *) mxGetPr(plhs[2]); 
    mexEvalString("fig1 = figure;");
    mexEvalString("set(gcf,'numbertitle','off','name','Flowrate (Bps) vs time (s)')");
    mexEvalString("movegui(fig1, 'northwest')");
    mexEvalString("pause(.00001)");
    /*
    mexEvalString("fig2 = figure;");
    mexEvalString("movegui(fig2, 'north')");
    */
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
		printf("graphing.cpp: unrecognized file: %s\n. Exiting.\n", file_name);
		exit(-1);
	}
}

void update_testcase0(Network * network, mxArray **plhs)
{
    vector_time[vector_index] = global_time;
    vector_flow_rate1[vector_index] = network->all_links[0]->get_flowrate();
    mxArray *inplot[3];
    // Changing the active figure
    mexEvalString("figure(fig1)");
    // Overwriting the active figure
    inplot[0] = plhs[0]; // time
    inplot[1] = plhs[1]; // set x axis to link1 flowrate
    inplot[2] = mxCreateString("b"); // set color to blue
    mexCallMATLAB(0, NULL, 3, inplot, "plot");
    mexEvalString("pause(.00001)");
}

void update_testcase1(Network * network, mxArray **plhs)
{
    vector_time[vector_index] = global_time;
    vector_flow_rate1[vector_index] = network->all_links[1]->get_flowrate();
    vector_flow_rate2[vector_index] = network->all_links[2]->get_flowrate();
    mxArray *inplot[2];
    // Changing the active figure
    mexEvalString("figure(fig1)");
    // Overwriting the active figure
    inplot[0] = plhs[0]; // time
    inplot[1] = plhs[1]; // set x axis to link1 flowrate
    inplot[2] = mxCreateString("b"); // set color to blue
    mexCallMATLAB(0, NULL, 2, inplot, "plot");
    mexEvalString("hold on");
    inplot[1] = plhs[2]; //set y axis to link2 flowrate
    inplot[2] = mxCreateString("r"); // set color to red
    mexCallMATLAB(0, NULL, 2, inplot, "plot");
    mexEvalString("hold off");
    mexEvalString("pause(.00001)");
}
