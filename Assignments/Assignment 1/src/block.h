#include <vector>

class Block
{
	double timestamp;
	double recv_time;
	vector<Transaction> txns;
	int blockID;
	int p_blockID;
	vector<int> c_blockID; 			//blockID's for children
	int depth;

public:
	Block(double time, vector<Transaction> txns, int p_blockID);
};