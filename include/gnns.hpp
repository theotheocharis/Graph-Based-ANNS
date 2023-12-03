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
        std::vector<Image *> *data;
        // Graph representation as adjacency list
        std::vector<std::vector<Image *> *> *graph;

        double MAF;

        double totalApproximate;
        double totalTrue;

        std::ofstream output;
    public:
        GNNS(int, int, int, std::vector<Image *> *,const std::string&);
        ~GNNS();
        //std::vector<Image*> search(Image* query);
        void search(Image* query);
        void constructGraph(std::vector<Image *> *,int);
        std::vector<double> getTrueNeighbors(Image *image);
        void outputResults(std::vector<std::pair<Image *, double>>, std::vector<double>, Image *query);
        void outputTimeMAF(int);
};

#endif