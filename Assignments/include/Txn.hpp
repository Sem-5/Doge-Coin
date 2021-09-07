#ifndef Txn_H
#define Txn_H

#include "getID.hpp"
#include <fstream>

class Txn
{
private:
    int id;
    int send;
    int recv;
    int amt;

public:
    Txn(int send, int recv, int amt) : 
    send(send), recv(recv), amt(amt), id(getID()) {}
    Txn() : id(-1), send(-1), recv(-1), amt(0) {}
    inline int ID() { return id; }
    inline int Sender() { return send; }
    inline int Receiver() { return recv; }
    inline int Amount() { return amt; }
};

#endif