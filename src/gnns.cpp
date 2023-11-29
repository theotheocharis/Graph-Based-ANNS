#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <unistd.h>

#include "../include/gnns.hpp"

using namespace std;

GNNS::GNNS(int E, int R, int N, int imageNumber) {
    this->E = E;
    this->R = R;
    this->N = N;

    this->graph = new vector<vector<Image *> *>;

    for (int i = 0; i < imageNumber; i++) {
        auto neighbors = new vector<Image *>;
        graph->push_back(neighbors);
    }

    this->imageNumber = imageNumber;
}

//Construction of graph using LSH to find k nearest neighbors
void GNNS::constructGraph(vector<Image *> *inputImages, int k) {
    LSH lsh(k, 5, inputImages); // Initialize LSH

    graph->clear();
    graph->resize(inputImages->size());
    for (int i = 0; i < inputImages->size(); ++i) {
        (*graph)[i] = new std::vector<Image*>(); // Allocate a new vector
    }

    for (int i = 0; i < inputImages->size(); ++i) {
        Image* image = (*inputImages)[i];
        auto neighbors = lsh.getNeighborsGNNS(image, k); // Get k-nearest neighbors

        if ((*graph)[i]) { // Check if the pointer is valid
            (*graph)[i]->insert((*graph)[i]->end(), neighbors.begin(), neighbors.end());
        }
    }
}


std::vector<Image*> GNNS::search(Image* query) {
    std::vector<Image*> bestResults;
    double distanceApproximate = std::numeric_limits<double>::max();
    double distanceTrue = std::numeric_limits<double>::max();
    for (int r = 0; r < R; ++r) { //Perform R random restarts
        int randomIndex = std::rand() % imageNumber; //Select a random start node
        Image* currentImage = (*graph)[randomIndex]->at(0);

        for (int t = 0; t < N; ++t) { // Perform T greedy steps
            double currentDistance = dist(query->getCoords(), currentImage->getCoords());
            if (currentDistance < distanceApproximate) {
                bestResults.clear();
                bestResults.push_back(currentImage);
                distanceApproximate = currentDistance;
                distanceTrue = 0;//need to implement it
            }

            Image* closestNeighbor = nullptr;
            double closestDistance = std::numeric_limits<double>::max();

            for (Image* neighbor : *(*graph)[currentImage->getId()]) {
                double distance = dist(query->getCoords(), neighbor->getCoords());
                if (distance < closestDistance) {
                    closestNeighbor = neighbor;
                    closestDistance = distance;
                }
            }

            if (closestNeighbor == nullptr || closestDistance >= currentDistance) { //Terminate if no closer neighbor is found
                break; 
            }

            currentImage = closestNeighbor; //Move to the closer neighbor
        }
    }

    return bestResults;
}


GNNS::~GNNS() {
    for (auto &neighbors : *graph) {
        delete neighbors;
    }
    delete graph;
}