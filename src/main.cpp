#include <string>
#include <iostream>

#include <RayTracer\Scene.h>

int main(int argc, char* argv[]) {
    Scene scene;
    std::cout << "output name: " << scene._outputFilename << "\n";
    
    scene.loadScene("testscenes/scene1.test");
    std::cout << "output name: " << scene._outputFilename << "\n";

    scene.render();

    scene.saveImage();

    std::cout << "Press a key...\n";
    getchar();
    return 0;
}