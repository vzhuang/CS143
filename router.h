#ifndef ROUTER_H
#define ROUTER_H
#include <iostream>
//#include <unordered_map>
#include <vector>
#include <string>
using namespace std;

struct routing_table {
	vector<int> source;
	vector<int> dest;
};

// Router Class
class Router {
	//vector<Link> my_links;
	routing_table my_table;
	
public:
	void build_table(routing_table table);
	Router();
};

#endif