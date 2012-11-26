#include <FreeImage\FreeImage.h>
#include <string>
#include <iostream>

int main(int argc, char* argv[]) {
    FreeImage_Initialise();

    // save a image
    {
        std::string imageName = "testShot.png";
        int width = 600, height = 600;
        int pix = width * height;
        BYTE *pixels = new BYTE[3*pix];	

        DWORD colors[3];
        colors[0] = 0x00FF0000; // Red
        colors[1] = 0x0000FF00; // Green
        colors[2] = 0x000000FF; // Blue

        for (int i=0, c=0, idx=0; i<3*pix; i+=3, ++c) {
            if (c%10 == 0) idx = ++idx%3;
            memcpy(&pixels[i], &colors[idx], 3);
        }

        FIBITMAP *img = FreeImage_ConvertFromRawBits(pixels, width, height, width * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);

        std::cout << "Saving image: " << imageName << "\n";

        FreeImage_Save(FIF_PNG, img, imageName.c_str(), 0);
        delete pixels;
    }

    FreeImage_DeInitialise();
    return 0;
}