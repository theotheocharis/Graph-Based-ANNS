#ifndef GNNS_HPP
#define GNNS_HPP

#include <set>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <iostream>
#include <vector>
#include <string>

#include "../include/image.hpp"
#include "../include/lsh.hpp"


class GNNS {
    private:
        int E, R, N;
        int imageNumber;
        // Graph representation as adjacency list
        std::vector<std::vector<Image *> *> *graph;
    public:
        GNNS(int, int, int, int);
        ~GNNS();
        std::vector<Image*> search(Image* query);
        void constructGraph(std::vector<Image *> *,int);
};

#endif