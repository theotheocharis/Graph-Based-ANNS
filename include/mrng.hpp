#ifndef MRNG_HPP
#define MRNG_HPP

#include <iostream>
#include <vector>
#include <string>

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

    double totalApproximate;
    double totalTrue;

    double MAF;

    std::ofstream output;

public:

    MRNG(int, int, std::vector<Image *> *, const std::string&);

    void constructGraph();

    std::vector<double> *findCentroid();
    void findStartingNode();

    void searchOnGraph(Image *);
    std::vector<double> getTrueNeighbors(Image *);

    void setAllUnchecked();

    void outputResults(std::vector<std::pair<Image *, double>>, std::vector<double>, Image *);
    void outputTimeMAF(int);

    ~MRNG();

};

#endif