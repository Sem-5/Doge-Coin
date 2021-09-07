#include "NetSim.hpp"

#include <iostream>
#include <sstream>

int main(int argc, char*argv[])
{
    if (argc != 6)
    {
        std::cout << "Format : ./sim nNodes fracSlow TxnTime BlockTime SimulationEnd" << std::endl;
        return 1;
    }
    int nNodes;
    double fracSlow;
    double TxnTime;
    double BlockTime;
    double SimulationEnd;
    std::stringstream(argv[1]) >> nNodes;
    std::stringstream(argv[2]) >> fracSlow;
    std::stringstream(argv[3]) >> TxnTime;
    std::stringstream(argv[4]) >> BlockTime;
    std::stringstream(argv[5]) >> SimulationEnd;
    
    NetSim ns(nNodes, fracSlow, TxnTime, BlockTime);
    ns.simulate(SimulationEnd);
}