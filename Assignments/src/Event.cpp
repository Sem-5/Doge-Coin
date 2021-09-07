#include "NetSim.hpp"
#include "Random.hpp"

#define CMAX 20

void TxnEvent::run()
{
    Node &rNode = simulator->getNodeRef(recv);
    if (!rNode.qeury(txn.ID()))
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
            do
            {
                node = Random::unif_int(0, simulator->getNum() - 1);
            } while (node == recv);
            int amt = Random::unif_int(1, CMAX);
            simulator->addEvent(t, new TxnEvent(recv, recv, Txn(recv, node, amt), simulator));
        }
    }
}

void BlkEvent::run()
{
    blk.setArrivalTime(simulator->getCurrTime());
    Node &rNode = simulator->getNodeRef(recv);
    if (!rNode.qeury(blk.ID()))
    {
        if ((send == recv) && (blk.Parent() != rNode.getMineID()))
            return; // outdated block

        int code = rNode.recvBlock(blk);
        
        if (code)
        {
            for (auto node : rNode.getPeers())
                if (node != send)
                {
                    double t = simulator->getCurrTime() + simulator->getDelay(recv, node, 8 * blk.size());
                    simulator->addEvent(t, new BlkEvent(recv, node, blk, simulator));
                }
            // If this created a new chain mine on it
            if (code & 1)
            {
                double t = simulator->getCurrTime() + Random::exponential(rNode.getMineSpeed());
                simulator->addEvent(t, new BlkEvent(recv, recv, rNode.mine(), simulator));
            }
        }
    }
}