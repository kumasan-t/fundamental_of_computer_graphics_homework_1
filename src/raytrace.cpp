#include "scene.h"
#include "intersect.h"

// compute the color corresponing to a ray by raytracing
vec3f raytrace_ray(Scene* scene, ray3f ray) {
    // get scene intersection
	intersection3f intersection = intersect_surfaces(scene, ray);
    // if not hit, return background
	if (!intersection.hit) {
		return scene->background;
	}
    // accumulate color starting with ambient
	vec3f accumulated_color = scene->ambient * intersection.mat->kd;
	// foreach light
	for (auto light : scene->lights) {
		
		// compute light direction
		vec3f light_direction = normalize(light->frame.o - intersection.pos);
        
		// compute light response
		vec3f light_color = light->intensity / lengthSqr(light->frame.o - intersection.pos);

		vec3f viewer_direction = normalize(scene->camera->frame.o - intersection.pos);

		vec3f bisector_h = normalize(light_direction + viewer_direction);
		vec3f diffuse = intersection.mat->kd;
        // compute the material response (brdf*cos)
		vec3f specular =
			intersection.mat->ks *
			pow(max(0.0f, dot(intersection.norm, bisector_h)), intersection.mat->n);
        
		// check for shadows and accumulate if needed
		ray3f shadow_ray = ray3f(intersection.pos, light_direction);
		auto shadow_intersect = intersect_surfaces(scene, shadow_ray);
		if (!shadow_intersect.hit) {
			accumulated_color += light_color *
				(diffuse + specular) *
				abs(dot(intersection.norm, light_direction));
		}
	    // if the material has reflections
	}
	if (!(intersection.mat->kr == zero3f)) {
	    // create the reflection ray
		ray3f reflected_ray = ray3f(intersection.pos, reflect(ray.d, intersection.norm));
		// accumulate the reflected light (recursive call) scaled by the material reflection
		accumulated_color += intersection.mat->kr * raytrace_ray(scene, reflected_ray);
	}
    // return the accumulated color
    return accumulated_color;
}

// raytrace an image
image3f raytrace(Scene* scene) {
    // allocate an image of the proper size
    auto image = image3f(scene->image_width, scene->image_height);
	int width = image.width();		// 512px
	int height = image.height();	// 512px
    // if no anti-aliasing
	if (scene->image_samples == 1) {
		// foreach pixel
		for (int pixel_i = 0; pixel_i < height; pixel_i++) {
			for (int pixel_j = 0; pixel_j < width; pixel_j++) {
				
				// compute ray-camera parameters (u,v) for the pixel
				auto u = (pixel_i + 0.5f) / width;
				auto v = (pixel_j + 0.5f) / height;
				auto camera_frame = scene->camera->frame;
				
				// computer the ray direction				
				vec3f ray_direction =
					(u - 0.5f) * scene->camera->width * camera_frame.x +
					(v - 0.5f) * scene->camera->height * camera_frame.y +
					-1 * scene->camera->dist * camera_frame.z;

				// compute camera ray
				vec3f camera_ray = normalize(ray_direction);
				ray3f ray = ray3f(camera_frame.o, camera_ray);
				
				// set pixel to the color raytraced with the ray
				image.at(pixel_i, pixel_j) = raytrace_ray(scene, ray);
			}
		}
	// else
	} else {
		// foreach pixel
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				// init accumulated color
				vec3f color = zero3f;
				// foreach sample
				for (int ii = 0; ii < scene->image_samples; ii++) {
					for (int jj = 0; jj < scene->image_samples; jj++) {
						float u = (float)(i + (ii + 0.5) / scene->image_samples) / width;
						float v = (float)(j + (jj + 0.5) / scene->image_samples) / height;
						// computer the ray direction
						auto camera_frame = scene->camera->frame;
						// compute ray-camera parameters (u,v) for the pixel and the sample
						vec3f ray_direction =
							(u - 0.5f) * scene->camera->width * camera_frame.x +
							(v - 0.5f) * scene->camera->height * camera_frame.y +
							-1 * scene->camera->dist * camera_frame.z;
						// compute camera ray
						vec3f camera_ray = normalize(ray_direction);
						ray3f ray = ray3f(camera_frame.o, camera_ray);
						color += raytrace_ray(scene, ray);
					}
				}
				// scale by the number of samples
				image.at(i, j) =  color / (float)pow(scene->image_samples,2);
			}
		} 
	}
    // done
    return image;
}

// runs the raytrace over all tests and saves the corresponding images
int main(int argc, char** argv) {
    auto args = parse_cmdline(argc, argv,
        { "01_raytrace", "raytrace a scene",
            {  {"resolution", "r", "image resolution", "int", true, jsonvalue() }  },
            {  {"scene_filename", "", "scene filename", "string", false, jsonvalue("scene.json")},
               {"image_filename", "", "image filename", "string", true, jsonvalue("")}  }
        });
    auto scene_filename = args.object_element("scene_filename").as_string();
    auto image_filename = (args.object_element("image_filename").as_string() != "") ?
        args.object_element("image_filename").as_string() :
        scene_filename.substr(0,scene_filename.size()-5)+".png";
    auto scene = load_json_scene(scene_filename);
    if(not args.object_element("resolution").is_null()) {
        scene->image_height = args.object_element("resolution").as_int();
        scene->image_width = scene->camera->width * scene->image_height / scene->camera->height;
    }
    message("rendering %s ... ", scene_filename.c_str());
    auto image = raytrace(scene);
    write_png(image_filename, image, true);
    delete scene;
    message("done\n");
}
