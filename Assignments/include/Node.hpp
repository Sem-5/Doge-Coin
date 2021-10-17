#ifndef Node_H
#define Node_H

#include "Block.hpp"
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <list>

class Node
{
protected:
    int id;
    bool isFast;
    double mineSpeed;
    int nMined;
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
    inline int getChainLast() { return chainLast; }
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

    Block mine();

    /* Print Tree */
    void print();

    /* Virtual functions, (is there something better??) */
    virtual int recvBlock(Block blk) { throw std::runtime_error("recvBlock : this should never happen"); return 0; };
    virtual int getMineID() { throw std::runtime_error("getMineID : this should never happen"); return 0; };
    virtual bool getRace() { throw std::runtime_error("getRace : this should never happen"); return 0; };
    virtual void setLead() { throw std::runtime_error("setLead : this should never happen"); };
    virtual void updateRace(int parentID) { throw std::runtime_error("updateRace : this should never happen");};
    virtual Block getBlock() { throw std::runtime_error("getBlock : this should never happen"); return Block(); };
    virtual std::list<Block> getChain() { throw std::runtime_error("getChain : this should never happen"); return std::list<Block>(); };
    virtual void extendChain(Block blk) { throw std::runtime_error("extendChain : this should never happen"); };
};

class HonestMiner : public Node
{
public:
    int recvBlock(Block blk);  // Returns 0 if invalid, 1 if forms new chain, 2 if valid but not in new chain
    inline int getMineID() { return getChainLast(); }

    // inline bool getRace() {return false; }
    // inline void setLead() {}
    // inline void updateRace(int parentID) {}
    // inline Block getBlock() { return Block(); };
    // inline std::list<Block> getChain() { return std::list<Block>(); }
    // inline void extendChain(Block blk) {}


};

class Attacker : public Node 
{
protected:
    std::list<Block> privateChain;
    int lead;
    bool race;          // in race condition, aka state 0'
    int parentID;         // parent in private chain
public:
    virtual int recvBlock(Block blk) { throw std::runtime_error("Attacker::recvBlock : this should never happen"); return 0; };   // 0 if invalid, 3 if no 

    Attacker() : Node(), lead(0), race(false), parentID(0) {};
    inline int getMineID() { return parentID; }
    inline bool getRace() { return race; }
    inline void setLead() { lead = 1; }
    inline void updateRace(int parentID) {parentID = parentID; race = false;}

    inline Block getBlock() { Block block = privateChain.front(); privateChain.pop_front(); return block; }
    inline std::list<Block> getChain() { std::list<Block> temp = privateChain; privateChain = std::list<Block>(); return temp; }
    void extendChain(Block blk);

    
};

class SelfishMiner : public Attacker
{
public:
    int recvBlock(Block blk);
};

class StubbornMiner : public Attacker
{
public:
    int recvBlock(Block blk);
};

#endif