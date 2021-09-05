#include <random>

double unif()
{
	static random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<double> uni(0, 1);
	return uni(gen);
}
int poisson(double mean)
{
	static random_device rd;
	mt19937 gen(rd());
	poisson_distribution<int> pois(mean);
	return pois(gen);
}