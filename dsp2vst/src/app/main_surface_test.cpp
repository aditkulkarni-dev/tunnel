#include "../RTClasses/Surfaces.h"
#include <iostream>

int main(){
    PlaneWall wall(
        {0, 0, 0},
        {0, 0, 1},
        0.5f,
        10.0f,
        10.0f
    );

    Ray ray(
        {{2, 2, -5},
        {0, 0, 1}}
    );

    HitRecord record = wall.calculateIntersection(ray);
    std::cout << "Did it hit? : " << record.didHit << "\n";
    std::cout << "time of intersection : " << record.t << "\n";
    std::cout << "point of intersection : " << record.hitPoint << "\n";
    return 0;
}