#include <random>

// Just contains some distribution functions needed for implementation

double unif_real(int a, int b)
{
	static random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<double> uni(a, b);
	return uni(gen);
}

int unif_int(int a, int b)
{
	static random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> uni(a, b);
	return uni(gen);
}


int poisson(double mean)
{
	static random_device rd;
	mt19937 gen(rd());
	poisson_distribution<int> pois(mean);
	return pois(gen);
}

int exponential(double mean)
{
	static random_device rd;
	mt19937 gen(rd());
	exponential_distribution<double> exp(mean);
	return exp(gen);
}