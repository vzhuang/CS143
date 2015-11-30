static double * vector_time;
static double * vector_flow_rate;
static int vector_index;
extern double global_time;
#include "graphing.h"
void init_graphs(Network * network, mxArray **plhs) {
    mexEvalString("close all; clear all;");
    vector_index = 0;
	plhs[0] = mxCreateNumericMatrix(1,1000,mxDOUBLE_CLASS,mxREAL);
	plhs[1] = mxCreateNumericMatrix(1,1000,mxDOUBLE_CLASS,mxREAL);
    mxArray *inplot[2];
    vector_time = (double *) mxGetPr(plhs[0]); 
    vector_flow_rate = (double *) mxGetPr(plhs[1]); 
    mexEvalString("fig1 = figure;");
    mexEvalString("movegui(fig1, 'northwest')");
    mexEvalString("pause(0)");
    /*
    mexEvalString("fig2 = figure;");
    mexEvalString("movegui(fig2, 'north')");
    */
}

void update_graphs(Network * network, mxArray **plhs) {
    vector_time[vector_index] = global_time;
    vector_flow_rate[vector_index] = network->all_links[0]->get_flowrate();
    vector_index++;
    mxArray *inplot[2];
    // Changing the active figure
    mexEvalString("figure(fig1)");
    // Overwriting the active figure
    inplot[0] = plhs[0];
    inplot[1] = plhs[1];
    mexCallMATLAB(0, NULL, 2, inplot, "plot");
    mexEvalString("pause(.00001)");
}
