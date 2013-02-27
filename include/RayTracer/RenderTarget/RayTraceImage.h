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
    void commit(const Sample& s, const color3 color) override;
    void OnDone() override;

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

    std::string filename;
};