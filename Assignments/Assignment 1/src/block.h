#ifndef block_h
#define block_h

#include <vector>
#include "txn.h"

class Block
{
public:
	// block struct, most names are self explanatory, ask if not understand
	double timestamp;
	double recv_time;
	std::vector<Transaction> txns;
	int blockID;
	int p_blockID;
	std::vector<int> c_blockID; 			//blockID's for children
	int depth;
	int userID;

	inline Block(double time, double recv_time, std::vector<Transaction> txns, int blockID, int p_blockID, int depth, int userID)
	: timestamp(time), recv_time(recv_time), txns(txns), blockID(blockID), p_blockID(p_blockID), depth(depth), userID(userID), c_blockID(std::vector<int>())
	{
	}

	inline void add_child(int childID)
	{
		c_blockID.push_back(childID);
	}
};

#endif