#include <iostream>
#include <queue>
#include <vector>
#include <ctime>
#include <unordered_map>

#include "user.h"
#include "block.h"
#include "txn.h"

class Event 
{
	int event_func;
	double time;
	Transaction transaction;
	Block block;
	User user;
	User parent_node;		// to not forward to sender again

public:
	Event(int func, double time, Transaction txn, Block block, User user, User parent_node);
};