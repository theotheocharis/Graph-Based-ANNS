#include <iostream>
#include <unistd.h>
#include <vector>

#include "../include/parser.hpp"
#include "../include/lsh.hpp"

using namespace std;

int main(int argc, char **argv) {
    int opt;
    string inputFile, queryFile, outputFile;
    int k = 4, L = 5, N = 1, R = 10000;

    string s;

    while((opt = getopt(argc, argv, "d:q:k:L:o:N:R:")) != -1) {
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
            case 'L':
                L = stoi(optarg);
                break;
            case 'o':
                outputFile = optarg;
                break;
            case 'N':
                N = stoi(optarg);
                break;
            case 'R':
                R = stoi(optarg);
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

        // Read images and queries from files
        vector<Image *> *inputImages = parser->readInputFile(inputFile);
        vector<Image *> *queryImages = parser->readQueryFile(queryFile);

        // Initialize LSH with the parsed parameters and input images
        LSH lsh(k, L, N, R, inputImages, outputFile);

        // Process each query image
        for(auto queryImage : *queryImages){
            lsh.query(queryImage);
        }

        lsh.outputTimeMAF((int)queryImages->size());

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