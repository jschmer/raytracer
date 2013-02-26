raytracer
=========

Simple raytracer project for educational purposes written in C++

TODO
====

Features:
  * Falls keine Kamera im Scenefile definiert ist:
    Eine Default-Kamera anhand der Scene-AABB berechnen/erstellen

  * Unterstützung von
    - Bump Map
    - Specular Map
    - Normal Map
    
  * Anti-Aliasing:
    - Mehrere Rays durch EINEN Pixel erzeugen und die Farben dieser Rays mitteln
      (Supersampling)
    - Diese Rays nur erzeugen wenn eine Kante im Spiel ist! (Sobel)
      (Edge detection)

Speedups:    
  * Uniform Grid Traversal:
    Das Bounding Box Gitter nur dem Ray folgend durchgehen

Debug
=====
  * tricell.test -> AABB der Scene ist nicht richtig
  - Sphere Bounding Box nicht ganz ok!
    ...kommt wohl mit scale/rotate nicht zurecht