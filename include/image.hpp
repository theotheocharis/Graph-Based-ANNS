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

    bool checked;

public:

    Image(uint, std::vector<double> *);
    ~Image();

    uint getId();
    void setId(uint);

    std::vector<double> *getCoords();
    void setCoords(std::vector<double> *);

    bool getChecked();
    void setChecked(bool);

};

#endif
