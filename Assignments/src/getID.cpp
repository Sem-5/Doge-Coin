#include "getID.hpp"

int getID()
{
    static int id = 1;
    return id++;
}