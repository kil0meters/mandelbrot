#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <getopt.h>

#include "mandelbrot.h"

int main(int argc, char *argv[]) {
    double l1, l2, zoom;
    int iterations;

    sscanf(argv[1], "%lf", &l1);
    sscanf(argv[2], "%lf", &l2);
    sscanf(argv[3], "%lf", &zoom);
    sscanf(argv[4], "%d", &iterations);

    printf("%lf, %lf, %lf, %d\n", l1, l2, zoom, iterations);

    int resolution[2] = {1920, 1080};
    double location[2] = {l1, l2};

    Mandelbrot mandelbrot = mandelbrot_new(resolution);
    set_mandelbrot_location(&mandelbrot, location);
    set_mandelbrot_zoom(&mandelbrot, zoom);
    set_mandelbrot_iterations(&mandelbrot, iterations);
    set_mandelbrot_rendering_mode(&mandelbrot, MANDELBROT_CHECKERBOARD);

    char *output_file = "output.png";
    bool result = mandelbrot_render_to_file(mandelbrot, output_file);
    if (result) {
        printf("error: could not open file %s\n", output_file);
        return 1;
    }

    return 0;
}
