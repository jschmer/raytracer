#pragma once
#include <string>
#include <memory>

#include <FreeImage/FreeImage.h>

#include <RayTracer/RenderTarget/IRenderTarget.h>


class RayTraceImage : public IRenderTarget
{
public:
    RayTraceImage(const std::string filename, const int width = 200, const int height = 200);
    ~RayTraceImage();

    // IRenderTarget interface
    void init(const unsigned int width, const unsigned int height, const unsigned int bytes_per_pixel = 3) override;
    bool getSample(Sample &s) override;
    void commit(const Sample& s, const vec3 color) override;
    void done() override;

    // additional public methods
    void save() const;

private:
    // helper
    void clampToUpper(vec3 &v, const float upper) const;
    BYTE* getByteBuffer() const;

private:
    static bool freeimage_initialised;

    std::unique_ptr<Pixel[]> pImage;

    unsigned int numPixels;
    unsigned int currentSampleWidth;
    unsigned int currentSampleHeight;
    unsigned int pixelCounter;
    unsigned int saveAfterNumPixel;

    std::string filename;
};