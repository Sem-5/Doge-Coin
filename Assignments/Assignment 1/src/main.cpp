#include <iostream>
#include "env.h"

using namespace std;

int main()
{
	vector<double> txn_means{0.5};
	vector<double> blk_means{0.5};
	Simulator sim(1, 1, txn_means, blk_means, 4.0);
	// for(int i =0; i<20; i++)
	// 	cout<<exponential(0.5)<<endl;
}