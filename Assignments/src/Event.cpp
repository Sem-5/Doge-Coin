#include "NetSim.hpp"
#include "Random.hpp"

#define CMAX 20

int tx_c = 0, blk_c = 0, pblk_c = 0, chain_c = 0;

void TxnEvent::run()
{
    tx_c++;
    std::cout << "Txn Events : " << tx_c << ", Blk Events : " << blk_c << ", PBlk Events : " << pblk_c << ", Chain Events : " << chain_c << "\r";
    std::cout << std::flush;
    Node *rNode = simulator->getNodeRef(recv);
    if (!rNode->qeury(txn.ID()))
    {
        rNode->addTxn(txn);
        for (auto node : rNode->getPeers())
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
    blk_c++;
    std::cout << "Txn Events : " << tx_c << ", Blk Events : " << blk_c << ", PBlk Events : " << pblk_c << ", Chain Events : " << chain_c << "\r";
    std::cout << std::flush;
    blk.setArrivalTime(simulator->getCurrTime());
    Node *rNode = simulator->getNodeRef(recv);
    if (!rNode->qeury(blk.ID()))
    {
        if ((send == recv) && (blk.Parent() != rNode->getMineID()))
            return; // outdated block

        int code = rNode->recvBlock(blk);
        
        switch(code){
            case 1:
            case 2:
            {
                for (auto node : rNode->getPeers())
                    if (node != send)
                    {
                        double t = simulator->getCurrTime() + simulator->getDelay(recv, node, 8 * blk.size());
                        simulator->addEvent(t, new BlkEvent(recv, node, blk, simulator));
                    }
                // If this created a new chain mine on it
                if (code & 1)
                {
                    double t = simulator->getCurrTime() + Random::exponential(rNode->getMineSpeed());
                    simulator->addEvent(t, new BlkEvent(recv, recv, rNode->mine(), simulator));
                }
                break;
            }

            case 3: // honest miners took over, start mining on this
            {
                double t = simulator->getCurrTime() + Random::exponential(rNode->getMineSpeed());
                simulator->addEvent(t, new PrivBlkEvent(recv, recv, Block( std::vector<Txn>(), rNode->getMineID(), recv ), simulator));
                break;
            }

            case 4:  // release 1 new block
            {
                Block releaseBlock = rNode->getBlock();
                int ret = rNode->recvBlock(releaseBlock);    // update attackers own tree

                for (auto node : rNode->getPeers())
                    if (node != send)
                    {
                        double t = simulator->getCurrTime() + simulator->getDelay(recv, node, 8 * releaseBlock.size());
                        simulator->addEvent(t, new BlkEvent(recv, node, releaseBlock, simulator));
                    }
                break;
            }

            case 5: // release entire chain and start mining on new
            {  
                std::list<Block> chain = rNode->getChain();
                int totalSize = 0;
                for (auto blk : chain)
                    totalSize += blk.size();

                simulator->addEvent(simulator->getCurrTime(), new ChainEvent(recv, recv, chain, totalSize, simulator));

                double t = simulator->getCurrTime() + Random::exponential(rNode->getMineSpeed());
                simulator->addEvent(t, new PrivBlkEvent(recv, recv, Block( std::vector<Txn>(), rNode->getMineID(), recv ), simulator));

                break;
            }

            default: break;
        }
    }
}

void PrivBlkEvent::run()
{
    pblk_c++;
    std::cout << "Txn Events : " << tx_c << ", Blk Events : " << blk_c << ", PBlk Events : " << pblk_c << ", Chain Events : " << chain_c << "\r";
    std::cout << std::flush;
    blk.setArrivalTime(simulator->getCurrTime());
    Node *rNode = simulator->getNodeRef(recv);
    if ( blk.Parent() != rNode->getMineID() )
        return;

    if (rNode->getRace())        // if race condition and won, send out block
    {
        for (auto node : rNode->getPeers())
            if (node != send)
            {
                double t = simulator->getCurrTime() + simulator->getDelay(recv, node, 8 * blk.size());
                simulator->addEvent(t, new BlkEvent(recv, node, blk, simulator));
            }
        rNode->updateRace(blk.Miner());
    }  
    else
        rNode->extendChain(blk);

    // mine for next block 
    double t = simulator->getCurrTime() + Random::exponential(rNode->getMineSpeed());
    rNode->incrCount();
    simulator->addEvent(t, new PrivBlkEvent(recv, recv, Block( std::vector<Txn>(), rNode->getMineID(), recv ), simulator));  
}

void ChainEvent::run() 
{
    chain_c++;
    std::cout << "Txn Events : " << tx_c << ", Blk Events : " << blk_c << ", PBlk Events : " << pblk_c << ", Chain Events : " << chain_c << "\r";
    std::cout << std::flush;
    for(auto &blk : chain)
        blk.setArrivalTime(simulator->getCurrTime());

    Node *rNode = simulator->getNodeRef(recv);
    if (!rNode->qeury(chain.front().ID()))
    {   
        int code = 0;
        for(auto &blk : chain)
            code = rNode->recvBlock(blk);

        if(code)
        {
            for (auto node : rNode->getPeers())
                if (node != send)
                {
                    double t = simulator->getCurrTime() + simulator->getDelay(recv, node, 8 * chainSize);
                    simulator->addEvent(t, new ChainEvent(recv, node, chain, chainSize, simulator));
                }  
                
            if (code & 1)
            {
                double t = simulator->getCurrTime() + Random::exponential(rNode->getMineSpeed());
                simulator->addEvent(t, new BlkEvent(recv, recv, rNode->mine(), simulator));
            } 
        }
    }
}