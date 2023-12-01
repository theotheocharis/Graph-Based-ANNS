#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <unistd.h>

#include "../include/gnns.hpp"

using namespace std;

GNNS::GNNS(int E, int R, int N, vector<Image *> *data,const string& outputFile) {
    this->E = E;
    this->R = R;
    this->N = N;

    this->graph = new vector<vector<Image *> *>;

    this->data = data;

    for (int i = 0; i < (int)data->size(); i++) {
        auto neighbors = new vector<Image *>;
        graph->push_back(neighbors);
    }

    output.open(outputFile, ios::trunc);
    if (!output.is_open()) {
        cerr << "Can't open output file!" << endl;
        abort();
    }
    string contents;
    contents.append("GNNS Results\n");
    output << contents;
}

//Construction of graph using LSH to find k nearest neighbors
void GNNS::constructGraph(vector<Image *> *inputImages, int k) {
    LSH lsh(k, 3, inputImages); // Initialize LSH

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


void GNNS::search(Image* query) {
    std::vector<std::pair<Image*, double>> candidates;
    int maxGreedySteps = std::sqrt(this->data->size());
    vector<double> neighborsTrue = getTrueNeighbors(query); // Calculate true distances

    for (int r = 0; r < R; ++r) { // Perform R random restarts
        double distanceApproximate = std::numeric_limits<double>::max();
        int randomIndex = std::rand() % this->data->size();
        Image* currentImage = (*graph)[randomIndex]->at(0);
        int neighborsAdded = 0; // Initialize neighborsAdded for each random restart

        for (int t = 0; t < maxGreedySteps; ++t) {
            double currentDistance = dist(query->getCoords(), currentImage->getCoords());
            if (currentDistance < distanceApproximate && neighborsAdded < this->N) {
                double trueDistance = dist(currentImage->getCoords(), query->getCoords());
                candidates.emplace_back(currentImage, currentDistance);
                distanceApproximate = currentDistance;
                neighborsAdded++;
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

            if (closestNeighbor == nullptr || closestDistance >= currentDistance) {
                break;
            }

            currentImage = closestNeighbor;
        }
    }

    outputResults(candidates, neighborsTrue, query);
}



// Function to get the true neighbors
vector<double> GNNS::getTrueNeighbors(Image *image) {
    vector<double> neighborsTrue;

    for (auto it : *this->data) {
        neighborsTrue.push_back(dist(it->getCoords(), image->getCoords()));
    }

    sort(neighborsTrue.begin(), neighborsTrue.end());

    return neighborsTrue;
}

// Function to output query results in txt file
void GNNS::outputResults(std::vector<std::pair<Image *, double>> candidates,
                         std::vector<double> neighborsTrue,
                         Image *query
                         ) {
    string contents;
    
    if (output.is_open()) {
        contents.append("Query: " + to_string(query->getId()) + "\n");
        for (int i = 0; i < N; i++) {
            contents.append("Nearest neighbor-" + to_string(i+1) + ": " + to_string(candidates.at(i).first->getId()) + "\n");
            contents.append("distanceApproximate: " + to_string(candidates.at(i).second) + "\n");
            contents.append("distanceTrue: " + to_string(neighborsTrue.at(i)) + "\n");
        }

        contents.append("\n");

        output << contents;
    }
}

GNNS::~GNNS() {
    for (auto &neighbors : *graph) {
        delete neighbors;
    }
    delete graph;
}