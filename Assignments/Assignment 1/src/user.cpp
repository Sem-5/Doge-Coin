#include "user.h"

bool User::is_valid_txn(Transaction txn){
	// Function returns true only if the transaction is not present in lru of txn's
	// If block seen before, returning false
	// Updates lru
	// Use the functions output to decide whether to broadcast or not

	if(txn_ptrs.find(txn.txnID) == txn_ptrs.end()) 
	{

		txn_pool.insert({txn.txnID, txn});

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
		std::vector<int> new_balance = update_balance(block);

		// check total money sent by each user
		std::vector<int> spent(balance.size(), 0);
		for(auto txn: block.txns)
		{
			if(txn.sendID != -1){		//coin base
				spent[txn.sendID] += txn.amount;

				if(spent[txn.sendID] > new_balance[txn.sendID])
					return false;
			}
		}

		// reached here -> valid block, update blockchain and add this ID to parent block
		blockchain.insert({block.blockID, block});
		if(block.p_blockID != -1){
			auto itr = blockchain.find(block.p_blockID);
			itr->second.add_child(block.blockID);
		}

		// update transaction pool, erase the transactions from the pool
		for(auto txn: block.txns)
		{
			if(txn_pool.find(txn.txnID) != txn_pool.end())
				txn_pool.erase(txn.txnID);
		}

		// check if new added block has higher depth
		if(block.depth > blockchain.find(curr_blkID)->second.depth)
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

	std::vector<int> path1, path2;
	int curr_id = block.blockID;
	while(curr_id != -1)
	{
		path1.push_back(curr_id);
		curr_id = blockchain.find(curr_id)->second.p_blockID;
	}

	curr_id = curr_blkID;
	while(curr_id != -1)
	{
		path2.push_back(curr_id);
		curr_id = blockchain.find(curr_id)->second.p_blockID;
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

std::vector<int> User::update_balance(Block block)
{
	// returns balance upto parent block of input block

	std::vector<int> new_balance = balance;
	int ancestorID = common_ancestor(block);
	int curr_id = curr_blkID;
	while(curr_id != ancestorID){
		for(auto txn: blockchain.find(curr_id)->second.txns){
			new_balance[txn.sendID] += txn.amount;
			new_balance[txn.recvID] -= txn.amount;
		}
		curr_id = blockchain.find(curr_id)->second.p_blockID;
	}

	curr_id = block.p_blockID;
	while(curr_id != ancestorID){
		for(auto txn: blockchain.find(curr_id)->second.txns){
			new_balance[txn.sendID] -= txn.amount;
			new_balance[txn.recvID] += txn.amount;
		}
		curr_id = blockchain.find(curr_id)->second.p_blockID;
	}

	return new_balance;

}

std::vector<Transaction> User::choose_txns()
{
	// picks random min(999, txn_pool.size()) elements for new block generation
	// updates transaction pool appropriately
	static std::random_device rd;

	std::vector<int> keys;
	for(auto kv: txn_pool)
		keys.push_back(kv.first);

	std::mt19937 gen(rd());
	shuffle(keys.begin(), keys.end(), gen);

	std::vector<Transaction> txns;
	for(int i=0; i<std::min(999, (int)keys.size()); i++)
		txns.push_back(txn_pool.find(keys[i])->second);

	return txns;
}