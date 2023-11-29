#include "../include/mrng.hpp"

using namespace std;

MRNG::MRNG(int N, int l, vector<Image *> *data, const string& outputFile) {
    this->N = N;
    this->l = l;

    this->totalApproximate = 0;
    this->totalTrue = 0;

    this->startingNode = nullptr;

    this->data = data;

    this->graph = new vector<vector<Image *> *>;

    for (int i = 0; i < (int)data->size(); i++) {
        auto neighbors = new vector<Image *>;
        graph->push_back(neighbors);
    }

    this->MAF = 1;

    output.open(outputFile, ios::trunc);
    if (!output.is_open()) {
        cerr << "Can't open output file!" << endl;
        abort();
    }
    string contents;
    contents.append("MRNG Results\n");
    output << contents;
}

// Construction of graph using LSH instead of brute force
// to find the nearest neighbor. Sacrificing accuracy for speed
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
    Image *currImage = this->data->at(0);
    double minDist = dist(centroid, currImage->getCoords());

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

// Search-on-graph function
void MRNG::searchOnGraph(Image *query) {
    vector<pair<Image *, double>> candidates;
    vector<Image *> *neighbors;
    set<uint> neighborsSet;
    Image *currImage = nullptr;
    double distance;

    chrono::duration<double> tApproximate{}, tTrue{};

    //True search
    auto startTrue = chrono::high_resolution_clock::now();
    vector<double> neighborsTrue = getTrueNeighbors(query);
    auto endTrue = chrono::high_resolution_clock::now();

    tTrue = endTrue - startTrue;
    this->totalTrue += tTrue.count();

    // Approximate search
    auto startApproximate = chrono::high_resolution_clock::now();
    distance = dist(this->startingNode->getCoords(), query->getCoords());
    candidates.emplace_back(this->startingNode, distance);
    neighborsSet.insert(this->startingNode->getId());

    int i = 1;
    while (i < this->l) {
        // Get first unchecked candidate in set
        for (auto pair : candidates) {
            if (!pair.first->getChecked()) {
                currImage = pair.first;
                currImage->setChecked(true);
                break;
            }
        }

        neighbors = this->graph->at(currImage->getId() - 1);

        for (auto neighbor : *neighbors) {
            // If neighbor not already in candidates
            if (neighborsSet.find(neighbor->getId()) == neighborsSet.end()) {
                distance = dist(neighbor->getCoords(), query->getCoords());
                neighborsSet.insert(neighbor->getId());
                candidates.emplace_back(neighbor, distance);
                i++;
            }
        }

        sort(candidates.begin(), candidates.end(), sortNeighbors);
    }
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

// Function to get the true neighbors
vector<double> MRNG::getTrueNeighbors(Image *image) {
    vector<double> neighborsTrue;

    for (auto it : *this->data) {
        neighborsTrue.push_back(dist(it->getCoords(), image->getCoords()));
    }

    sort(neighborsTrue.begin(), neighborsTrue.end());

    return neighborsTrue;
}

// Function to reset the checked flag of all images
void MRNG::setAllUnchecked() {
    for (auto image : *data) {
        image->setChecked(false);
    }
}

// Function to output query results in txt file
void MRNG::outputResults(std::vector<std::pair<Image *, double>> candidates,
                         std::vector<double> neighborsTrue,
                         Image *query
                         ) {
    string contents;

    if (output.is_open()) {
        contents.append("Query: " + to_string(query->getId()) + "\n");

        for (int i = 0; i < this->N; i++) {
            contents.append("Nearest neighbor-" + to_string(i+1) + ": " + to_string(candidates.at(i).first->getId()) + "\n");
            contents.append("distanceApproximate: " + to_string(candidates.at(i).second) + "\n");
            contents.append("distanceTrue: " + to_string(neighborsTrue.at(i)) + "\n");
        }

        contents.append("\n");

        output << contents;
    }
}

void MRNG::outputTimeMAF(int querySize) {
    string contents;

    if (output.is_open()) {
        contents.append("tAverageApproximate: " + to_string(this->totalApproximate / querySize) + "\n");
        contents.append("tAverageTrue: " + to_string(this->totalTrue / querySize) + "\n");
        contents.append("MAF: " + to_string(this->MAF) + "\n");

        output << contents;
    }
}

MRNG::~MRNG() {
    for (int i = 0; i < (int)this->data->size(); i++) {
        delete graph->at(i);
    }

    delete graph;

    if (output.is_open()) {
        output.close();
    }
}