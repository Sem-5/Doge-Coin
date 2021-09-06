#ifndef NetSim_H
#define NetSim_H

#include <vector>
#include <utility>
#include <map>
#include <queue>
#include "Node.h"
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

class NetSim
{
private:
    int nNodes;
    std::vector<Node> nodes;
    std::map<pii, double> solDelay;
    double slowTR, fastTR;
    std::multimap<double, Event*> eventQueue;
    double currTime, TxnGen;

public:
    NetSim(int numNodes, double fracSlow, double howFast, double txnparam);
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