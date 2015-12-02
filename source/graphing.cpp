static double * vector_time;
static double * vector_flow_rate1;
static double * vector_flow_rate2;
static int vector_index;
extern double global_time;
extern char * file_name;
#include "graphing.h"
void init_graphs(Network * network, mxArray **plhs) {
    mexEvalString("close all; clear all;");
    vector_index = 0;
	plhs[0] = mxCreateNumericMatrix(1,100000,mxDOUBLE_CLASS,mxREAL);
	plhs[1] = mxCreateNumericMatrix(1,100000,mxDOUBLE_CLASS,mxREAL);
	plhs[2] = mxCreateNumericMatrix(1,100000,mxDOUBLE_CLASS,mxREAL);
    mxArray *inplot[2];
    vector_time = (double *) mxGetPr(plhs[0]); 
    vector_flow_rate1 = (double *) mxGetPr(plhs[1]); 
    vector_flow_rate2 = (double *) mxGetPr(plhs[1]); 
    mexEvalString("fig1 = figure;");
    mexEvalString("figure(fig1)");
    mexEvalString("set(gcf,'numbertitle','off','name','Flowrate vs time')");
    mexEvalString("xlabel('Time (s)');");
    //mexEvalString("xlabel('Time (s)' , 'HandleVisibility' , 'off' ) ;");
    //mexEvalString("set( gca , 'NextPlot' , 'replacechildren' );");
    mexEvalString("movegui(fig1, 'northwest')");
    mexEvalString("pause(.00001)");
    /*
    mexEvalString("fig2 = figure;");
    mexEvalString("movegui(fig2, 'north')");
    */
}

void update_graphs(Network * network, mxArray **plhs) {
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
    vector_index++;
    mxArray *inplot[2];
    // Changing the active figure
    mexEvalString("figure(fig1)");
    // Overwriting the active figure
    inplot[0] = plhs[0]; // time
    inplot[1] = plhs[1]; // set x axis to link1 flowrate
    mexCallMATLAB(0, NULL, 2, inplot, "plot");
    mexEvalString("pause(.00001)");
}

void update_testcase1(Network * network, mxArray **plhs)
{
    vector_time[vector_index] = global_time;
    vector_flow_rate1[vector_index] = network->all_links[1]->get_flowrate();
    vector_flow_rate2[vector_index] = network->all_links[2]->get_flowrate();
    vector_index++;
    mxArray *inplot[2];
    // Changing the active figure
    mexEvalString("figure(fig1)");
    // Overwriting the active figure
    inplot[0] = plhs[0]; // time
    inplot[1] = plhs[1]; // set x axis to link1 flowrate
    mexCallMATLAB(0, NULL, 2, inplot, "plot");
    mexEvalString("hold on");
    inplot[1] = plhs[2]; //set y axis to link2 flowrate
    mexCallMATLAB(0, NULL, 2, inplot, "plot");
    mexEvalString("hold off");
    mexEvalString("pause(.00001)");
}
