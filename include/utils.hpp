#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <utility>
#include <set>
#include <chrono>

#include "image.hpp"

double dist(std::vector<double> *, std::vector<double> *);

int euclideanModulo(int, int);

uint binaryToUint(std::string);

std::vector<int> *hammingDistance(const std::string&, int);

bool sortPairBySecond(const std::pair<uint, double> &, const std::pair<uint, double> &);

#endif
