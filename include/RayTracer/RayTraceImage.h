#pragma once
#include <Windows.h>

struct Pixel {
    BYTE Blue;
    BYTE Green;
    BYTE Red;
};

class RayTraceImage
{
public:
    RayTraceImage(int width = 200, int height = 200)
        : width(width),
          height(height)
    {
        pImage = new Pixel[width*height];
        numPixels = width*height;
    }

    ~RayTraceImage() {
        delete[] pImage;
    }

    void fill() {
        Pixel RedPixel, GreenPixel, BluePixel;
        RedPixel.Blue    = 0x00;
        RedPixel.Green   = 0x00;
        RedPixel.Red     = 0xFF;

        GreenPixel.Blue  = 0x00;
        GreenPixel.Green = 0xFF;
        GreenPixel.Red   = 0x00;

        BluePixel.Blue   = 0xFF;
        BluePixel.Green  = 0x00;
        BluePixel.Red    = 0x00;

        for (unsigned int i=0; i<numPixels; ++i)
            pImage[i] = RedPixel;
    }

    BYTE* getByteBuffer() {
        return (BYTE*)pImage;
    }

private:
    unsigned int width;
    unsigned int height;
    unsigned int numPixels;
    Pixel *pImage;
};

