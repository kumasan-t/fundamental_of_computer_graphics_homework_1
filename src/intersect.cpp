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
	float min_distance = INFINITY;
	// grab the surfaces presents in the scene
	auto surfaces = scene->surfaces;
	// foreach surface
	for (Surface* surface : surfaces) {
		// if it is a quad
		if (surface->isquad) {
			
			// compute ray intersection (and ray parameter), continue if not hit
			float t;
			float dn = dot(ray.d, surface->frame.z);
			if ( dn != 0) {
				t = dot((surface->frame.o - ray.e), surface->frame.z) / dn;
			}

			if (t < ray.tmin or t > ray.tmax) {
				continue;
			}

			auto ray_at_t = ray.eval(t);									// world coordinate
			auto plane_point = transform_point(surface->frame, ray_at_t);   // surface local coordinate
			// check if this is the closest intersection, continue if not
			if ( abs(plane_point.x) > surface->radius or
				 abs(plane_point.y) > surface->radius) {
				continue;
			} else {
				// if hit, set intersection record value
				if (t < min_distance) {
					//intersection.pos = transform_point_inverse(scene->camera->frame,ray_at_t);
					intersection.pos = ray_at_t;
					intersection.ray_t = t;
					intersection.hit = true;
					intersection.norm = surface->frame.z;
					intersection.mat = surface->mat;
					min_distance = t;
				}
			}
		// if it is a sphere
		} else {
			// compute ray intersection (and ray parameter), continue if not hit
			auto a = lengthSqr(ray.d);
			auto b = 2 * dot(ray.d, (ray.e - surface->frame.o));
			auto c = lengthSqr(ray.e - surface->frame.o) - pow(surface->radius,2);
			auto d = pow(b,2) - 4 * a * c;
			
			float t_plus;
			float t_minus;
			
			if (d >= 0) {
				t_plus = (-1 * b + sqrt(d)) / (2 * a);
				t_minus = (-1 * b - sqrt(d)) / (2 * a);
			} else {
				continue;
			}

			// check if computed param is within ray.tmin and ray.tmax
			float min_t;
			if (t_plus < t_minus and t_plus > ray.tmin and t_plus < ray.tmax) {
				min_t = t_plus;
			} else if (t_minus > ray.tmin and t_minus < ray.tmax) {
				min_t = t_minus;
			} else {
				continue;
			}

			// check if this is the closest intersection, continue if not
			auto point_on_sphere = ray.eval(min_t);
			if (min_t < min_distance) {
				// if hit, set intersection record values
				min_distance = min_t;
				intersection.hit = true;
				intersection.mat = surface->mat;
				//intersection.pos = transform_point_inverse(scene->camera->frame,point_on_sphere);
				intersection.pos = point_on_sphere;
				intersection.ray_t = min_t;
				intersection.norm = normalize(point_on_sphere - surface->frame.o);
			}
		}
		// record closest intersection
	}
	return intersection;
}

