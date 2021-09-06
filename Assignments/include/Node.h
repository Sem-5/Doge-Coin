#ifndef Node_H
#define Node_H

#include "Block.hpp"
#include <vector>
#include <unordered_set>
#include <unordered_map>

class Node
{
private:
    bool isFast;
    std::vector<int> peerID;
    std::unordered_set<int> TxnID; // have broadcasted before

    std::unordered_set<int> TxnPool; // not in the longest blockchain
    std::unordered_map<int, Block> BlockTree;
    std::unordered_set<int> BlockChain; // id of each block in current chain
    std::unordered_set<int> TxnInChain; // transactions in chain
    std::unordered_map<int, int> UTXO; // remaining coins at each node (by current longest)
    int chainLen, chainLast;

    void modifyChain(int blkid);

public:
    Node() : chainLen(1), chainLast(0), BlockChain({0}) { BlockTree[0] = Block(); }
    
    /* Transaction forwarding related functions  */
    void setSpeed(bool fast) { isFast = fast; }
    bool getSpeed() const { return isFast; }
    void addPeer(int peer) { peerID.push_back(peer); }
    const std::vector<int> & getPeers() const { return peerID; }
    void addTxn (int txn) 
    { 
        TxnID.insert(txn);
        if (TxnInChain.find(txn) == TxnInChain.end())
            TxnPool.insert(txn); 
    }
    bool qeuryTxn(int txn) const { return TxnID.find(txn) != TxnID.end(); }

    /* Blockchain related functions */
    void recvBlock(Block blk);

};

#endif