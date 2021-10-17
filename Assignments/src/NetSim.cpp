#include "NetSim.hpp"
#include "Random.hpp"
#include <algorithm>
#include <math.h>

#define CMAX 20

NetSim::NetSim(int numNodes, double fracSlow,  double txnparam, double blockparam, int advType = 0, double fracAdvConn = 0, double advparam = 0) :
    honNodes(numNodes), currTime(0.0), TxnGen(txnparam), advType(advType)
{
    nNodes = numNodes + (advType == 0 ? 0 : 1);
    for (int i = 0; i < numNodes; i++)
        nodes.push_back(new HonestMiner());
    blockGen = blockparam*nNodes;

    /* HONEST PART OF NETWORK */
    /* Update fast and slow nodes */
    int nSlow = fracSlow * numNodes;
    std::vector<bool> isFast(numNodes);
    for (int i = nSlow; i < numNodes; ++i)
        isFast[i] = true;
    std::shuffle(isFast.begin(), isFast.end(), std::mt19937{std::random_device{}()});
    /* Set mining speed : uniform distribution */
    for (int i = 0; i < numNodes; ++i)
    {
        nodes[i]->setID(i);
        nodes[i]->setSpeed(isFast[i]);
        nodes[i]->setMineSpeed(Random::unif_real(3.0*blockGen/5.0, 7.0*blockGen/5.0));
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
                nodes[node]->addPeer(peer);
                nodes[peer]->addPeer(node);
            }
            nodeID.push_back(node);
        }
    }

    /* ADD ADVERSARY */
    if(advType == 0)
        return;

    if(advType==1)
        nodes.push_back(new SelfishMiner());
    if(advType==2)
        nodes.push_back(new StubbornMiner());

    nodes[numNodes]->setID(numNodes);
    nodes[numNodes]->setSpeed(true);
    nodes[numNodes]->setMineSpeed(advparam);

    int connNodes = fracAdvConn * numNodes;
    std::vector<bool> isConn(numNodes);
    for (int i = 0; i < connNodes; ++i)
        isConn[i] = true;
    std::shuffle(isConn.begin(), isConn.end(), std::mt19937{std::random_device{}()});

    for (int i = 0; i < numNodes; i++) 
    {
        if(isConn[i]) 
        {
            nodes[numNodes]->addPeer(i);
            nodes[i]->addPeer(numNodes);
        }
    }

    /* Speed of light delays from uniform distribution */
    for (int i = 0; i < nNodes; ++i)
        for (int j : nodes[i]->getPeers())
            solDelay[pii(i, j)] = solDelay[pii(j, i)] = Random::unif_real(10, 500);

}

/* Gets random delays by pij + |m|/cij + Exp(96/cij) */
double NetSim::getDelay(int send, int recv, int nkbits)
{
    double delay = solDelay[pii(send, recv)];
    bool fast = nodes[send]->getSpeed() && nodes[recv]->getSpeed();
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
    for (int i = 0; i < honNodes; ++i)
    {
        double t = Random::exponential(nodes[i]->getMineSpeed());
        addEvent(t, new BlkEvent(i, i, nodes[i]->mine(), this));
    }
    if(advType != 0) 
    {
        double t = Random::exponential(nodes[honNodes]->getMineSpeed());
        addEvent(t, new PrivBlkEvent(honNodes, honNodes, Block( std::vector<Txn>(), nodes[honNodes]->getMineID(), honNodes ), this));
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
    std::cout << std::endl;
    /* Node Logging */
    for (auto node : nodes)
    {
        node->print();
        delete(node);
    }
}