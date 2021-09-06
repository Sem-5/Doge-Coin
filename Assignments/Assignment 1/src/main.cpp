#include <iostream>
#include "env.h"

using namespace std;

int main()
{
	vector<double> txn_means{0.5, 0.5};
	vector<double> blk_means{0.5, 0.5};
	Simulator sim(2, 1, txn_means, blk_means, 10.0);
}