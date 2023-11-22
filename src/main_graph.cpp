#include <iostream>
#include <unistd.h>

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

    if (m == 1) {
        // Call GNNS
    } else {
        // Call MRNG
    }

    return 0;
}