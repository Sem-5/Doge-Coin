#include <iostream>
#include <queue>
#include <vector>
#include <math>

#include "user.h"
#include "event.h"
#include "block.h"
#include "distris.h"

class Simulator
{
	vector<User> users; 													
	int n;			
	priority_queue <Event, vector<Event>, greater<Event> > event_queue;		// priority queue which maintains list of events to be done
																			// problem: how to cancel events ahead in time? 
																			// should we instead use map from time to list of events?
	vector<vector<double> > light_delay;			// speed of light delay between pairs
	int blockID, txnID;
	double cur_time;								// curr time in simulation

public:
	inline Simulator(int n, double z, vector<double> txn_means, vector<double> blk_means, double end_time) 
	: blockID(0), txnID(0), cur_time(0.0), n(n)
	{

		// initialize n users each having genesis block
		Block genesis(0, 0, vector<int>, 0, -1, 0);
		vector<bool> fast = fast_list(n,z);
		blockID++;

		for(int i = 0; i < n; i++){
			User user(txn_means[i], blk_means[i], n, genesis, fast[i]);
			users.push_back(user);
		}

		// sample uniformly from (10,500) to create speed of light delay matrix
		light_delay = vector<vector<double> > (n, vector<double>(n, 0.0))
		for(int i = 0; i < n; i++){
			for(int j = 0; j < n; j++){
				if(i==j)
					light_delay[i][j] = 0;
				else
					light_delay[i][j] = unif_real(10, 500);
			}
		}

		// set peers for the nodes
		set_network();

		// set up initial transaction and block creation here
		init_blocks();
		init_txns();

		// start simulating
		simulate(end_time);
	}

	// controlling simulation
	void simulate(double end_time);

	// helper functions
	void set_network();
	void init_blocks();
	void init_txns();
	double get_latency(int i, int j, int size);
	vector<bool> fast_list(int n, double z);

	// The 4 events, ordered 0,1,2,3 in this order
	void generate_transaction(Event e);
	void receive_transaction(Event e);
	void generate_block(Event e);
	void receive_block(Event e);
};