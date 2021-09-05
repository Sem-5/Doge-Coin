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
	int txn_time;
	vector<int> peers;								// only store ID 
	unordered_map<int, Transaction> txn_pool;
	unordered_map<int, Block> blockchain;			// Each block contains block id of parent and children, 
													// map for easier access of blocks during forking?

	list<Transaction> txn_enc;						// bounded lru based queue for transactions already forwarded
	unordered_map<int, list<Transaction>::iterator> txn_ptrs;

	list<Block> blk_enc;					
	unordered_map<int, list<Block>::iterator> blk_ptrs;

	void update_txn_pool();							// called if new block is on greater depth 

public:
	User(int txn_time, Block genesis);
	void add_peers(vector<int> users);
	bool is_valid_txn(Transaction txn);
	bool is_valid_block(Block block);
}