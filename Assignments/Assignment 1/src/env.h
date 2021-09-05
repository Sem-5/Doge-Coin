#include <iostream>
#include <queue>
#include <vector>

#include "user.h"
#include "event.h"
#include "block.h"
#include "distris.h"

class Simulator
{
	vector<User> users; 			// need to define class User
	priority_queue <Event, vector<Event>, greater<Event> > event_queue;		// need to define Event class with its comparator
	vector<vector<int> > light_delay;
	int blockID, currID;

public:
	Simulator(int n, int z, vector<int> means);
	void simulate(double endTime);
	void set_network();
	double get_latency(int i, int j);

	void generate_transaction();
	void receive_transaction();
	void generate_block();
	void receive_block();
};