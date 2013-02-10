#pragma once
#include <string>
#include <memory>

#include <FreeImage\FreeImage.h>

struct Sample {
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
    RayTraceImage(const int width = 200, const int height = 200, const std::string filename = "SceneRender.png")
        : width(width),
          height(height),
          filename(filename),
          pixelCounter(0)
    {
        pImage.reset(new Pixel[width*height]);

        numPixels = width*height;
        currentSampleWidth  = 0;
        currentSampleHeight = 0;

        // save the image all 10%
        saveAfterNumPixel = numPixels/10;

        FreeImage_Initialise();
    }

    ~RayTraceImage() {
        FreeImage_DeInitialise();
    }

    bool getSample(Sample &s) {
        if ((pixelCounter % saveAfterNumPixel) == 0)
            this->save();

        if (currentSampleHeight < height) {
            currentSampleHeight += currentSampleWidth/width;
            currentSampleWidth %= width;

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

        pImage[(int)s.y*width + (int)s.x] = PixelColor;
    }

    void save() const {
        // save the rendered image
        FIBITMAP *img = FreeImage_ConvertFromRawBits(this->getByteBuffer(), width, height, width * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, true);
        FreeImage_Save(FIF_PNG, img, filename.c_str(), 0);
    }

    BYTE* getByteBuffer() const {
        return (BYTE*) (pImage.get());
    }

private:
    unsigned int width;
    unsigned int height;
    unsigned int numPixels;

    std::unique_ptr<Pixel[]> pImage;

    unsigned int currentSampleWidth;
    unsigned int currentSampleHeight;
    unsigned int pixelCounter;
    unsigned int saveAfterNumPixel;

    std::string filename;
};

