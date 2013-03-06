#include <RayTracer/SceneReader/SceneParserHelper.h>

#include <String/StringHelper.h>

bool readvals(std::stringstream &s, const int numvals, std::vector<float> &values) {
    for (int i = 0; i < numvals; i++) {
        float f;
        s >> f; 
        if (s.fail()) {
            //cout << "Failed reading value " << i << " will skip\n"; 
            return false;
        }
        values.push_back(f);
    }
    return true; 
}

std::string getDirectory(std::string path) {
    path = String::replace(path, "\\", "/");
    auto pos = path.rfind('/');
    if (std::string::npos != pos) {
        // leave trailing slash there!
        auto dir = path.substr(0, pos + 1);
        return dir;
    }
    else {
        return "./";
    }
}