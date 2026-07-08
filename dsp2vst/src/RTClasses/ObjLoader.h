#include <string>
#include "Surfaces.h"
#include "Scene.h"
#include <fstream>
#include <sstream>

class ObjLoader{
private:

     std::ifstream file;

     int getVertexIdx(const std::string& token);

public:
    ObjLoader(const std::string& path) : file(path){
        if (!file.is_open()){
        throw std::runtime_error("Couldn't open OBJ file");
        }
    }

    void loadObj(Scene &scene, float absorption=0.2f);


};