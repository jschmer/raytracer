#include <RayTracer/RenderTarget/RayTraceImage.h>

bool RayTraceImage::freeimage_initialised = false;

RayTraceImage::RayTraceImage(const std::string filename, const int width, const int height)
    : filename(filename)
{
    init(width, height);
}

RayTraceImage::~RayTraceImage() {
    if (freeimage_initialised)
        FreeImage_DeInitialise();
}

void RayTraceImage::init(const unsigned int width, const unsigned int height, const unsigned int bytes_per_pixel) {
    this->width  = width;
    this->height = height;

    numPixels = width*height;
    pImage.reset(new Pixel[numPixels]);

    pixelCounter = 0;
    currentSampleWidth  = 0;
    currentSampleHeight = 0;

    // save the image all 10%
    saveAfterNumPixel = numPixels/10;

    if (!freeimage_initialised) {
        FreeImage_Initialise();
        freeimage_initialised = true;
    }
}

bool RayTraceImage::getSample(Sample &s) {
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

void RayTraceImage::commit(const Sample &s, vec3 color) const {
    clampToUpper(color, 1.0f);

    Pixel PixelColor;
    PixelColor.Red   = (unsigned int) (color[0]*255);
    PixelColor.Green = (unsigned int) (color[1]*255);
    PixelColor.Blue  = (unsigned int) (color[2]*255);

    pImage[(int)s.y*width + (int)s.x] = PixelColor;

    if ((pixelCounter % saveAfterNumPixel) == 0)
        this->save();
}

void RayTraceImage::save() const {
    // save the rendered image
    FIBITMAP *img = FreeImage_ConvertFromRawBits(this->getByteBuffer(), width, height, width * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, true);
    FreeImage_Save(FIF_PNG, img, filename.c_str(), 0);
}


void RayTraceImage::clampToUpper(vec3 &v, const float upper) const {
    for (int i=0; i<3; ++i)
        v[i] = v[i] > upper ? upper : v[i]; 
}

BYTE* RayTraceImage::getByteBuffer() const {
    return (BYTE*) (pImage.get());
}