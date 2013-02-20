#pragma once

#include <RayTracer/glm_includes.h>
#include <RayTracer/AssimpToRaytracerTypes.h>

// DevIL stuff
#include <il/il.h>

static bool devil_initialized = false;

aiTexel* loadTexture(std::string texture_path, unsigned int &width, unsigned int &height) {
    if (!devil_initialized)
        ilInit();
    
    // load binary texture data! with DevIL
    ILuint texid;           /* ILuint is a 32bit unsigned integer.´Variable texid will be used to store image name. */
    ilGenImages(1, &texid); /* Generation of one image name */
    ilBindImage(texid);     /* Binding of image name */

    auto success = ilLoadImage((const ILstring) texture_path.c_str()); /* Loading of image "image.jpg" */
    if (success) {
        // Convert every colour component into unsigned byte
        // If your image contains alpha channel you can replace IL_RGB with IL_RGBA
        success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE); 
        if (!success)
            return nullptr;

        width  = ilGetInteger(IL_IMAGE_WIDTH);
        height = ilGetInteger(IL_IMAGE_HEIGHT);
        auto bpp    = ilGetInteger(IL_IMAGE_BPP);
        auto pdata  = ilGetData();

        // converting image data to aiTexels
        auto aiTexelData = new aiTexel[width*height];
        unsigned int dim = bpp * width * height;
        for (auto image_idx = 0u, texel_idx = 0u; image_idx < dim; ++image_idx, ++texel_idx) {
            auto r = pdata[image_idx];
            auto g = pdata[++image_idx];
            auto b = pdata[++image_idx];
            auto a = pdata[++image_idx];

            aiTexel texel;
            texel.r = r;
            texel.g = g;
            texel.b = b;
            texel.a = a;

            aiTexelData[texel_idx] = texel;
        }

        ilDeleteImages(1, &texid); /* Because we have already copied image data into texture data
                                   we can release memory used by image. */

        return aiTexelData;
    }

    return nullptr;
}