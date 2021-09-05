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
		// check total money sent by each user
		vector<int> spent(balance.size(), 0);
		for(auto txn: block.txns)
		{
			if(txn.sendID != -1){		//coin base
				spent[txn.sendID] += txn.amount;

				if(spent[txn.sendID] > balance[txn.sendID])
					return false;
			}
		}

		// update balance of each user
		for(auto txn: block.txns)
		{	
			if(txn.sendID != -1)		//coin base
				balance[txn.sendID] -= txn.amount;
			balance[txn.recvID] += txn.amount;
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
			update_balance();
			curr_blkID = block.blockID;
			*long_change = true;			// new block has higher depth
		}

		return true;

	}

	return false;
}

void User::update_balance(Block block)
{
	// block is the new deepest node
	// need to change balance of users accordingly
	// 2 options: compute balance each time on the fly
	// OR do lca algo, 2 traversals from block and curr_blkID to genesis and find common ID

		// NEED TO IMPLEMENT LCA BETWEEN CURR_BLKID AND PASSED BLOCK
		// CURR_BLK -> ANCESTOR -> BLOCK UPDATE POOL ACCORDINGLY
}

vector<Transaction> User::choose_transaction()
{
	// picks random min(999, txn_pool.size()) elements for new block generation
	// updates transaction pool appropriately
		// TO IMPLEMENT
		// USER CHOOSES min(999, txn_pool) NUMBER OF ELEMENTS FROM POOL AT RANDOM
}