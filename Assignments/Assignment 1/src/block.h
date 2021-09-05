#include <vector>

struct Block
{
	// block struct, most names are self explanatory, ask if not understand
	double timestamp;
	double recv_time;
	vector<Transaction> txns;
	int blockID;
	int p_blockID;
	vector<int> c_blockID; 			//blockID's for children
	int depth;

	inline Block(double time, double recv_time, vector<Transaction> txns, int blockID, int p_blockID, int depth)
	: timestamp(time), recv_time(recv_time), txns(txns), blockID(blockID), p_blockID(p_blockID), depth(depth)
	{
	}

	inline void add_child(int childID)
	{
		c_blockID.push_back(childID);
	}
};