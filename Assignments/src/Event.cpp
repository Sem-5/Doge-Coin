#include "NetSim.hpp"
#include "Random.hpp"
#include <iostream> // debugging

#define CMAX 20

void TxnEvent::run()
{
    Node& rNode = simulator->getNodeRef(recv);
    if ( ! rNode.qeuryTxn(txn.ID()) )
    {
        rNode.addTxn(txn.ID());
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
                node = Random::unif_int(0, simulator->getNum());
            } while(node == recv);
            int amt = Random::unif_int(1, CMAX);
            simulator->addEvent(t, new TxnEvent(recv, recv, Txn(recv, node, amt), simulator));
        }
    }
}