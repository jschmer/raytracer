#pragma once

#include <RayTracer/glm_includes.h>

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
    virtual void init(const unsigned int width, const unsigned int height, const unsigned int bytes_per_pixel = 3) = 0;
    virtual bool getSample(Sample& s) = 0;
    virtual void commit(const Sample& s, const color3 color) = 0;
    virtual void done() = 0;

    unsigned int width() const {
        return _width;
    }

    unsigned int height() const {
        return _height;
    }

    void clampToUpper(color3 &v, const float upper) const {
        for (int i=0; i<3; ++i)
            v[i] = v[i] > upper ? upper : v[i]; 
    }

protected:
    unsigned int _width, _height;
};