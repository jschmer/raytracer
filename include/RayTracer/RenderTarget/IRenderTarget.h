/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
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
    IRenderTarget()
        : _currentSampleWidth(0),
        _currentSampleHeight(0),
        _pixelCounter(0),
        _width(0),
        _height(0)
    {}

    // public virtual interface
    virtual void init(const unsigned int width, const unsigned int height, const unsigned int bytes_per_pixel = 3) = 0;
    virtual void commit(const Sample& s, const color3 color) = 0;
    virtual void OnDone() = 0;

    // public interface
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

    bool getSample(Sample& s) {
        if (_currentSampleHeight < _height) {
            _currentSampleHeight += _currentSampleWidth/_width;
            _currentSampleWidth %= _width;

            s.x = _currentSampleWidth + 0.5f;
            s.y = _currentSampleHeight + 0.5f;
            ++_currentSampleWidth;

            return true;
        } else {
            _pixelCounter = 0;
            _currentSampleWidth  = 0;
            _currentSampleHeight = 0;
            return false;
        }
    }

protected:
    unsigned int _width, _height;

private:
    unsigned int _currentSampleWidth;
    unsigned int _currentSampleHeight;
    unsigned int _pixelCounter;
};