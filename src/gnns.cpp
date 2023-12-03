#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

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

// Construction of graph using LSH to find k nearest neighbors
void GNNS::constructGraph(vector<Image *> *inputImages, int k) {
    LSH lsh(5, 3, inputImages); // Initialize LSH

    for (int i = 0; i < (int)inputImages->size(); ++i) {
        Image* image = (*inputImages)[i];
        auto neighbors = lsh.getNeighborsGNNS(image, k); // Get k-nearest neighbors

        for (auto neighbor : neighbors)
        {
            graph->at(i)->push_back(neighbor);
        }
        
    }
}

// GNNS search function
void GNNS::search(Image* query) {
    std::vector<std::pair<Image*, double>> candidates;
    set<uint> neighborsSet;
    int maxGreedySteps = 50;
    chrono::duration<double> tApproximate{}, tTrue{};

    // Find true neighbors
    auto startTrue = chrono::high_resolution_clock::now();
    vector<double> neighborsTrue = getTrueNeighbors(query); // Calculate true distances
    auto endTrue = chrono::high_resolution_clock::now();

    tTrue = endTrue - startTrue;
    this->totalTrue += tTrue.count();

    // Approximate search
    auto startApproximate = chrono::high_resolution_clock::now();
    // Perform R random restarts
    for (int r = 0; r < R; ++r) {
        // Start with a random image from the dataset
        int randomIndex = std::rand() % this->data->size();
        Image* currentImage = (*graph)[randomIndex]->at(0);
        // If starting image has already been considered in previous iteration
        // Find a new starting image
        while (neighborsSet.find(currentImage->getId()) != neighborsSet.end())
        {
            randomIndex = std::rand() % this->data->size();
            currentImage = (*graph)[randomIndex]->at(0);
        }

        double currentDistance = dist(query->getCoords(), currentImage->getCoords());
        neighborsSet.insert(currentImage->getId());
        // Perform greedy steps
        for (int t = 0; t < maxGreedySteps; ++t) {
            double distanceNext = std::numeric_limits<double>::max();
            Image* nextImage = nullptr;
            // For E expansions
            for (int i = 0; i < E ; ++i){
                // If image has less than E neighbors break
                if (i == (int)graph->at(currentImage->getId() - 1)->size()){
                    break;
                }
                auto neighbor = graph->at(currentImage->getId() - 1)->at(i);
                // If neighbor hasn't been already considered
                if (neighborsSet.find(neighbor->getId()) == neighborsSet.end()){
                    double neighborDistance = dist(query->getCoords(),neighbor->getCoords());
                    // Add to candidates
                    candidates.emplace_back(neighbor, neighborDistance);
                    neighborsSet.insert(neighbor->getId());
                    // Find best neighbor for next iteration
                    if(neighborDistance < distanceNext){
                        distanceNext = neighborDistance;
                        nextImage = neighbor;
                    }
                }
            }
            // If no new neighbor has been found OR if they are all worse
            // than current image, break
            if (nextImage == nullptr || distanceNext >= currentDistance) {
                break;
            }

            currentImage = nextImage;
        }
    }

    // Sort candidates based on distance
    sort(candidates.begin(), candidates.end(), sortNeighbors);

    auto endApproximate = chrono::high_resolution_clock::now();

    tApproximate = endApproximate - startApproximate;
    this->totalApproximate += tApproximate.count();
    
    // Set MAF
    double af = candidates.at(0).second / neighborsTrue.at(0);
    if (af > this->MAF) {
        this->MAF = af;
    }

    // Output results
    outputResults(candidates, neighborsTrue, query);
}

// Function to output maximum approximation factor and average time
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