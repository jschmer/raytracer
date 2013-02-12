#pragma once
#include <string>
#include <memory>

#include <FreeImage/FreeImage.h>

#include <RayTracer/RenderTarget/IRenderTarget.h>


class RayTraceImage : public IRenderTarget
{
public:
    RayTraceImage(const std::string filename = "SceneRender.png", const int width = 200, const int height = 200);
    ~RayTraceImage();

    // IRenderTarget interface
    void init(const unsigned int width, const unsigned int height, const unsigned int bytes_per_pixel = 3) override;
    bool getSample(Sample &s) override;
    void commit(const Sample &s, vec3 color) const override;

    void save() const;

private:
    void clampToUpper(vec3 &v, const float upper) const;
    BYTE* getByteBuffer() const;

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

    static bool freeimage_initialised;
};