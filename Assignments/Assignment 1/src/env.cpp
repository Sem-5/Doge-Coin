#include "env.h"

void Simulator::set_network()
{	
	// Generates the random connected graph
	// Sets user peers appropriately in user struct

	static std::random_device rd;

	double k = (log10(n)/n);

	std::vector<int> arr(1,1);
	
	// generate graph
	for(int node=2; node<=n; node++)
	{
		// sampling degree for n-th node from poisson distribution
		int d = poisson(k*(node-1));
		while(d>n-1 || d==0)
			d = poisson(k*(node-1));

		// picking d random values
		std::mt19937 gen(rd());
		shuffle(arr.begin(), arr.end(), gen);

		// adding them to the peer list of the users
		for(int i=0; i<d; i++)
		{
			users[node-1].add_peer(arr[i]-1);
			users[arr[i]-1].add_peer(node-1);
		}

		arr.push_back(node);
	}
}

void Simulator::init_blocks()
{
	// At start of simulation, each node starts with one "generate_block" event 
	// on an empty block with just the coinbase to start simulation

	for(int i = 0; i<n; i++){
		// generate block generation time (PoW)
		double t = exponential(users[i].blk_time);

		// add coinbase transaction
		Transaction coinbase(txnID, i, -1, 50.0);
		std::vector<Transaction> txns;
		txns.push_back(coinbase);

		// Create block to be generated and add in event
		// note that here BlockID and TxnID for coinbase is just a temporary value
		Block blk(0, t, txns, blockID, users[i].curr_blkID, users[i].blockchain[users[i].curr_blkID].depth + 1, i);
		Event e(2, t, 
				Transaction(-1,-1,-1,-1), 
				blk, 
				i, i);
		event_queue.push(e);

	}
}

void Simulator::init_txns()
{
	// Same as init_blocks, each user starts waiting for the interarrival time between transactions
	// no actual transaction being created here unlike blocks

	for(int i = 0; i<n; i++){
		// sample time when transaction finally occurs
		double t = exponential(users[i].txn_time);

		// schedule event to generate transaction at time t
		Event e(0, t, 
				Transaction(-1,-1,-1,-1), 
				Block(-1,-1, std::vector<Transaction>(),-1,-1,-1,-1), 
				i, i);
		event_queue.push(e);
	}
}

std::vector<bool> Simulator::fast_list(int n, double z)
{
	static std::random_device rd;

	std::vector<int> arr;
	for(int i = 0; i<n; i++)
		arr.push_back(i);

	std::mt19937 gen(rd());
	shuffle(arr.begin(), arr.end(), gen);

	int num_slow = n*z;
	std::vector<bool> fast(n, true);
	for(int i =0; i<num_slow; i++)
		fast[arr[i]] = false;

	return fast;
}

double Simulator::get_latency(int i, int j, double size)
{
	// size is in MB, latency returned is in ms
	// Gives the latency when sender is i and receiver j
	// latency may not be same to and fro

	double v = light_delay[i][j];
	double c = (users[i].fast && users[j].fast) ? 100 : 5;
	double d = exponential(96.0/c);

	double latency = v + (double)size/c + d;
	return latency;
}

void Simulator::simulate(double end_time)
{
	// the simulator object
	// keeps calling events till end_time not reached
	while(cur_time <= end_time && event_queue.size() > 0)
	{
		Event e = event_queue.top();
		event_queue.pop();
		cur_time = e.time;

		// call appropriate function
		switch(e.event_func)
		{
			case 0: generate_transaction(e); break;
			case 1: receive_transaction(e); break;
			case 2: generate_block(e); break;
			case 3: receive_block(e); break;
		}
	}
}

void Simulator::generate_transaction(Event e)
{
	// pick receiver except itself with uniform probability
	int recv = unif_int(0, n-1);
	while(recv == e.userID)
		recv = unif_int(0, n-1);

	// create transaction by sampling some fraction of total coins
	int tot_coins = users[e.userID].balance[e.userID];
	double fraction = unif_real(0,1);
	int coins = tot_coins * fraction;

	Transaction txn(txnID, e.userID, recv, coins);
	txnID++;

	// call is_valid_txn to update lru and txn_pool of user
	bool is_valid = users[e.userID].is_valid_txn(txn);

	// if transaction is valid, broadcast to all peers
	if(is_valid)
	{
		for(int peer_id: users[e.userID].peers)
		{
			if(peer_id != e.p_userID)
			{
				double t = get_latency(e.userID, peer_id, 0.001);
				Event new_event(1, t+cur_time, 
								txn, 
								Block(-1,-1, std::vector<Transaction>(),-1,-1,-1,-1), 
								peer_id, e.userID);
				event_queue.push(new_event);
			}
		}
	}

	// schedule another event for next time transaction created
	double t = exponential(users[e.userID].txn_time);
	Event new_event(0, t, 
					Transaction(-1,-1,-1,-1), 
					Block(-1,-1, std::vector<Transaction>(),-1,-1,-1,-1),
					e.userID, e.userID);
	event_queue.push(new_event);
}

void Simulator::receive_transaction(Event e)
{
	// on receiving transaction, call is_valid_txn to update lru and txn_pool
	// if false, transaction already encountered

	bool is_valid = users[e.userID].is_valid_txn(e.transaction);
	if (is_valid)
	{
		for(int peer_id: users[e.userID].peers)
		{
			if(peer_id != e.p_userID)	// forward to all except sender
			{
				double t = get_latency(e.userID, peer_id, 0.001);
				Event new_event(1, t+cur_time, 
								e.transaction, 
								Block(-1,-1, std::vector<Transaction>(),-1,-1,-1,-1), 
								peer_id, e.userID);
				event_queue.push(new_event);
			}
		}
	}
}

void Simulator::generate_block(Event e)
{
	// check if the block received at this instant is actually child of longest chain
	// if not, ignore event
	// put appropriate blockID and txnID inside block and update ID's
	// update blockchain of user and lru using is_valid_block
	// broadcast to all other users
	// event e already has the block which was being mined
	// here just forward to all peers and schedule new block for mining

	if(e.block.p_blockID != users[e.userID].curr_blkID)
		return;
	e.block.blockID = blockID;
	e.block.txns.back().txnID = txnID;
	blockID++;
	txnID++; 

	bool long_change = false;
	bool is_valid = users[e.userID].is_valid_block(e.block, &long_change);
	if (is_valid)
	{
		for(int peer_id: users[e.userID].peers)
		{
			if(peer_id != e.p_userID)
			{
				double t = get_latency(e.userID, peer_id, e.block.txns.size() * 0.001);
				Event new_event(3, t+cur_time,
								 Transaction(-1,-1,-1,-1), 
								 e.block, 
								 peer_id, e.userID);
				event_queue.push(new_event);
			}
		}
	}

	// choose set of transactions for block
	std::vector<Transaction> txns = users[e.userID].choose_txns();
	Transaction coinbase(txnID, e.userID, -1, 50.0);
	txns.push_back(coinbase);

	// block creation time
	double t = exponential(users[e.userID].blk_time);

	// create block and add in queue
	Block blk(cur_time, t+cur_time, txns, blockID, users[e.userID].curr_blkID, users[e.userID].blockchain[users[e.userID].curr_blkID].depth + 1, e.userID);
	Event new_event(2, t+cur_time, 
					Transaction(-1,-1,-1,-1), 
					blk, 
					e.userID, e.userID);
	event_queue.push(new_event);
}

void Simulator::receive_block(Event e)
{
	// received block
	// check if all transactions are valid
	// long_change is true if the received block becomes the leaf on the longest chain
	// that is the current block everyone must mine on changes

	Block recv_block = e.block;
	recv_block.recv_time = cur_time;
	bool long_change = false;
	bool is_valid = users[e.userID].is_valid_block(e.block, &long_change);

	// long change was true, then we should start mining on a new block
	if(long_change)
	{
		// choose set of transactions for block
		std::vector<Transaction> txns = users[e.userID].choose_txns();
		Transaction coinbase(txnID, e.userID, -1, 50.0);
		txns.push_back(coinbase);

		// block creation time
		double t = exponential(users[e.userID].blk_time);

		// create block and add in queue
		Block blk(cur_time, t+cur_time, txns, blockID, users[e.userID].curr_blkID, users[e.userID].blockchain[users[e.userID].curr_blkID].depth + 1, e.userID);
		Event new_event(2, t+cur_time, 
						Transaction(-1,-1,-1,-1), 
						blk, 
						e.userID, e.userID);
		event_queue.push(new_event);
	}

	// if block is valid, share to all peers
	if (is_valid)
	{
		for(int peer_id: users[e.userID].peers)
		{
			if(peer_id != e.p_userID)
			{
				double t = get_latency(e.userID, peer_id, e.block.txns.size() * 0.001);
				Event new_event(3, t+cur_time, 
								Transaction(-1,-1,-1,-1), 
								recv_block, 
								peer_id, e.userID);
				event_queue.push(new_event);
			}
		}
	}
}