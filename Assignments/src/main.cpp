#include "NetSim.hpp"

#include <iostream>
#include <sstream>

int main(int argc, char*argv[])
{
    if (argc < 7)
    {
        std::cout << "Format : ./sim nNodes fracSlow TxnTime BlockTime SimulationEnd AdvType [FracAdvConn] [AdvGenTime]" << std::endl;
        return 1;
    }
    int nNodes;
    double fracSlow;
    double TxnTime;
    double BlockTime;
    double SimulationEnd;
    int AdvType;
    double FracAdvConn = 0;
    double AdvGenTime = 0;
    std::stringstream(argv[1]) >> nNodes;
    std::stringstream(argv[2]) >> fracSlow;
    std::stringstream(argv[3]) >> TxnTime;
    std::stringstream(argv[4]) >> BlockTime;
    std::stringstream(argv[5]) >> SimulationEnd;
    std::stringstream(argv[6]) >> AdvType;
    if(AdvType)
    {
        std::stringstream(argv[7]) >> FracAdvConn;
        std::stringstream(argv[8]) >> AdvGenTime;
    }
    
    NetSim ns(nNodes, fracSlow, TxnTime, BlockTime, AdvType, FracAdvConn, AdvGenTime);
    ns.simulate(SimulationEnd);
}