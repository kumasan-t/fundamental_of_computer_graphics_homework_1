#include "scene.h"
#include "intersect.h"

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
			else {
				continue;
			}

			if (t < ray.tmin or t > ray.tmax) {
				continue;
			}

			auto plane_point = ray.eval(t);
			// check if this is the closest intersection, continue if not
			if ( abs(plane_point.x) > surface->frame.o.x + surface->radius ||
				 abs(plane_point.y) > surface->frame.o.y + surface->radius ||
				 abs(plane_point.z) > surface->frame.o.z + surface->radius) {
				continue;
			} else {
				// if hit, set intersection record value
				if (t < min_distance) {
					intersection.pos = plane_point;
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
			auto a = 1.0f;
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

			float min_t;
			if (t_minus >= ray.tmin && t_minus <= ray.tmax) min_t = t_minus;
			else if (t_plus >= ray.tmin && t_plus <= ray.tmax) min_t = t_plus;
			else continue;

			// check if this is the closest intersection, continue if not
			auto point_on_sphere = ray.eval(min_t);
			if (min_t < min_distance) {
				// if hit, set intersection record values
				min_distance = min_t;
				intersection.hit = true;
				intersection.mat = surface->mat;
				intersection.pos = point_on_sphere;
				intersection.ray_t = min_t;
				intersection.norm = normalize(point_on_sphere - surface->frame.o);
			}
			else {
				continue;
			}
		}
		// record closest intersection
	}
	return intersection;
}

