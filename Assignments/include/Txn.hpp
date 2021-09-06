#ifndef Txn_H
#define Txn_H

#define TXN_FILE "transaction.txt"
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
    Txn(int send, int recv, int amt) : send(send), recv(recv), amt(amt), id(getID())
    {
        std::ofstream ofd;
        ofd.open(TXN_FILE, std::ios::app);
        ofd << "Transaction " << id << " : " << send << " pays " << recv << " " << amt << " coins" << std::endl;
        ofd.close();
    }
    inline int ID() { return id; }
    inline int Sender() { return send; }
    inline int Receiver() { return recv; }
    inline int Amount() { return amt; }
};

#endif