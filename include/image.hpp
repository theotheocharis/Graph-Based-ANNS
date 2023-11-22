#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <iostream>
#include <vector>
#include <string>

#include "utils.hpp"

class Image {

private:

    uint id;
    std::vector<double> *coords;
    uint cluster;

    bool assigned;

public:

    Image(uint, std::vector<double> *, uint);
    ~Image();

    uint getId();
    void setId(uint);

    std::vector<double> *getCoords();
    void setCoords(std::vector<double> *);

    uint getCluster();
    void setCluster(uint);

    bool getAssigned();
    void setAssigned(bool);

};

#endif
