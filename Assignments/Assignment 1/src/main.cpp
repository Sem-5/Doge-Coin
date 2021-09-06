#include <iostream>
#include "env.h"

using namespace std;

int main()
{
	int n;
	double z, end_time;
	cin>>n>>z>>end_time;

	if(z<0 || z>1){
		cout<<"z should be in between 0 and 1"<<endl;
		exit(EXIT_FAILURE);
	}

	vector<double> txn_means(n), blk_means(n);
	for(int i =0; i<n; i++)
		cin>>txn_means[i];
	for(int i = 0; i < n; i++)
		cin>>blk_means[i];

	Simulator sim(n, z, txn_means, blk_means, end_time);
	return 0;
}