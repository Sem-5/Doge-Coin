#include "Node.h"
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <stack>

void Node::modifyChain(int blkid)
{
    int lca = blkid;
    std::stack<int> newBlocks;
    while (BlockChain.find(blkid) == BlockChain.end())
    {
        newBlocks.push(lca);
        lca = BlockTree[lca].Parent();
    }

    /* Backup */
    std::unordered_map<int, int> delUTXO;
    std::unordered_map<int, int> delInChain;
    std::unordered_map<int, int> delPool;

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
        return;
    }

    /* If here, new chain is legit */
    chainLast = blkid;
    chainLen = BlockTree[blkid].Depth();
}

void Node::recvBlock(Block blk)
{
    int id = blk.ID();
    int pid = blk.Parent();
    if (BlockTree.find(id) != BlockTree.end()) return;
    if (BlockTree.find(pid) == BlockTree.end()) return;
    BlockTree[id] = blk;
    BlockTree[id].setDepth(BlockTree[pid].Depth() + 1);
    if (BlockTree[id].Depth() > chainLen)
        modifyChain(id);
}