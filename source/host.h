/*
 * FILE: host.h
 *
 */

#ifndef HOST_H
#define HOST_H

#include "link.h"
#include <string>
class Host {
	Link * link;
	string ip;

public:
	// Constructor
	Host(Link * my_link, string my_ip);
	// Specify the link attached to this host
	void set_link(Link * my_link);
	//Return the ip of this host
	string get_ip();
};

#endif  /* HOST_H */
