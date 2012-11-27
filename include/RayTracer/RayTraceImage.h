#pragma once
#include <Windows.h>
#include <FreeImage\FreeImage.h>
#include <string>

class Sample {
public:
    Sample() {}
    Sample(int x, int y)
        : x(x),
          y(y)
    {}

    int x;
    int y;
};

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
        currentSample = 0;
    }

    ~RayTraceImage() {
        delete[] pImage;
    }

    bool getSample(Sample &s) {
        if (currentSample <= numPixels) {
            s.x = currentSample % width;
            s.y = currentSample / width;
            ++currentSample;

            return true;
        } else {
            return false;
        }
    }

    void save(std::string filename) {
        FreeImage_Initialise();

        // save the rendered image
        {
            FIBITMAP *img = FreeImage_ConvertFromRawBits(this->getByteBuffer(), width, height, width * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);
            FreeImage_Save(FIF_PNG, img, filename.c_str(), 0);
        }

        FreeImage_DeInitialise();
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

    unsigned int currentSample;
};

