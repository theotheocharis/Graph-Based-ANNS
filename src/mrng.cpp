#include "../include/mrng.hpp"

using namespace std;

MRNG::MRNG(int N, int l, int imageNumber) {
    this->N = N;
    this->l = l;

    this->graph = new vector<vector<Image *> *>;

    for (int i = 0; i < imageNumber; i++) {
        auto neighbors = new vector<Image *>;
        graph->push_back(neighbors);
    }

    this->imageNumber = imageNumber;
}

// Construction of graph using LSH instead of brute force
// to find k nearest neighbors. Sacrificing accuracy for speed
void MRNG::constructGraph(vector<Image *> *inputImages) {
    auto lsh = new LSH(4, 5, inputImages);
    bool condition;

    for (int i = 0; i < (int)inputImages->size(); i++) {
        auto image = inputImages->at(i);
        auto candidates = new set<Image *>;
        candidates->insert(lsh->getNeighbor(image));

        for (auto r : *inputImages) {
            condition = true;
            if (candidates->find(r) == candidates->end()) {
                for (auto t : *candidates) {
                    double distPR = dist(image->getCoords(), r->getCoords());
                    double distRT = dist(r->getCoords(), t->getCoords());
                    double distPT = dist(image->getCoords(), t->getCoords());

                    if (distPR > distPT && distPR > distRT) {
                        condition = false;
                        break;
                    }
                }

                if (condition) {
                    candidates->insert(r);
                }
            }
        }

        for (auto neighbor : *candidates) {
            this->graph->at(i)->push_back(neighbor);
        }

        delete candidates;
    }

    delete lsh;
}

vector<vector<Image *>*>* MRNG::getGraph() {
    return this->graph;
}

MRNG::~MRNG() {
    for (int i = 0; i < imageNumber; i++) {
        delete graph->at(i);
    }

    delete graph;
}