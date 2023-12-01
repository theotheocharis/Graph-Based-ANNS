#include <iostream>
#include <unistd.h>
#include <vector>

#include "../include/parser.hpp"
#include "../include/mrng.hpp"
#include "../include/gnns.hpp"

using namespace std;

int main(int argc, char **argv) {
    int opt;
    string inputFile, queryFile, outputFile;
    int k = 5, E = 3, R = 5, N = 5, l = 20, m;

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
        auto gnns = new GNNS(E, R, N, inputImages, outputFile);

        //cout << "Size of query images" << queryImages->size() << endl;

        gnns->constructGraph(inputImages, k); // Construct the graph

        for (const auto& query : *queryImages) { 

            gnns->search(query);

        }

        delete gnns;

    } else {
        // Initialize mrng object
        auto mrng = new MRNG(N, l, inputImages, outputFile);

        // Graph construction
        mrng->constructGraph();

        // Find image closest to centroid to use as starting node
        mrng->findStartingNode();

        for (auto queryImage : *queryImages) {
            // Run query
            mrng->searchOnGraph(queryImage);

            // Reset image checked flags
            mrng->setAllUnchecked();
        }

        // Output average time and MAF
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