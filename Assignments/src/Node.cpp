#include "Node.h"
#include <iostream>
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
            break;
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
        for (auto x : delUTXO)
            UTXO[x.first] -= x.second;
        for (auto x : delInChain)
            if (x.second < 0)
                TxnInChain.insert(x.first);
            else if (x.second > 0)
                TxnInChain.erase(x.first);
        for (auto x : delPool)
            if (x.second < 0)
                TxnPool.insert(x.first);
            else if (x.second > 0)
                TxnPool.erase(x.first);
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

    chainLast = blkid;
    chainLen = BlockTree[blkid].Depth();
    return 1;
}

bool Node::recvBlock(Block blk)
{
    int id = blk.ID();
    int pid = blk.Parent();
    if (BlockTree.find(id) != BlockTree.end()) return 0;
    if (BlockTree.find(pid) == BlockTree.end()) return 0;
    BlockTree[id] = blk;
    BlockTree[id].setDepth(BlockTree[pid].Depth() + 1);
    if (BlockTree[id].Depth() > chainLen)
        return modifyChain(id);
    return 0;
}

Block Node::mine()
{
    int ntxn = std::min( (size_t)999, TxnPool.size());
    std::vector<Txn> txns;
    std::unordered_set<int>::iterator it;
    if (!TxnPool.empty()) it = TxnPool.begin();
    while (ntxn--)
    {
        txns.push_back(TxnMap[*it]);
        std::advance(it, 1);
    }
    return Block(txns, chainLast, id);
}