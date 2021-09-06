#include "user.h"

bool User::is_valid_txn(Transaction txn){
	// Function returns true only if the transaction is not present in lru of txn's
	// If block seen before, returning false
	// Updates lru
	// Use the functions output to decide whether to broadcast or not

	if(txn_ptrs.find(txn.txnID) == txn_ptrs.end()) 
	{

		txn_pool[txn.txnID] = txn;

		// update lru
		if(txn_enc.size() == txn_max_sz)
		{
			int id = txn_enc.back();
			txn_enc.pop_back();
			txn_ptrs.erase(id);
		}

		txn_enc.push_front(txn.txnID);
		txn_ptrs[txn.txnID] = txn_enc.begin();

		return true;
	}

	else
	{
		txn_enc.erase(txn_ptrs[txn.txnID]);
		txn_enc.push_front(txn.txnID);
		txn_ptrs[txn.txnID] = txn_enc.begin();
	}

	return false;
}

bool User::is_valid_block(Block block, bool *long_change){
	// Check if block already exists in blockchain
	// If yes, not a valid block, already forwarded

	if(blockchain.find(block.blockID) == blockchain.end()) 
	{	
		// find balance of each user in that path upto parent block
		vector<int> new_balance = update_balance();

		// check total money sent by each user
		vector<int> spent(balance.size(), 0);
		for(auto txn: block.txns)
		{
			if(txn.sendID != -1){		//coin base
				spent[txn.sendID] += txn.amount;

				if(spent[txn.sendID] > new_balance[txn.sendID])
					return false;
			}
		}

		// reached here -> valid block, update blockchain and add this ID to parent block
		blockchain[block.blockID] = block;
		if(block.p_blockID != -1)
			blockchain[block.p_blockID].add_child(block.blockID);

		// update transaction pool, erase the transactions from the pool
		for(auto txn: block.txns)
		{
			if(txn_pool.find(txn.txnID) != txn_pool.end())
				txn_pool.erase(txn.txnID);
		}

		// check if new added block has higher depth
		if(block.depth > blockchain[curr_blkID].depth)
		{
			// update balance of each user based on new path
			balance = new_balance;
			for(auto txn: block.txns)
			{	
				if(txn.sendID != -1)		//coin base
					balance[txn.sendID] -= txn.amount;
				balance[txn.recvID] += txn.amount;
			}

			curr_blkID = block.blockID;
			*long_change = true;			// new block has higher depth
		}

		return true;

	}

	return false;
}

int User::common_ancestor(Block block)
{
	if(block.p_blockID == curr_blkID)
		return curr_blkID;

	vector<int> path1, path2;
	int curr_id = block.blockID;
	while(curr_id != -1)
	{
		path1.push_back(curr_id);
		curr_id = blockchain[curr_id].p_blockID;
	}

	curr_id = curr_blkID;
	while(curr_id != -1)
	{
		path2.push_back(curr_id);
		curr_id = blockchain[curr_id].p_blockID;
	}

	int i = path1.size() - 1;
	int j = path2.size() - 1;
	while(i == j){
		i--;
		j--;
	}

	// should be defined, since last element is common anyways
	return path1[i+1];
}

vector<int> User::update_balance(Block block)
{
	// returns balance upto parent block of input block

	vector<int> new_balance = balance;
	int ancestorID = common_ancestor(block);
	int curr_id = curr_blkID;
	while(curr_id != ancestorID){
		for(auto txn: blockchain[curr_id].txns){
			new_balance[txn.sendID] += txn.amount;
			new_balance[txn.recvID] -= txn.amount;
		}
		curr_id = blockchain[curr_id].p_blockID;
	}

	curr_id = block.p_blockID;
	while(curr_id != ancestorID){
		for(auto txn: blockchain[curr_id].txns){
			new_balance[txn.sendID] -= txn.amount;
			new_balance[txn.recvID] += txn.amount;
		}
		curr_id = blockchain[curr_id].p_blockID;
	}

	return new_balance;

}

vector<Transaction> User::choose_transaction()
{
	// picks random min(999, txn_pool.size()) elements for new block generation
	// updates transaction pool appropriately

	vector<int> keys;
	for(auto kv: txn_pool)
		keys.push_back(kv.first);

	mt19937 gen(rd());
	shuffle(keys.begin(), keys.end(), gen);

	vector<Transaction> txns;
	for(int i=0; i<min(999, keys.size()); i++)
		txns.push_back(txn_pool[keys[i]]);

	return txns;
}