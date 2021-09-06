#ifndef event_h
#define event_h

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
public:
	int event_func;				// 4 functions, 0,1,2,3 represent values, check env.h for details
	double time;				// time of initiating event
	Transaction transaction;
	Block block;
	int userID;
	int p_userID;				// sender userID to not forward to sender again

	inline Event(int func, double time, Transaction txn, Block block, int userID, int p_nodeID)
	: event_func(func), time(time), transaction(txn), block(block), userID(userID), p_userID(p_nodeID) 
	{
	}

	// comparator for priority queue
	bool operator >(Event const &x)
	{
		return time > x.time;
	}
};

#endif