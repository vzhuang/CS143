#include <iostream>
//#include <unordered_map>
#include <vector>
#include <string>
#include "router.h"
using namespace std;

void Router::build_table (routing_table table) {
	table.source.push_back(0);
	table.source.push_back(1);
	table.dest.push_back(1);
	table.dest.push_back(0);
}

Router::Router() {
	build_table(my_table);
}


