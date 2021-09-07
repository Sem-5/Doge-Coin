#ifndef Random_h
#define Random_h

#include <random>
#include <algorithm>

class Random
{
private:
    Random() {}

public:
    static inline double unif_real(double a, double b)
    {
        static std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> uni(a, b);
        return uni(gen);
    }

    static inline int unif_int(int a, int b)
    {
        static std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> uni(a, b);
        return uni(gen);
    }

    static inline int poisson(double mean)
    {
        static std::random_device rd;
        std::mt19937 gen(rd());
        std::poisson_distribution<int> pois(mean);
        return pois(gen);
    }

    static inline double exponential(double exptime)
    {
        static std::random_device rd;
        std::mt19937 gen(rd());
        std::exponential_distribution<double> exp(1.0/exptime);
        return exp(gen);
    }
    static inline std::vector<int> sample(std::vector<int> &data, int count)
    {
        static std::random_device rd;
        std::mt19937 gen(rd());
        std::vector<int> samples;
        std::sample(data.begin(), data.end(), std::back_inserter(samples), count, gen);
        return samples;
    }
};

#endif