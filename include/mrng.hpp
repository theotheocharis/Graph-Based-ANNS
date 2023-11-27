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

    int imageNumber;
    // Graph representation as adjacency list
    std::vector<std::vector<Image *> *> *graph;

public:

    MRNG(int, int, int);

    void constructGraph(std::vector<Image *> *);

    std::vector<std::vector<Image *>*>* getGraph();

    ~MRNG();

};

#endif