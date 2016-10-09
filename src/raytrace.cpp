#include "scene.h"
#include "intersect.h"

// compute the color corresponing to a ray by raytracing
vec3f raytrace_ray(Scene* scene, ray3f ray) {
    // get scene intersection
    // if not hit, return background
    
    // accumulate color starting with ambient
    
    // foreach light
        // compute light response
        // compute light direction
        // compute the material response (brdf*cos)
        // check for shadows and accumulate if needed
    
    // if the material has reflections
        // create the reflection ray
        // accumulate the reflected light (recursive call) scaled by the material reflection
    
    // return the accumulated color∫
    return zero3f;
}

// raytrace an image
image3f raytrace(Scene* scene) {
    // allocate an image of the proper size
    auto image = image3f(scene->image_width, scene->image_height);
    
    // if no anti-aliasing
	if (true) {
		// foreach pixel
		for (int i = 0; i < image.width; i++) {
			for (int j = 0; j < image.height; j++) {
				// compute ray-camera parameters (u,v) for the pixel
				auto camera_frame = scene->camera->frame;
				auto point_on_image_x = camera_frame.o.x + (i - 0.5) * image.width * camera_frame.x;
				auto point_on_image_y = camera_frame.o.y + (j - 0.5) * image.width * camera_frame.y;
				auto point_on_image_z = camera_frame.o.z + scene->camera->dist;
				vec3f point_on_image3f(point_on_image_x, point_on_image_y, point_on_image_z);
				// compute camera ray
				normalize(point_on_image3f);
				ray3f camera_ray = ray3f(camera_frame.o, point_on_image3f);
				// set pixel to the color raytraced with the ray
				vec3f accumulated_color = raytrace_ray(scene, camera_ray);
				image.at(i, j).x = accumulated_color.x;
				image.at(i, j).x = accumulated_color.y;
				image.at(i, j).x = accumulated_color.z;
			}
		}
		// else
	} else {
        // foreach pixel
                // init accumulated color
                // foreach sample
                        // compute ray-camera parameters (u,v) for the pixel and the sample
                        // compute camera ray
                        // set pixel to the color raytraced with the ray
                // scale by the number of samples
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
