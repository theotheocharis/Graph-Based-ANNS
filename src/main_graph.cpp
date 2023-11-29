#include <iostream>
#include <unistd.h>

#include "../include/parser.hpp"
#include "../include/mrng.hpp"

using namespace std;

int main(int argc, char **argv) {
    int opt;
    string inputFile, queryFile, outputFile;
    int k = 50, E = 30, R = 1, N = 1, l = 20, m;

    while((opt = getopt(argc, argv, "d:q:k:E:R:N:l:m:o:")) != -1) {
        switch (opt) {
            case 'd':
                inputFile = optarg;
                break;
            case 'q':
                queryFile = optarg;
                break;
            case 'k':
                k = stoi(optarg);
                break;
            case 'E':
                E = stoi(optarg);
                break;
            case 'R':
                R = stoi(optarg);
                break;
            case 'N':
                N = stoi(optarg);
                break;
            case 'l':
                l = stoi(optarg);
                break;
            case 'm':
                m = stoi(optarg);
                break;
            case 'o':
                outputFile = optarg;
                break;
            default:
                abort();
        }
    }

    auto parser = new Parser();
    vector<Image *> *inputImages = parser->readInputFile(inputFile);
    vector<Image *> *queryImages = parser->readQueryFile(queryFile);

    if (m == 1) {
        // Call GNNS
    } else {
        // Initialize mrng object
        auto mrng = new MRNG(N, l, inputImages, outputFile);

        // Graph construction
        mrng->constructGraph();

        // Find image closest to centroid to use as starting node
        mrng->findStartingNode();

        for (auto queryImage : *queryImages) {
            mrng->searchOnGraph(queryImage);
            mrng->setAllUnchecked();
        }

        mrng->outputTimeMAF((int)queryImages->size());

        delete mrng;
    }

    delete parser;
    for (auto image : *inputImages) {
        delete image;
    }
    delete inputImages;

    for (auto image : *queryImages) {
        delete image;
    }
    delete queryImages;

    return 0;
}