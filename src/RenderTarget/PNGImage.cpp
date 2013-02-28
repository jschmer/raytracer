#include <RayTracer/RenderTarget/PNGImage.h>

bool PNGImage::freeimage_initialised = false;

PNGImage::PNGImage(const std::string filename, const int width, const int height)
    : filename(filename)
{
    init(width, height);
}

PNGImage::~PNGImage() {
    if (freeimage_initialised) {
        FreeImage_DeInitialise();
        freeimage_initialised = false;
    }
}

void PNGImage::init(const unsigned int width, const unsigned int height, const unsigned int bytes_per_pixel) {
    _width  = width;
    _height = height;

    _numPixels = width*height;
    pImage.reset(new Pixel[_numPixels]);

    _num_committed_samples = 0;

    // save the image all 10%
    _save_after_num_samples = _numPixels/10;

    if (!freeimage_initialised) {
        FreeImage_Initialise();
        freeimage_initialised = true;
    }
}

void PNGImage::commit(const Sample &s, vec3 color) {
    clampToUpper(color, 1.0f);

    Pixel PixelColor;
    PixelColor.Red   = (unsigned int) (color[0]*255);
    PixelColor.Green = (unsigned int) (color[1]*255);
    PixelColor.Blue  = (unsigned int) (color[2]*255);

    auto idx = (int)s.y*_width + (int)s.x;
    if (idx < _numPixels)
        pImage[(int)s.y*_width + (int)s.x] = PixelColor;

    if ((++_num_committed_samples % _save_after_num_samples) == 0)
        this->save();
}

void PNGImage::save() const {
    // save the rendered image
    FIBITMAP *img = FreeImage_ConvertFromRawBits(this->getByteBuffer(), _width, _height, _width * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, true);
    FreeImage_Save(FIF_PNG, img, filename.c_str(), 0);
    FreeImage_Unload(img);
}

BYTE* PNGImage::getByteBuffer() const {
    return (BYTE*) (pImage.get());
}

void PNGImage::OnDone() {
    this->save();
}

void PNGImage::setOutputName(std::string name) {
    filename = name;
}