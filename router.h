#include <iostream>
//#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

// Router Class
class Router {
	//vector<Link> my_links;
	routing_table my_table;
	
public:
	void build_table(vector<int> source, vector<int> dest);
	Router();
};

struct routing_table {
	vector<int> source;
	vector<int> dest;
};