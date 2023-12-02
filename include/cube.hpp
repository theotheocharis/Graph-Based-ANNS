#ifndef CUBE_HPP
#define CUBE_HPP

#include <iostream>
#include <vector>
#include <utility>
#include <map>
#include <list>
#include <string>
#include <algorithm>
#include <fstream>
#include <queue>

#include "hash_table.hpp"
#include "hash_function.hpp"

class HyperCube {
private:

    int k;
    int M;
    int probes;
    int N;
    int R;
    int w;

    HashTable *cube;
    std::vector<std::pair<HashFunction *, std::unordered_map<uint, char>*>> vertices;
    std::vector<Image *> *data;

    double MAF;

    std::ofstream output;

public:

    HyperCube(int, int, int, int, int, std::vector<Image *> *data, std::string);
    ~HyperCube();

    std::string project(std::vector<double> *);

    void insert(void *);
    void query(void *);

    std::priority_queue<double, std::vector<double>, std::greater<>> getTrueNeighbors(void *);

    void outputResults(std::vector<std::pair<uint, double>>, std::priority_queue<double, std::vector<double>, std::greater<>>, const std::list<uint>&, void *, double, double);
    void outputMAF();

};

#endif