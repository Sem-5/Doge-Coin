#ifndef Block_H
#define Block_H

#define BLK_FILE "block.txt"

#include "getID.hpp"
#include "Txn.hpp"
#include <vector>
#include <fstream>
#include <unordered_map>

class Block
{
private:
    int id;
    int phash;
    int depth;
    double arrivalTime;
    int minerID;
    std::vector<Txn> txns; // no coinbase - work with minerID 
    std::unordered_map<int,int> balanceSheet;

public:
    /*
     depth will not be set!! set when adding to chain
     no need of coinbase txn, uses minerID for that
    */
    Block(std::vector<Txn> txns, int phash, int minerID) : 
    phash(phash), txns(txns), id(getID()), depth(-1), minerID(minerID)
    {
        std::ofstream ofd;
        ofd.open(BLK_FILE, std::ios::app);
        ofd << "Block " << id << " : ";
        for (auto tx : txns)
            ofd << tx.ID() << " ";
        ofd << std::endl;
        ofd.close();
        
        /* update balanceSheet */
        balanceSheet[minerID] += 50;
        for (auto txn : txns)
        {
            balanceSheet[txn.Receiver()] += txn.Amount();
            balanceSheet[txn.Sender()] -= txn.Amount();
        }

    }
    // use ONLY for genesis block creation
    Block() : id(0), phash(-1), depth(0), minerID(-1) {}
    
    inline int ID() { return id; }
    inline int Depth() { return depth; }
    inline int Parent() { return phash; }
    inline const std::vector<Txn>& Txns() { return txns; }
    inline const std::unordered_map<int,int>& Balance() { return balanceSheet; }
    inline void setDepth(int d) { depth = d; }
    inline int size() { return txns.size()+1; } // in KB
    inline int Miner() { return minerID; }
    inline void setArrivalTime(double t) { arrivalTime = t; }
    inline double getArrivalTime() { return arrivalTime; }
};

#endif