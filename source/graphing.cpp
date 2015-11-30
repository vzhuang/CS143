static double * vector_time;
static double * vector_flow_rate1;
static double * vector_flow_rate2;
static int vector_index;
extern double global_time;
#include "graphing.h"
void init_graphs(Network * network, mxArray **plhs) {
    mexEvalString("close all; clear all;");
    vector_index = 0;
	plhs[0] = mxCreateNumericMatrix(1,1000,mxDOUBLE_CLASS,mxREAL);
	plhs[1] = mxCreateNumericMatrix(1,1000,mxDOUBLE_CLASS,mxREAL);
	plhs[2] = mxCreateNumericMatrix(1,1000,mxDOUBLE_CLASS,mxREAL);
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
    vector_time[vector_index] = global_time;
    vector_flow_rate1[vector_index] = network->all_links[1]->get_flowrate();
    vector_flow_rate2[vector_index] = network->all_links[2]->get_flowrate();
    vector_index++;
    mxArray *inplot[2];
    // Changing the active figure
    mexEvalString("figure(fig1)");
    // Overwriting the active figure
    inplot[0] = plhs[0];
    inplot[1] = plhs[1];
    mexCallMATLAB(0, NULL, 2, inplot, "plot");
    mexEvalString("hold on");
    inplot[1] = plhs[2];
    mexCallMATLAB(0, NULL, 2, inplot, "plot");
    mexEvalString("hold off");
    mexEvalString("pause(.00001)");
}
