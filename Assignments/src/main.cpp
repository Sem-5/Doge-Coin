#include "NetSim.hpp"

int main()
{
    NetSim ns(5, 0.25, 1.25, 1e3, 10e3);
    ns.simulate(100e3);
}