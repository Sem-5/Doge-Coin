#include <iostream>
#include <queue>
#include <vector>
#include <ctime>
#include <unordered_map>
#include <iterator>

#include "block.h"
#include "txn.h"

class User 
{
	int curr_blkID;									// blockID of leaf of longest chain
	double txn_time;								// mean of exponential for generating next txn
	double blk_time;								// mean of exponential for PoW

	vector<int> peers;								// store ID of all peers
	vector<int> balance;							// coin balance of each peer
	bool fast;										// fast or slow node for link speed

	unordered_map<int, Transaction> txn_pool;
	unordered_map<int, Block> blockchain;			// Each block contains block id of parent and children, 
													// map for easier access of blocks and checking

	list<int> txn_enc;								// bounded lru based queue for transactions already forwarded
	int txn_max_sz;									// maximum size of transaction queue
	unordered_map<int, list<int>::iterator> txn_ptrs;	// ptr to txnID in list for O(1) access

	list<Block> blk_enc;							// same concept as for txns
	int blk_max_sz;					
	unordered_map<int, list<int>::iterator> blk_ptrs; 	

public:
	inline User(int txn_time, int blk_time, int n, Block genesis, bool fast) 
	: txn_time(txn_time), blk_time(blk_time), txn_max_sz(2000), blk_max_sz(1000), fast(fast), curr_blkID(0)
	{
		// add genesis block
		blockchain[genesis.blockID] = genesis;
		blk_enc.push_front(genesis.blockID);
		blk_ptrs[genesis.blockID] = blk_enc.begin();

		// initialize all users balance to 0
		balance = vector<int> (n, 0);
	}

	inline void add_peer(int user) {
		peers.push_back(user);
	}

	bool is_valid_txn(Transaction txn);
	bool is_valid_block(Block block, bool *long_change);
	vector<Transaction> choose_txns();
	void update_balance(Block block);
}