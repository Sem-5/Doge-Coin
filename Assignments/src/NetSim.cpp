#include "NetSim.hpp"
#include "Random.hpp"
#include <algorithm>
#include <math.h>

#define CMAX 20

NetSim::NetSim(int numNodes, double fracSlow,  double txnparam, double blockparam) :
 nNodes(numNodes), nodes(numNodes), currTime(0.0), TxnGen(txnparam), blockGen(blockparam*numNodes)
{
    /* Update fast and slow nodes */
    int nSlow = fracSlow * numNodes;
    std::vector<bool> isFast(numNodes);
    for (int i = nSlow; i < numNodes; ++i)
        isFast[i] = true;
    std::shuffle(isFast.begin(), isFast.end(), std::mt19937{std::random_device{}()});
    /* Set mining speed : uniform distribution */
    for (int i = 0; i < numNodes; ++i)
    {
        nodes[i].setID(i);
        nodes[i].setSpeed(isFast[i]);
        nodes[i].setMineSpeed(Random::unif_real(3.0*blockGen/5.0, 7.0*blockGen/5.0));
    }
    /* Random network generation with poisson node parameters */
    {
        double param = (log10(numNodes) / numNodes);
        std::vector<int> nodeID({0});
        for (int node = 1; node < numNodes; node++)
        {
            double k = param * node;
            int deg = Random::poisson(k);
            while (deg > node || deg == 0)
                deg = Random::poisson(k);
            for (auto peer : Random::sample(nodeID, deg))
            {
                nodes[node].addPeer(peer);
                nodes[peer].addPeer(node);
            }
            nodeID.push_back(node);
        }
    }
    /* Speed of light delays from uniform distribution */
    for (int i = 0; i < numNodes; ++i)
        for (int j : nodes[i].getPeers())
            solDelay[pii(i, j)] = solDelay[pii(j, i)] = Random::unif_real(10, 500);
}

/* Gets random delays by pij + |m|/cij + Exp(96/cij) */
double NetSim::getDelay(int send, int recv, int nkbits)
{
    double delay = solDelay[pii(send, recv)];
    bool fast = nodes[send].getSpeed() && nodes[recv].getSpeed();
    delay += nkbits*(fast ? 0.01 : 0.2);
    delay += Random::exponential(96.0*(fast ? 0.01 : 0.2));
    return delay;
}

void NetSim::simulate(double endTime)
{
    /* Initial setup */
    for (int i = 0; i < nNodes; ++i)
    {
        double t = Random::exponential(getTxnGen());
        int node;
        do
        {
            node = Random::unif_int(0, nNodes-1);
        } while (node == i);
        int amt = Random::unif_int(1, CMAX);
        addEvent(t, new TxnEvent(i, i, Txn(i, node, amt), this));
    }
    for (int i = 0; i < nNodes; ++i)
    {
        double t = Random::exponential(nodes[i].getMineSpeed());
        addEvent(t, new BlkEvent(i, i, nodes[i].mine(), this));
    }
    /* Event loop */
    while ((currTime < endTime) && (!eventQueue.empty()))
    {
        auto it = eventQueue.begin();
        Event* next = it->second;
        currTime = it->first;
        next->run();
        delete(next);
        eventQueue.erase(it);
    }
    /* Cleaning up remaining events */
    for (auto it : eventQueue)
    {
        delete(it.second);
    }
    /* Node Logging */
    for (auto node : nodes)
    {
        node.print();
    }
}