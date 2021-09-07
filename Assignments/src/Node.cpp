#include "Node.hpp"
#include <stack>

#define LOGDIR std::string("logs/")

bool Node::modifyChain(int blkid)
{
    /* Find common ancestor in blockchain */
    int lca = blkid;
    std::stack<int> newBlocks;
    while (BlockChain.find(lca) == BlockChain.end())
    {
        newBlocks.push(lca);
        lca = BlockTree[lca].Parent();
    }

    /* Backup if the new chain is invalid*/
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

    /* Add new blocks from lca */
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
        return 0;
    if (BlockTree.find(pid) == BlockTree.end()) 
        return 0;
    BlockTree[id] = blk;
    BlockTree[id].setDepth(BlockTree[pid].Depth() + 1);
    if (BlockTree[id].Depth() > chainLen)
        return modifyChain(id);
    return 2;
}

Block Node::mine()
{
    nMined++;
    /* Picks max possible transactions from pool and tries to create meaningful block with them */
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

/* Logger functin. Read README for details. */
void Node::print()
{
    std::ofstream ofd;
    ofd.open(LOGDIR + std::to_string(getID()) + "_TREE.log", std::ios::ate);
    ofd << mineSpeed << " " << isFast << " " << nMined << std::endl;
    for (auto [x,y] : BlockTree)
    {
        ofd << y.getArrivalTime() << " " << x << " " << y.Parent() 
            << " " << y.Txns().size() << " " << y.Miner() << std::endl;
    }
    ofd.close();
    std::stack<std::string> chain;
    std::stack<int> chainID;
    int tail = chainLast;
    while(tail != -1)
    {
        std::string block_log = "Blk ID : " + std::to_string(BlockTree[tail].ID()) + "\n";
        if (tail == 0)
            block_log += "Genesis block\n";
        else
        {
            block_log +=  std::to_string(BlockTree[tail].Miner()) + " mines 50 coins\n";
            for (auto txn : BlockTree[tail].Txns())
            {
                block_log += "TxnID " + std::to_string(txn.ID()) + " : " +
                            std::to_string(txn.Sender()) + " pays " + std::to_string(txn.Receiver())
                            + " " + std::to_string(txn.Amount()) + " coins\n";
            }
        }
        chain.push(block_log);
        chainID.push(tail);
        tail = BlockTree[tail].Parent();
    }
    ofd.open(LOGDIR + std::to_string(getID()) + "_TNX.log", std::ios::ate);
    while(!chain.empty())
    {
        ofd << chain.top() << std::endl;
        chain.pop();
    }
    ofd.close();
    ofd.open(LOGDIR + std::to_string(getID()) + "_CHAIN.log", std::ios::ate);
    while(!chainID.empty())
    {
        ofd << chainID.top() << " " << BlockTree[chainID.top()].Miner() << std::endl;
        chainID.pop();
    }
    ofd.close();
}