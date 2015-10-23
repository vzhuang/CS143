#include <iostream>
//#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

void Router::build_table (vector<int> source, vector<int> dest) {
	source.push_back(0);
	source.push_back(1);
	dest.push_back(1);
	dest.push_back(0);
}

Router::Router() {
	build_table(my_table.source, my_table.dest);
}
// Packet Class
class Naive_Packet {
	int my_data;
	string source;
	string destination;

public:
	void build_table();
};


int main () {
	Router router1;
	return 0;
}


