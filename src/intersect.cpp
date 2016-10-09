#include "scene.h"
#include "intersect.h"
/*
Ray definition
vec3f e;        // origin
vec3f d;        // direction
float tmin;     // min t value
float tmax;     // max t value

Intersection definition
bool        hit;        // whether it hits something
float       ray_t;      // ray parameter for the hit
vec3f       pos;        // hit position
vec3f       norm;       // hit normal
Material*   mat;        // hit material
*/
// intersects the scene's surfaces and return the first intrerseciton (used for raytracing homework)
intersection3f intersect_surfaces(Scene* scene, ray3f ray) {
    // create a default intersection record to be returned
    auto intersection = intersection3f();
	intersection.pos = vec3f(INFINITY, INFINITY, INFINITY);

	// grab the surfaces presents in the scene
	auto surfaces = scene->surfaces;
	
	// foreach surface
	for (auto surface : surfaces) {
		// if it is a quad
		if (surface->isquad) {
			// compute ray intersection (and ray parameter), continue if not hit
			float t = dot((surface->frame.o - ray.e),surface->frame.z)/dot(ray.d,surface->frame.z);
			// check if computed param is within ray.tmin and ray.tmax
			if (t > ray.tmin && t < ray.tmax) {
				// check if this is the closest intersection, continue if not
				vec3f temp_intersect_pos = ray.e + t * ray.d;
				if (intersection.pos.z > temp_intersect_pos.z) {
					intersection.pos = temp_intersect_pos;
					// if hit, set intersection record values
					intersection.ray_t = t;
					intersection.hit = true;
					intersection.norm = surface->frame.z;
					intersection.mat = surface->mat;
				}
			}
		// if it is a sphere
		} else {
			// compute ray intersection (and ray parameter), continue if not hit

			// check if computed param is within ray.tmin and ray.tmax

			// check if this is the closest intersection, continue if not

			// if hit, set intersection record values
		}
		// record closest intersection
	
	}
	return intersection;
}

