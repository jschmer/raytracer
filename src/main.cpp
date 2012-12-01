#define _USE_MATH_DEFINES
#include <string>
#include <iostream>

#include <RayTracer\Scene.h>

int main(int argc, char* argv[]) {

    for (int i=1; i<5; ++i) {
        stringstream s;
        s << i;

        Scene scene;
        scene.loadScene("testscenes/scene1-" + s.str() + ".test");

        std::cout << "output name: " << scene._outputFilename << "\n";

        scene.render();
    }

    std::cout << "Press a key...\n";
    return 0;
}