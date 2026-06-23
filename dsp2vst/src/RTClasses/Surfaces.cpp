#include "Surfaces.h"

HitRecord PlaneWall::calculateIntersection(const Ray &ray) const
{   
    /*
    r(t) = O + tD   r : R -> R3
    n * <x,y,z> + d = 0
    n * <rx(t), ry(t), rz(t)> + d = 0
    n * < Ox + tDx, Oy + tDy, Oz + tDz> + d = 0
    n_x (O_x + tD_x) + n_y (O_y + tD_y) + n_z (O_z + tD_z) + d = 0
    t(n_x * D_x + n_y * D_y + n_z * D_z) + (n_x * O_x + n_y * O_y + n_z * O_z + d) = 0
    t = - (n_x * O_x + n_y * O_y + n_z * O_z + d) / (n_x * D_x + n_y * D_y + n_z * D_z)
    t = - (n * O + d) / (n * D) 

    r(t_intersection) = O + (- (n * O + d) / (n * D) )D
    This is where we check the constraints, 
    if r_x, r_y and r_z are under the constraints,
    the intersection of valid.

    */
   float numerator = - (normal.dot(ray.origin) - planeConstant);
   float deno = (normal.dot(ray.direction));

   if (std::abs(deno) < EPSILON){
    return HitRecord();
   }

   float t = numerator / deno;
   if (t < 0.0f){
    return HitRecord();
   }

   Vector3D P_intersection = ray.origin + ray.direction*t;
   
   Vector3D local = P_intersection - pointOnSurface;
   float uCoord = local.dot(u);
   float vCoord = local.dot(v);

   if (std::abs(uCoord) <= width / 2 &&
    std::abs(vCoord) <= height / 2)
    {
        return HitRecord({true, t, P_intersection, normal});
    }

    return HitRecord();
   

    
}