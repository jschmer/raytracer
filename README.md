# raytracer

Simple raytracer project for educational purposes written in C++

This software uses the FreeImage open source image library. See http://freeimage.sourceforge.net for details.  
FreeImage is used under the GNU GPL, version 3. 

## Features
* Supports only Triangles (except .test scene files: these support Spheres)
* Illumination Model with
  * Reflection
  * Shadows
* Lights:
  * Point
  * Directional
  * Area (for soft shadows)
* Texture/Material support:
  * Ambient map
  * Diffuse map
  * Specular color map
 
* Supported Scene file types:
  * Everything [Assimp](http://assimp.sourceforge.net/) can load
  * custom .test file type (see ./scenes/ for examples)
  * custom .raytrace file type
    * can embed other scene files
    * used for defining additional scene objects (camera, lights, output size, ...)
      for scene file types that can't specify these (e.g. .obj)
* Automatically defines a camera and Light if there isn't one present (looking/shining down Z-Axis)
  * Required for plain model files (.obj)
* Very simple spatial division speedup with regular grid inside the scenes AABB
* easy derivable IRenderTarget interface to support your own render target
  * default shipped implementation: PNGImage (with [FreeImage](http://freeimage.sourceforge.net) library)

## TODO
### Features
* Unterstützung von
  - Specular Level Map (shininess)
  - Bump Map
  - Normal Map

* Anti-Aliasing
  - Mehrere Rays durch EINEN Pixel erzeugen und die Farben dieser Rays mitteln (Supersampling)
  - Diese Rays nur erzeugen wenn eine Kante im Spiel ist! (Sobel) (Edge detection)

### Speedups
* Uniform Grid Traversal
  - Das Bounding Box Gitter nur dem Ray folgend durchgehen

## Issues
- tricell.test -> AABB der Scene ist nicht richtig
  - Sphere Bounding Box nicht ganz ok! (...bezieht scale/rotate nicht mit ein)