#include "NetSim.hpp"

int main()
{
    NetSim ns(20, 0.5, 5e3, 60e3);
    ns.simulate(600e3);
}