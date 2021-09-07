#include "Node.h"
#include <iostream> // debug
#include <stack>

bool Node::modifyChain(int blkid)
{
    int lca = blkid;
    std::stack<int> newBlocks;
    while (BlockChain.find(lca) == BlockChain.end())
    {
        newBlocks.push(lca);
        lca = BlockTree[lca].Parent();
    }

    /* Backup */
    std::unordered_map<int, int> delUTXO;
    std::unordered_map<int, int> delInChain;
    std::unordered_map<int, int> delPool;

    std::stack<int> newBlocksCopy = newBlocks;

    /* Revert till lca */
    int tail = chainLast;
    while (tail != lca)
    {
        for (auto x : BlockTree[tail].Balance())
        {
            UTXO[x.first] -= x.second;
            delUTXO[x.first] -= x.second;
        }
        for (auto t : BlockTree[tail].Txns())
        {
            TxnPool.insert(t.ID());
            delPool[t.ID()]++;
            TxnInChain.erase(t.ID());
            delInChain[t.ID()]--;
        }
        tail = BlockTree[tail].Parent();
    }

    /* Add up from lca */
    bool valid = true;
    while (!newBlocks.empty())
    {
        tail = newBlocks.top();
        newBlocks.pop();
        for (auto x : BlockTree[tail].Balance())
        {
            UTXO[x.first] += x.second;
            delUTXO[x.first] += x.second;
            if (UTXO[x.first] < 0)
            {
                valid = false;
                break;
            }
        }
        for (auto t : BlockTree[tail].Txns())
        {
            if (TxnInChain.find(t.ID()) != TxnInChain.end())
            {
                valid = false;
                break;
            }
            TxnInChain.insert(t.ID());
            delInChain[t.ID()]++;
            TxnPool.erase(t.ID());
            delPool[t.ID()]--;
        }

        if(!valid)
        {
            std::cout << "Block " << tail << " is invalid, aborting" << std::endl;
            break;
        }
    }
    
    if (!valid)
    {
        /* Kill blockchain upto tail */
        newBlocks.push(tail);
        while(!newBlocks.empty())
        {
            BlockTree.erase(newBlocks.top());
            newBlocks.pop();
        }
        /* Restore State */
        for (auto [x,y] : delUTXO)
            UTXO[x] -= y;
        for (auto [x,y] : delInChain)
            if (y < 0)
                TxnInChain.insert(x);
            else if (y > 0)
                TxnInChain.erase(x);
        for (auto [x,y] : delPool)
            if (y < 0)
                TxnPool.insert(x);
            else if (y > 0)
                TxnPool.erase(x);
        return 0;
    }

    /* If here, new chain is legit */
    while(chainLast != lca)
    {
        BlockChain.erase(chainLast);
        chainLast = BlockTree[chainLast].Parent();
    }

    while(!newBlocksCopy.empty())
    {
        BlockChain.insert(newBlocksCopy.top());
        newBlocksCopy.pop();
    }

    std::cout << "New block added successfully" << std::endl;
    chainLast = blkid;
    chainLen = BlockTree[blkid].Depth();
    return 1;
}

int Node::recvBlock(Block blk)
{
    TransmitID.insert(blk.ID());
    int id = blk.ID();
    int pid = blk.Parent();
    if (BlockTree.find(id) != BlockTree.end()) 
    {
        std::cout << "Have seen block " << id << " before" << std::endl; 
        return 0;
    }
    if (BlockTree.find(pid) == BlockTree.end()) 
    {
        std::cout << "Cannot identify parent " << pid << " of " << id << std::endl;
        return 0;
    }
    BlockTree[id] = blk;
    BlockTree[id].setDepth(BlockTree[pid].Depth() + 1);
    if (BlockTree[id].Depth() > chainLen)
    {
        std::cout << "Length exceeded - running modify" << std::endl;
        return modifyChain(id);
    }
    return 2;
}

Block Node::mine()
{
    std::cout << "Mining on " << chainLast << std::endl;
    int ntxn = std::min( (size_t)999, TxnPool.size());
    std::vector<Txn> txns;
    std::unordered_set<int>::iterator it;
    if (!TxnPool.empty()) it = TxnPool.begin();
    std::unordered_map<int,int> expense;
    while (ntxn--)
    {
        Txn txn = TxnMap[*it];
        expense[txn.Sender()] += txn.Amount();
        expense[txn.Receiver()] -= txn.Amount();
        if (expense[txn.Sender()] <= UTXO[txn.Sender()]) 
            txns.push_back(txn);
        else
        {
            expense[txn.Sender()] -= txn.Amount();
            expense[txn.Receiver()] += txn.Amount();
        }
        std::advance(it, 1);
    }
    return Block(txns, chainLast, id);
}