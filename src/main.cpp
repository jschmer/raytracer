#include <FreeImage\FreeImage.h>
#include <string>
#include <iostream>

#include <RayTracer\RayTraceImage.h>

int main(int argc, char* argv[]) {
    FreeImage_Initialise();

    // save a image
    {
        std::string imageName = "testShot.png";
        int width = 600, height = 600;

        RayTraceImage rayimg(width, height);
        rayimg.fill();

        FIBITMAP *img = FreeImage_ConvertFromRawBits(rayimg.getByteBuffer(), width, height, width * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);
        FreeImage_Save(FIF_PNG, img, imageName.c_str(), 0);
    }

    FreeImage_DeInitialise();
    getchar();
    return 0;
}