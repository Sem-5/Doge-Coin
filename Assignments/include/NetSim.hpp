#ifndef NetSim_H
#define NetSim_H

#include <vector>
#include <utility>
#include <map>
#include <iostream> // debugging
#include <queue>
#include "Node.hpp"
#include "Txn.hpp"

typedef std::pair<int,int> pii;

class NetSim;

class Event
{
protected:
    NetSim *simulator;
public:
    Event(NetSim* sim) : simulator(sim) {}
    virtual void run() = 0;
};

class TxnEvent : public Event
{
    int send;
    int recv;
    Txn txn;
public:
    TxnEvent(int send, int recv, Txn txn, NetSim* sim) :
    Event(sim), send(send), recv(recv), txn(txn){}
    void run();
};

class BlkEvent : public Event
{
    int send;
    int recv;
    Block blk;
public:
    BlkEvent(int send, int recv, Block blk, NetSim* sim) :
    Event(sim), send(send), recv(recv), blk(blk) {}
    void run();
};

class PrivBlkEvent : public Event 
{
    int send;
    int recv;
    Block blk;
public:
    PrivBlkEvent(int send, int recv, Block blk, NetSim* sim) :
    Event(sim), send(send), recv(recv), blk(blk) {}
    void run();
};

class ChainEvent : public Event
{
    int send;
    int recv;
    std::list<Block> chain;
    int chainSize;
public:
    ChainEvent(int send, int recv, std::list<Block> chain, int chainSize, NetSim* sim) :
        Event(sim), send(send), recv(recv), chain(chain), chainSize(chainSize) {}
    void run();
};

class NetSim
{
private:
    int honNodes;
    int nNodes;
    std::vector<Node*> nodes;
    std::map<pii, double> solDelay;
    std::multimap<double, Event*> eventQueue;
    double currTime, TxnGen, blockGen;
    int advType;

public:
    /*
        numNodes : number of honest miners
        txnparam : specified per node  
        blockparam : specified for whole chain  
        advType : 0 no adversary, 1 selfish miner, 2 stubborn miner 
        fracAdvConn : fraction of honest miners connected to adversary
    */
    NetSim(int numNodes, double fracSlow, double txnparam, double blockparam, int advType, double fracAdvConn, double advparam);
    double getDelay(int send, int recv, int nkbits);
    void simulate(double endTime);

    /* Helpers */
    inline Node* getNodeRef(int index) { return nodes[index]; }
    inline void addEvent(double t,Event *e) { eventQueue.insert( std::make_pair(t,e) ); }
    inline double getCurrTime() { return currTime; }
    inline double getTxnGen() { return TxnGen; }
    inline int getNum() { return nNodes; }
};

#endif