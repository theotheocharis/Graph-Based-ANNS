#include <iostream>
#include <vector>
#include <unistd.h>
#include <cstring>

#include "../include/parser.hpp"
#include "../include/cube.hpp"

using namespace std;

int main(int argc, char **argv) {
    int k = 14;
    int M = 10;
    int probes = 2;
    int N = 1;
    int R = 10000;

    string s;

    string inputFile, queryFile, outputFile;

    for (int i = 0; i < argc; i++) {
        if (!strcmp("-d", argv[i])) {
            inputFile = argv[i+1];
        }
        if (!strcmp("-q", argv[i])) {
            queryFile = argv[i+1];
        }
        if (!strcmp("-o", argv[i])) {
            outputFile = argv[i+1];
        }
        if (!strcmp("-k", argv[i])) {
            k = stoi(argv[i+1]);
        }
        if (!strcmp("-M", argv[i])) {
            M = stoi(argv[i+1]);
        }
        if (!strcmp("-N", argv[i])) {
            N = stoi(argv[i+1]);
        }
        if (!strcmp("-R", argv[i])) {
            R = stoi(argv[i+1]);
        }
        if (!strcmp("-probes", argv[i])) {
            probes = stoi(argv[i+1]);
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

        vector<Image *> *inputImages = parser->readInputFile(inputFile);
        vector<Image *> *queryImages = parser->readQueryFile(queryFile);

        HyperCube hyperCube(k, M, probes, N, R, inputImages, outputFile);

        for (auto queryImage : *queryImages) {
            hyperCube.query(queryImage);
        }

        hyperCube.outputMAF();

        delete parser;

        for (auto image : *inputImages) {
            delete image;
        }
        delete inputImages;

        for (auto image : *queryImages) {
            delete image;
        }
        delete queryImages;

        cout << "Query finished! Would you like to query again? (y/n)" << endl;
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