#include <string>
#include <iostream>

#include <RayTracer\Scene.h>

int main(int argc, char* argv[]) {
    Scene scene;
    scene.loadScene("testscenes/scene1.test");

    std::cout << "output name: " << scene._outputFilename << "\n";

    scene.render();

    std::cout << "Press a key...\n";
    //getchar();
    return 0;
}