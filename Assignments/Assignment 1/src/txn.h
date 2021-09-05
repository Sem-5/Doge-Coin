// self explantory, struct for a transaction
// recvID = -1 for coinbase

struct Transaction 
{
	int txnID;
	int sendID;
	int recvID;
	int amount;

	inline Transaction(int txnID, int sendID, int recvID, int amount)
	: txnID(txnID), sendID(sendID), recvID(recvID), amount(amount)
	{
	}
};