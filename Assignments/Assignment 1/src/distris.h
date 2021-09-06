#ifndef distris_h
#define distris_h

#include <random>

// Just contains some distribution functions needed for implementation

double unif_real(int a, int b)
{
	static std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> uni(a, b);
	return uni(gen);
}

int unif_int(int a, int b)
{
	static std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> uni(a, b);
	return uni(gen);
}


int poisson(double mean)
{
	static std::random_device rd;
	std::mt19937 gen(rd());
	std::poisson_distribution<int> pois(mean);
	return pois(gen);
}

int exponential(double mean)
{
	static std::random_device rd;
	std::mt19937 gen(rd());
	std::exponential_distribution<double> exp(mean);
	return exp(gen);
}

#endif