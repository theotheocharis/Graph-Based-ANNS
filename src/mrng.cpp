#include "../include/mrng.hpp"

using namespace std;

MRNG::MRNG(int N, int l, vector<Image *> *data) {
    this->N = N;
    this->l = l;

    this->data = data;

    this->graph = new vector<vector<Image *> *>;

    for (int i = 0; i < (int)data->size(); i++) {
        auto neighbors = new vector<Image *>;
        graph->push_back(neighbors);
    }
}

// Construction of graph using LSH instead of brute force
// to find k nearest neighbors. Sacrificing accuracy for speed
void MRNG::constructGraph() {
    auto lsh = new LSH(4, 5, this->data);
    bool condition;

    for (int i = 0; i < (int)this->data->size(); i++) {
        auto image = this->data->at(i);
        auto candidates = new set<Image *>;
        candidates->insert(lsh->getNeighbor(image));

        for (auto r : *this->data) {
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

// Find centroid of data set
vector<double> *MRNG::findCentroid() {
    auto centroid = new vector<double>;

    int imageNumber = (int) this->data->size();
    int sizeOfCoords = (int) this->data->at(0)->getCoords()->size();

    for (int i = 0; i < sizeOfCoords; i++) {
        double total = 0;
        for (int j = 0; j < imageNumber; j++) {
            total += this->data->at(j)->getCoords()->at(i);
        }
        centroid->push_back(total/imageNumber);
    }

    return centroid;
}

// Find the closest image to centroid to be used as starting node in search
void MRNG::findStartingNode() {
    vector<double> *centroid = findCentroid();
    Image *currImage;
    double minDist = 1000000;

    for (auto image : *this->data) {
        double distance = dist(centroid, image->getCoords());

        if (distance < minDist) {
            currImage = image;
            minDist = distance;
        }
    }

    this->startingNode = currImage;
    delete centroid;
}

vector<vector<Image *>*>* MRNG::getGraph() {
    return this->graph;
}

Image *MRNG::getStartingNode() {
    return this->startingNode;
}

MRNG::~MRNG() {
    for (int i = 0; i < (int)this->data->size(); i++) {
        delete graph->at(i);
    }

    delete graph;
}