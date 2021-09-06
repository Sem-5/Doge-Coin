#ifndef user_h
#define user_h

#include <iostream>
#include <vector>
#include <ctime>
#include <unordered_map>
#include <iterator>
#include <random>
#include <list>
#include <cmath>
#include <algorithm>

#include "txn.h"
#include "block.h"

class User 
{
public:
	int curr_blkID;									// blockID of leaf of longest chain
	double txn_time;								// parameter of exponential for generating next txn
	double blk_time;								// parameter of exponential for PoW

	std::vector<int> peers;								// store ID of all peers
	std::vector<int> balance;							// coin balance of each peer
	bool fast;										// fast or slow node for link speed

	std::unordered_map<int, Transaction> txn_pool;
	std::unordered_map<int, Block> blockchain;			// Each block contains block id of parent and children, 
													// map for easier access of blocks and checking

	std::list<int> txn_enc;								// bounded lru based queue for transactions already forwarded
	int txn_max_sz;									// maximum size of transaction queue
	std::unordered_map<int, std::list<int>::iterator> txn_ptrs;	// ptr to txnID in list for O(1) access	

	inline User(double txn_time, double blk_time, int n, Block genesis, bool fast) 
	: txn_time(txn_time), blk_time(blk_time), txn_max_sz(2000), fast(fast), curr_blkID(genesis.blockID)
	{
		// add genesis block
		int genesisID = genesis.blockID;
		blockchain.insert({genesisID, genesis});

		// initialize all users balance to 0
		balance = std::vector<int> (n, 0);
	}

	inline void add_peer(int user) {
		peers.push_back(user);
	}

	bool is_valid_txn(Transaction txn);
	bool is_valid_block(Block block, bool *long_change);
	std::vector<Transaction> choose_txns();
	std::vector<int> update_balance(Block block);
	int common_ancestor(Block block);
};

#endif