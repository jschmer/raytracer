#pragma once
#include <Windows.h>
#include <FreeImage\FreeImage.h>
#include <string>
#include <glm\glm.hpp>

using namespace glm;

class Sample {
public:
    Sample() {}
    Sample(int x, int y)
        : x(static_cast<float>(x)),
          y(static_cast<float>(y))
    {}

    float x, y;
};

struct Pixel {
    BYTE Blue;
    BYTE Green;
    BYTE Red;

    Pixel() {
        Blue  = 0U;
        Green = 0U;
        Red   = 0U;
    }
};

class RayTraceImage
{
public:
    RayTraceImage(int width = 200, int height = 200, std::string filename = "SceneRender.png")
        : width(width),
          height(height),
          filename(filename),
          pixelCounter(0)
    {
        pImage = new Pixel[width*height];
        memset(pImage, 0, sizeof(Pixel));
        numPixels = width*height;
        currentSampleWidth = 0;
        currentSampleHeight = 0;

        saveAfterNumPixel = numPixels/10;

        FreeImage_Initialise();
    }

    ~RayTraceImage() {
        delete[] pImage;

        FreeImage_DeInitialise();
    }

    bool getSample(Sample &s) {
        /*
        if ((pixelCounter % saveAfterNumPixel) == 0)
            this->save();
        */
        if (currentSampleHeight < height) {
            int w = width;
            currentSampleHeight += currentSampleWidth/w;
            currentSampleWidth %= w;

            s.x = currentSampleWidth + 0.5f;
            s.y = currentSampleHeight + 0.5f;
            ++currentSampleWidth;
            ++pixelCounter;

            return true;
        } else {
            return false;
        }
    }

    void clampToUpper(vec3 &v, float const up) const {
        for (int i=0; i<3; ++i)
            v[i] = v[i] > up ? up : v[i]; 
    }

    void commit(Sample const &s, vec3 color) const {
        clampToUpper(color, 1.0f);

        Pixel PixelColor;
        PixelColor.Red   = (unsigned int) (color[0]*255);
        PixelColor.Green = (unsigned int) (color[1]*255);
        PixelColor.Blue  = (unsigned int) (color[2]*255);

        pImage[(int)s.y*width+(int)s.x] = PixelColor;
    }

    void save() const {
        // save the rendered image
        FIBITMAP *img = FreeImage_ConvertFromRawBits(this->getByteBuffer(), width, height, width * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, true);
        FreeImage_Save(FIF_PNG, img, filename.c_str(), 0);
    }

    BYTE* getByteBuffer() const {
        return (BYTE*)pImage;
    }

private:
    unsigned int width;
    unsigned int height;
    unsigned int numPixels;
    Pixel *pImage;

    unsigned int currentSampleWidth;
    unsigned int currentSampleHeight;
    unsigned int pixelCounter;
    unsigned int saveAfterNumPixel;

    std::string filename;
};

