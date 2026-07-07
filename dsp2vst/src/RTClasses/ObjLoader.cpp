#include "ObjLoader.h"

int ObjLoader::getVertexIdx(const std::string &token)
{
    size_t slash = token.find('/');
    if (slash == std::string::npos){
        return std::stoi(token);
    }
    return std::stoi(token.substr(0, slash));
}

std::vector<Triangle> ObjLoader::parseObjToTriangles(float absorption){
    // TODO: insert return statement here
    file.clear();
    file.seekg(0);
    std::string line;
    std::vector<Vector3D> vertices;
    std::vector<Vector3D> normals;
    std::vector<Triangle> triangles;


    while (std::getline(file, line)) // get a line from file to string line
    {
        if (line.empty() || line[0] == '#'){continue;}
            
        /* code */
        std::stringstream ss(line); // create a string stream of line
        std::string type; 
        ss >> type; // get the first character of the stream. in this case, its something like v

        if (type == "v"){
            float x, y, z;
            ss >> x >> y >> z;
            vertices.emplace_back(x,y,z);
        }

        else if(type == "vn"){
            float x, y, z;
            ss >> x >> y >> z;
            normals.emplace_back(x,y,z);
        }

        else if (type == "vt"){
            continue;
        }
        else if (type == "f"){
            std::string a, b, c;
            ss >> a >> b >> c;
            int i1 = getVertexIdx(a)-1;
            int i2 = getVertexIdx(b)-1;
            int i3 = getVertexIdx(c)-1;
            triangles.emplace_back(vertices[i1], vertices[i2], vertices[i3], absorption);
        }

    }
    std::cout << "number of vertices : " << vertices.size() << "\n";
    std::cout << "number of faces : " << triangles.size() << "\n";

    return triangles;

}
