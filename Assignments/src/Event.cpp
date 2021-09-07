#include "NetSim.hpp"
#include "Random.hpp"
#include <iostream> // debugging

#define CMAX 20

void TxnEvent::run()
{
    std::cout << "TxnID " << txn.ID() << " from " << send << " to " << recv 
              << " at " << simulator->getCurrTime() << "ms" << std::endl;
    Node& rNode = simulator->getNodeRef(recv);
    if ( ! rNode.qeury(txn.ID()) )
    {
        rNode.addTxn(txn);
        for (auto node : rNode.getPeers())
            if (node != send)
            {
                double t = simulator->getCurrTime() + simulator->getDelay(recv, node, 8);
                simulator->addEvent(t, new TxnEvent(recv, node, txn, simulator));
            }
        // If this was a self broadcast event
        if (send == recv)
        {
            double t = simulator->getCurrTime() + Random::exponential(simulator->getTxnGen());
            int node;
            do {
                node = Random::unif_int(0, simulator->getNum()-1);
            } while(node == recv);
            int amt = Random::unif_int(1, CMAX);
            simulator->addEvent(t, new TxnEvent(recv, recv, Txn(recv, node, amt), simulator));
        }
    }
}

void BlkEvent::run()
{
    std::cout << "BlkID " << blk.ID() << " from " << send << " to " << recv 
              << " at " << simulator->getCurrTime() << "ms" << std::endl;
    Node& rNode = simulator->getNodeRef(recv);
    if ( ! rNode.qeury(blk.ID()) )
    {
        for (auto node : rNode.getPeers())
            if (node != send)
            {
                double t = simulator->getCurrTime() + simulator->getDelay(recv, node, 8*blk.size());
                simulator->addEvent(t, new BlkEvent(recv, node, blk, simulator));
            }

        if ( rNode.recvBlock(blk) )
        {
            // If this created a new chain
            double t = simulator->getCurrTime() + Random::exponential(rNode.getMineSpeed());
            simulator->addEvent(t, new BlkEvent(recv, recv, rNode.mine(), simulator));
        }
    }
}