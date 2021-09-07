#ifndef Node_H
#define Node_H

#include "Block.hpp"
#include <vector>
#include <unordered_set>
#include <unordered_map>

class Node
{
private:
    int id;
    bool isFast;
    double mineSpeed;
    std::vector<int> peerID;
    std::unordered_set<int> TransmitID; // have broadcasted before

    std::unordered_map<int, Txn> TxnMap; // stores the actual transaction ONCE
    std::unordered_set<int> TxnPool; // not in the longest blockchain
    std::unordered_map<int, Block> BlockTree;
    std::unordered_set<int> BlockChain; // id of each block in current chain
    std::unordered_set<int> TxnInChain; // transactions in chain
    std::unordered_map<int, int> UTXO; // remaining coins at each node (by current longest)
    int chainLen, chainLast;

    bool modifyChain(int blkid); // returns 1 on successful modification

public:
    Node() : chainLen(0), chainLast(0), BlockChain({0}) { BlockTree[0] = Block(); }
    
    /* Transaction forwarding related functions  */
    inline void setID( int ID ) { id = ID; }
    inline int getID() { return id; }
    inline void setSpeed(bool fast) { isFast = fast; }
    inline bool getSpeed() const { return isFast; }
    inline double getMineSpeed() const { return mineSpeed; }
    inline void setMineSpeed(double ms) { mineSpeed = ms; }
    inline void addPeer(int peer) { peerID.push_back(peer); }
    inline const std::vector<int> & getPeers() const { return peerID; }
    void addTxn (Txn txn) 
    { 
        TransmitID.insert(txn.ID());
        TxnMap[txn.ID()] = txn;
        if (TxnInChain.find(txn.ID()) == TxnInChain.end())
            TxnPool.insert(txn.ID()); 
    }
    inline bool qeury(int hash) const { return TransmitID.find(hash) != TransmitID.end(); }

    /* Blockchain related functions */
    bool recvBlock(Block blk); // Returns 1 iff new block formed a longer chain
    inline int getMineID() { return chainLast; }
    Block mine();
};

#endif