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

    this->MAF = 1;

    this->totalApproximate = 0;
    this->totalTrue = 0;

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
    LSH lsh(5, 3, inputImages); // Initialize LSH

    for (int i = 0; i < inputImages->size(); ++i) {
        Image* image = (*inputImages)[i];
        auto neighbors = lsh.getNeighborsGNNS(image, k); // Get k-nearest neighbors
        //if ((*graph)[i]) { // Check if the pointer is valid
        //    (*graph)[i]->insert((*graph)[i]->end(), neighbors.begin(), neighbors.end());
        //}

        for (auto neighbor : neighbors)
        {
            graph->at(i)->push_back(neighbor);
        }
        
    }
}


void GNNS::search(Image* query) {
    std::vector<std::pair<Image*, double>> candidates;
    set<uint> neighborsSet;
    int maxGreedySteps = 50;
    chrono::duration<double> tApproximate{}, tTrue{};
    auto startTrue = chrono::high_resolution_clock::now();
    vector<double> neighborsTrue = getTrueNeighbors(query); // Calculate true distances
    auto endTrue = chrono::high_resolution_clock::now();

    tTrue = endTrue - startTrue;
    this->totalTrue += tTrue.count();
    
    auto startApproximate = chrono::high_resolution_clock::now();
    for (int r = 0; r < R; ++r) { // Perform R random restarts
        int randomIndex = std::rand() % this->data->size();
        Image* currentImage = (*graph)[randomIndex]->at(0);
        while (neighborsSet.find(currentImage->getId()) != neighborsSet.end())
        {
            randomIndex = std::rand() % this->data->size();
            currentImage = (*graph)[randomIndex]->at(0);
        }

        double currentDistance = dist(query->getCoords(), currentImage->getCoords());
        neighborsSet.insert(currentImage->getId());

        for (int t = 0; t < maxGreedySteps; ++t) {
            double distanceNext = std::numeric_limits<double>::max();
            Image* nextImage = nullptr;
            for (int i = 0; i < E ; ++i){

                if (i == graph->at(currentImage->getId() - 1)->size()){
                    break;
                }
                auto neighbor = graph->at(currentImage->getId() - 1)->at(i);
                if (neighborsSet.find(neighbor->getId()) == neighborsSet.end()){
                    double neighborDistance = dist(query->getCoords(),neighbor->getCoords());
                    candidates.emplace_back(neighbor, neighborDistance);
                    neighborsSet.insert(neighbor->getId());
                    if(neighborDistance < distanceNext){
                        distanceNext = neighborDistance;
                        nextImage = neighbor;
                    }
                }
            }

            if (nextImage == nullptr || distanceNext >= currentDistance) {
                break;
            }

            currentImage = nextImage;
        }
    }

    sort(candidates.begin(), candidates.end(), sortNeighbors);

    auto endApproximate = chrono::high_resolution_clock::now();

    tApproximate = endApproximate - startApproximate;
    this->totalApproximate += tApproximate.count();
    
    // Set MAF
    double af = candidates.at(0).second / neighborsTrue.at(0);
    if (af > this->MAF) {
        this->MAF = af;
    }

    outputResults(candidates, neighborsTrue, query);
}

void GNNS::outputTimeMAF(int querySize) {
    string contents;

    if (output.is_open()) {
        contents.append("tAverageApproximate: " + to_string(this->totalApproximate / querySize) + "\n");
        contents.append("tAverageTrue: " + to_string(this->totalTrue / querySize) + "\n");
        contents.append("MAF: " + to_string(this->MAF) + "\n");

        output << contents;
    }
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