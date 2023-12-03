#include "../include/lsh.hpp"

using namespace std;

const int M = (int)pow(2, 32) - 5;

// Constructor for the LSH (Locality Sensitive Hashing) class
LSH::LSH(int k, int L,int N, int R, vector<Image *> *data, const string& outputFile) {
    
    // Initialize the given parameters
    this->k = k;
    this->L = L;
    this->N = N;
    this->R = R;
    this->data = data;
    this->w = 10;

    this->MAF = 1;

    this->totalApproximate = 0;
    this->totalTrue = 0;

    r.resize(L, vector<int>(0));

    for (int i = 0; i < L; i++) {
        auto table = new HashTable(data->size() / 8);
        auto hashFuncs = new vector<HashFunction *>;
        for (int j = 0; j < k; j++) {
            hashFuncs->push_back(new HashFunction(w));
            r[i].push_back(rand());
        }
        hashTables.emplace_back(table, hashFuncs);
    }

    // Populate hash tables with images
    for (auto image : *data) {
        insert(image);
    }

    output.open(outputFile, ios::trunc);
    if (!output.is_open()) {
        cerr << "Can't open output file!" << endl;
        abort();
    }
}

LSH::LSH(int k, int L, std::vector<Image *> *data) {
    this->k = k;
    this->L = L;
    this->data = data;
    this->w = 10;

    this->totalApproximate = 0;
    this->totalTrue = 0;

    r.resize(L, vector<int>(0));

    for (int i = 0; i < L; i++) {
        auto table = new HashTable(data->size() / 8);
        auto hashFuncs = new vector<HashFunction *>;
        for (int j = 0; j < k; j++) {
            hashFuncs->push_back(new HashFunction(w));
            r[i].push_back(rand());
        }
        hashTables.emplace_back(table, hashFuncs);
    }

    for (auto image : *data) {
        insert(image);
    }
}

void LSH::insert(Image *image) {

    for (int i = 0; i < L; i++) {
        int ID = 0;
        for (int j = 0; j < k; j++) {
            ID += r[i][j] * hashTables[i].second->at(j)->h(image->getCoords());
        }
        ID = euclideanModulo(ID, M);
        hashTables[i].first->insert((uint)ID, image);
    }

}

// Function to get approximately nearest neighbors of a given image 'q' using LSH
void LSH::query(Image* q) {
    list<pair<uint, void *>> neighborsID, neighborsBucket;
    vector<pair<uint, double>> neighborsLSH;
    list<uint> neighborsRNear;
    set<uint> neighborsSet;

    chrono::duration<double> tLSH{}, tTrue{};

    auto startTrue = chrono::high_resolution_clock::now();
    vector<double> neighborsTrue = getTrueNeighbors(q);
    auto endTrue = chrono::high_resolution_clock::now();

    tTrue = endTrue - startTrue;
    this->totalTrue += tTrue.count();

    auto startLSH = chrono::high_resolution_clock::now();
    for (int i = 0; i < L; i++) {
        int ID = 0;
        for (int j = 0; j < k; j++) {
            ID += r[i][j] * hashTables[i].second->at(j)->h(q->getCoords());
        }

        neighborsID.splice(neighborsID.end(), hashTables[i].first->findSameID(ID));
        neighborsBucket.splice(neighborsBucket.end(), hashTables[i].first->findBucket(ID));
    }

    if (neighborsID.size() >= (size_t) N) {
        for (auto nID : neighborsID) {
            auto neighbor = (Image *) nID.second;
            if (neighborsSet.find(neighbor->getId()) == neighborsSet.end()) {
                double distance = dist(q->getCoords(), neighbor->getCoords());
                if (distance < R) {
                    neighborsRNear.push_back(neighbor->getId());
                }
                neighborsLSH.emplace_back(neighbor->getId(), distance);
                neighborsSet.insert(neighbor->getId());
            }
        }
    } else {
        for (auto nBucket : neighborsBucket) {
            auto neighbor = (Image *) nBucket.second;
            if (neighborsSet.find(neighbor->getId()) == neighborsSet.end()) {
                double distance = dist(q->getCoords(), neighbor->getCoords());
                if (distance < R) {
                    neighborsRNear.push_back(neighbor->getId());
                }
                neighborsLSH.emplace_back(neighbor->getId(), distance);
                neighborsSet.insert(neighbor->getId());
            }
        }
    }

    // Sort the neighbors based on distance
    sort(neighborsLSH.begin(), neighborsLSH.end(), sortPairBySecond);
    set<uint> setRNear(neighborsRNear.begin(), neighborsRNear.end());

    auto endLSH = chrono::high_resolution_clock::now();

    tLSH = endLSH - startLSH;
    this->totalApproximate += tLSH.count();

    double af = neighborsLSH.at(0).second / neighborsTrue.at(0);
    if (af > this->MAF) {
        this->MAF = af;
    }

    outputResults(neighborsLSH, neighborsTrue, setRNear, q);
}

std::vector<double> LSH::getTrueNeighbors(Image *image) {
    vector<double> neighborsTrue;

    for (auto it : *this->data) {
        neighborsTrue.push_back(dist(it->getCoords(), image->getCoords()));
    }

    sort(neighborsTrue.begin(), neighborsTrue.end());

    return neighborsTrue;
}

// Function for graph construction. Given an image it returns
// its approximate neighbor.
Image *LSH::getNeighbor(Image *image) {
    list<pair<uint, void *>> neighborsID, neighborsBucket;
    auto neighbors = new vector<pair<Image *, double>>;
    set<uint> neighborsSet;

    for (int i = 0; i < L; i++) {
        int ID = 0;
        for (int j = 0; j < k; j++) {
            ID += r[i][j] * hashTables[i].second->at(j)->h(image->getCoords());
        }

        neighborsID.splice(neighborsID.end(), hashTables[i].first->findSameID(ID));
        neighborsBucket.splice(neighborsBucket.end(), hashTables[i].first->findBucket(ID));
    }

    for (auto nID : neighborsBucket) {
        auto neighbor = (Image *) nID.second;
        if (image->getId() != neighbor->getId()) {
            double distance = dist(image->getCoords(), neighbor->getCoords());
            neighbors->emplace_back(neighbor, distance);
        }
    }

    sort(neighbors->begin(), neighbors->end(), sortNeighbors);

    auto neighbor = neighbors->at(0).first;
    delete neighbors;

    return neighbor;
}

std::vector<Image*> LSH::getNeighborsGNNS(Image *queryImage, int k) {
    list<pair<uint, void *>> neighborsID, neighborsBucket;
    vector<pair<Image*, double>> neighborCandidates;

    // LSH processing to find potential neighbors
    for (int i = 0; i < L; ++i) {
        int ID = 0;
        for (int j = 0; j < this->k; ++j)    {
            ID += r[i][j] * hashTables[i].second->at(j)->h(queryImage->getCoords());
        }

        neighborsID.splice(neighborsID.end(), hashTables[i].first->findSameID(ID));
        neighborsBucket.splice(neighborsBucket.end(), hashTables[i].first->findBucket(ID));
    }

    // Calculating distances and storing candidates
    for (auto nID : neighborsBucket) {
        Image* neighbor = static_cast<Image*>(nID.second);
        if (neighbor != queryImage) {
            double distance = dist(queryImage->getCoords(), neighbor->getCoords());
            neighborCandidates.emplace_back(neighbor, distance);
        }
    }

    // Sort candidates by distance
    std::sort(neighborCandidates.begin(), neighborCandidates.end(),
              [](const std::pair<Image*, double>& a, const std::pair<Image*, double>& b) {
                  return a.second < b.second;
              });

    // Select the top k neighbors
    std::vector<Image*> kNearestNeighbors;
    for (int i = 0; i < k && i < (int)neighborCandidates.size(); ++i) {
        kNearestNeighbors.push_back(neighborCandidates[i].first);
    }

    return kNearestNeighbors;
}

void LSH::outputResults(vector<pair<uint, double>> neighborsLSH,
                        vector<double> neighborsTrue, const set<uint>& neighborsRNear,
                        Image *q) {
    string contents;

    if (output.is_open()) {
        contents.append("Query: " + to_string(q->getId()) + "\n");

        for (int i = 0; i < N; i++) {
            contents.append("Nearest neighbor-" + to_string(i+1) + ": " + to_string(neighborsLSH[i].first) + "\n");
            contents.append("distanceLSH: " + to_string(neighborsLSH[i].second) + "\n");
            contents.append("distanceTrue: " + to_string(neighborsTrue[i]) + "\n");
        }

        contents.append("R-near neighbors:\n");

        for (auto nRNear : neighborsRNear) {
            contents.append(to_string(nRNear) + "\n");
        }

        contents.append("\n");

        output << contents;
    }
}

void LSH::outputTimeMAF(int querySize) {
    string contents;

    if (output.is_open()) {
        contents.append("tAverageApproximate: " + to_string(this->totalApproximate / querySize) + "\n");
        contents.append("tAverageTrue: " + to_string(this->totalTrue / querySize) + "\n");
        contents.append("MAF: " + to_string(this->MAF) + "\n");

        output << contents;
    }
}

// Destructor for the LSH class
LSH::~LSH() {
    // Delete each hash table
    for (int i = 0; i < L; i++) {
        delete hashTables[i].first;
        for (auto hFunc : *hashTables[i].second) {
            delete hFunc;
        }
        delete hashTables[i].second;
    }

    if (output.is_open()) {
        output.close();
    }

}
