#pragma once

#include <glm/glm.hpp>

using glm::vec3;
typedef unsigned char BYTE;

struct Sample {
    float x, y;
};

struct Pixel {
    BYTE Blue;
    BYTE Green;
    BYTE Red;

    // default - white!
    Pixel() {
        Blue  = 255U;
        Green = 255U;
        Red   = 255U;
    }
};

class IRenderTarget {
public:
    virtual void init(unsigned int const width, unsigned int const height, unsigned int const bytes_per_pixel) = 0;
    virtual bool getSample(Sample& s) = 0;
    virtual void commit(Sample const &s, vec3 color) const = 0;
};