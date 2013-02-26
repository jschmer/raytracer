raytracer
=========

Simple raytracer project for educational purposes written in C++

This software uses the FreeImage open source image library. See http://freeimage.sourceforge.net for details.
FreeImage is used under the GNU GPL, version 3. 

TODO
====

Features:
-  Falls keine Kamera im Scenefile definiert ist:
 - Eine Default-Kamera anhand der Scene-AABB berechnen/erstellen

- Unterstützung von
 - Bump Map
 - Specular Map
 - Normal Map
    
- Anti-Aliasing:
 - Mehrere Rays durch EINEN Pixel erzeugen und die Farben dieser Rays mitteln (Supersampling)
 - Diese Rays nur erzeugen wenn eine Kante im Spiel ist! (Sobel) (Edge detection)

Speedups:    
- Uniform Grid Traversal:
  - Das Bounding Box Gitter nur dem Ray folgend durchgehen

Debug
=====
- tricell.test -> AABB der Scene ist nicht richtig
  - Sphere Bounding Box nicht ganz ok! (...bezieht scale/rotate nicht mit ein)