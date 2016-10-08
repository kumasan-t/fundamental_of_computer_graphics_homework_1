#include "scene.h"
#include "intersect.h"

// intersects the scene's surfaces and return the first intrerseciton (used for raytracing homework)
intersection3f intersect_surfaces(Scene* scene, ray3f ray) {
    // create a default intersection record to be returned
    auto intersection = intersection3f();

	// grab the surfaces presents in the scene
	auto surfaces = scene->surfaces;
	
	// foreach surface
	for (auto surface : surfaces) {
		// if it is a quad
		if (surface->isquad) {
			// compute ray intersection (and ray parameter), continue if not hit
		}
	}


            
    
            // check if computed param is within ray.tmin and ray.tmax
    
            // check if this is the closest intersection, continue if not
    
            // if hit, set intersection record values
    
      // if it is a sphere
            // compute ray intersection (and ray parameter), continue if not hit
    
            // check if computed param is within ray.tmin and ray.tmax
            
            // check if this is the closest intersection, continue if not
            
            // if hit, set intersection record values
    
    // record closest intersection
    return intersection;
}

