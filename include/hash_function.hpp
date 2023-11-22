#ifndef HASH_FUNCTION_HPP
#define HASH_FUNCTION_HPP

#include <iostream>
#include <vector>
#include <random>
#include <string>

#include "image.hpp"

class HashFunction {
private:
    double t;
    int w;

    std::random_device rd;
    std::mt19937 generator;
    std::normal_distribution<double> normalDistribution;
    std::uniform_real_distribution<double> uniDistribution;

public:
    HashFunction(int);

    int h(void *);

    int coinFlip();

};

#endif