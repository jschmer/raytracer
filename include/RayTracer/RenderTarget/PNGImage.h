#pragma once
#include <string>
#include <memory>

#include <FreeImage/FreeImage.h>

#include <RayTracer/RenderTarget/IRenderTarget.h>


class PNGImage : public IRenderTarget
{
public:
    PNGImage(const std::string filename, const int width = 200, const int height = 200);
    ~PNGImage();

    // IRenderTarget interface
    void init(const unsigned int width, const unsigned int height, const unsigned int bytes_per_pixel = 3) override;
    void commit(const Sample& s, const color3 color) override;
    void OnDone() override;
    void setOutputName(std::string name);

    // additional public methods
    void save() const;

private:
    // helper
    BYTE* getByteBuffer() const;

private:
    static bool freeimage_initialised;

    std::unique_ptr<Pixel[]> pImage;

    unsigned int _num_committed_samples;
    unsigned int _save_after_num_samples;
    unsigned int _numPixels;

    std::string filename;
};