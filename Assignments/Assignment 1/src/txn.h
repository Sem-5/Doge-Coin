#ifndef txn_h
#define txn_h
// self explantory, struct for a transaction
// recvID = -1 for coinbase

class Transaction 
{
public:
	int txnID;
	int sendID;
	int recvID;
	int amount;

	inline Transaction(int txnID, int sendID, int recvID, int amount)
	: txnID(txnID), sendID(sendID), recvID(recvID), amount(amount)
	{
	}
};

#endif