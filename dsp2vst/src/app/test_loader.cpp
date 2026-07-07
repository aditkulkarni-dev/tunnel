#include "../RTClasses/ObjLoader.h"

int main(){
    std::string path = "sponza_triangulated.obj";
    ObjLoader loader(path);
    std::vector<Triangle> triangles = loader.parseObjToTriangles();

    return 0;

}