#ifndef NetSim_H
#define NetSim_H

#include <vector>
#include <utility>
#include <map>
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

class NetSim
{
private:
    int nNodes;
    std::vector<Node> nodes;
    std::map<pii, double> solDelay;
    std::multimap<double, Event*> eventQueue;
    double currTime, TxnGen, blockGen;

public:
    NetSim(int numNodes, double fracSlow, double txnparam, double blockparam);
    double getDelay(int send, int recv, int nkbits);
    void simulate(double endTime);

    /* Helpers */
    inline Node& getNodeRef(int index) { return nodes[index]; }
    inline void addEvent(double t,Event *e) { eventQueue.insert( std::make_pair(t,e) ); }
    inline double getCurrTime() { return currTime; }
    inline double getTxnGen() { return TxnGen; }
    inline int getNum() { return nNodes; }
};

#endif