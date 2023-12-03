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
    int k = 50, E = 30, R = 1, N = 1, l = 20, m = 1;

    string s;

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

    if (inputFile.empty()) {
        cout << "Please provide input file!" << endl;
        cin >> inputFile;
    }
    if (queryFile.empty()) {
        cout << "Please provide query file!" << endl;
        cin >> queryFile;
    }
    if (outputFile.empty()) {
        cout << "Please provide output file!" << endl;
        cin >> outputFile;
    }

    do {
        auto parser = new Parser();

        // Read input and query images from files
        vector<Image *> *inputImages = parser->readInputFile(inputFile);
        vector<Image *> *queryImages = parser->readQueryFile(queryFile);

        if (m == 1) {
            // Initialize GNNS object
            auto gnns = new GNNS(E, R, N, inputImages, outputFile);

            // Graph construction
            gnns->constructGraph(inputImages, k);

            for (auto query : *queryImages) {
                // Run query
                gnns->search(query);
            }

            gnns->outputTimeMAF((int)queryImages->size());

            delete gnns;

        } else {
            // Initialize MRNG object
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

        cout << "Query finished! Would you like to start a new query? (y/n)" << endl;
        cin >> s;
        if (s == "y") {
            if (inputFile.empty()) {
                cout << "Please provide input file!" << endl;
                cin >> inputFile;
            }
            if (queryFile.empty()) {
                cout << "Please provide query file!" << endl;
                cin >> queryFile;
            }
            if (outputFile.empty()) {
                cout << "Please provide output file!" << endl;
                cin >> outputFile;
            }
        }
    } while (s != "n");

    return 0;
}