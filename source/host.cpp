#include <string>
#include "host.h"

using namespace std;

Host::Host(Link * my_link, string my_ip) {
	link = my_link;
	ip = my_ip;
}

void Host::set_link(Link * my_link) {
	link = my_link;
}

string Host::get_ip() {
	return ip;
}
