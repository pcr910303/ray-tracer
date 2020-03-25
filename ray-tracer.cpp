#include "camera.hpp"
#include "hitable-list.hpp"
#include "hitable.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "sphere.hpp"
#include "vec3.hpp"

#include <cfloat>
#include <cmath>
#include <iostream>
#include <random>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);

vec3 color(const ray &r, hitable *world, int depth) {
    hit_record rec;
    if(world->hit(r, 0.001, FLT_MAX, rec)) {
        ray scattered;
        vec3 attcnuation;
        if(depth < 50 && rec.mat_ptr->scatter(r, rec, attcnuation, scattered)) {
            return attcnuation * color(scattered, world, depth + 1);
        }
        return vec3(0, 0, 0);
    }
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

hitable *random_scene() {
    int n = 500;
    hitable **list = new hitable *[n + 1];
    list[0] = new sphere(vec3(0, -1000, 0), 1000,
                         new lambertian(vec3(0.5, 0.5, 0.5)));
    int i = 1;
    for(int a = -11; a < 11; a++) {
        for(int b = -11; b < 11; b++) {
            float choose_mat = dis(gen);
            vec3 center(a + 0.9 * dis(gen), 0.2, b + 0.9 * dis(gen));
            if((center - vec3(4, 0.2, 0)).length() > 0.9) {
                if(choose_mat < 0.8) {
                    list[i++] =
                        new sphere(center, 0.2,
                                   new lambertian(vec3(dis(gen) * dis(gen),
                                                       dis(gen) * dis(gen),
                                                       dis(gen) * dis(gen))));
                } else if(choose_mat < 0.95) {
                    list[i++] = new sphere(
                        center, 0.2,
                        new metal(vec3(0.5 * (1 + dis(gen)),
                                       0.5 * (1 + dis(gen)), 0.5 * dis(gen))));
                } else {
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }
    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] =
        new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] =
        new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

    return new hitable_list(list, i);
}

int main() {
    int nx = 2560;
    int ny = 1600;
    int ns = 100;

    std::cout << "P3\n" << nx << " " << ny << "\n255\n";

    hitable *world = random_scene();

    vec3 lookfrom(13, 2, 3);
    vec3 lookat(0, 0, 0);
    float dist_to_focus = 10.0;
    float aperture = 0.1;
    camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny),
               aperture, dist_to_focus);

    for(int j = ny - 1; j >= 0; j--) {
        if((ny - j) % 10 == 0) {
            std::cerr << "Drawing " << ny - j << "th line." << std::endl;
        }
        for(int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            for(int s = 0; s < ns; s++) {
                float u = float(i + dis(gen)) / float(nx);
                float v = float(j + dis(gen)) / float(ny);
                ray r = cam.get_ray(u, v);
                col += color(r, world, 0);
            }
            col /= float(ns);
            col = vec3(std::sqrt(col[0]), std::sqrt(col[1]), std::sqrt(col[2]));
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
        if((ny - j) % 10 == 0) {
            std::cerr << double(ny - j) / double(ny) * 100. << "% finished."
                      << std::endl;
        }
    }

    return 0;
}
