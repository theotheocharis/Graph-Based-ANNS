#ifndef MRNG_HPP
#define MRNG_HPP

#include <iostream>
#include <vector>

#include "image.hpp"
#include "lsh.hpp"

class MRNG {
private:
    int N;
    int l;

    std::vector<Image *> *data;
    // Graph representation as adjacency list
    std::vector<std::vector<Image *> *> *graph;

    Image *startingNode;

public:

    MRNG(int, int, std::vector<Image *> *);

    void constructGraph();

    std::vector<double> *findCentroid();
    void findStartingNode();

    std::vector<std::vector<Image *>*>* getGraph();
    Image *getStartingNode();

    ~MRNG();

};

#endif