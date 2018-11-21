#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "mandelbrot.h"

typedef struct MandelbrotOptions {
    int resolution[2];
    int iterations;
    double viewport_size;
    double location[2];
    char *output_file_location;
    RenderingMode rendering_mode;
} MandelbrotOptions;

static const struct option options[] = {
    { "size", required_argument, NULL, 's'},
    { "resolution", required_argument, NULL, 'r' },
    { "location", required_argument, NULL, 'l' },
    { "iterations", required_argument, NULL, 'i' },
    { "rendering-mode", required_argument, NULL, 'm' },
    { "output", required_argument, NULL, 'o' },
    { NULL, no_argument, NULL, 0 }
};

static void print_usage(const char *executable_name) {
    printf("mandelbrot v0.1\n"
           "kilometers <kilo@meters.sh>\n\n"
           "USAGE:\n%s [ -s FLOAT | -r INTxINT | -l FLOATxFLOAT | -i INT\n"
           "              -m RENDERING_MODE | -o STRING ]\n\n"
           "  -s, --size            Viewport size multiplier [DEFAULT: 1.0]\n"
           "  -r, --resolution      Render resolution [DEFAULT: 256x256]\n"
           "  -l, --location        Viewport location [DEFAULT: 1.0x1.0]\n"
           "  -i, --iterations      Number of iterations [DEFAULT: 1000]\n"
           "  -m, --rendering-mode  Rendering mode [DEFAULT: default]\n"
           "  -o, --output          Output file location [DEFAULT: output.png]\n\n"
           "RENDERING MODES\n\n"
           "  default\n"
           "  checkerboard\n",
           executable_name);
    exit(0);
}

static RenderingMode rendering_mode_from_string(char *string) {
    if (!strcmp(string, "default")) {
        return MANDELBROT_DEFAULT;
    }
    else if (!strcmp(string, "checkerboard")) {
        return MANDELBROT_CHECKERBOARD;
    }
    else {
        return 0;
    }
}

MandelbrotOptions parse_command_line_options(int argc, char *argv[]) {
    int resolution[2] = { 256, 256 };
    int iterations = 1000;
    double viewport_size = 1.0;
    double location[2] = { 1.0, 1.0};
    char *output_file_location = "output.png";
    RenderingMode rendering_mode = MANDELBROT_DEFAULT;

    char c;
    while ((c = getopt_long(argc, argv, "s:r:l:i:m:o:", options, NULL)) != -1) {
        switch (c) {
            case 's': {
                int result = sscanf(optarg, "%lf", &viewport_size);
                if (result != 1) {
                    printf("error: size: expected input as \"FLOAT\"\n");
                    exit(1);
                }
            } break;
            case 'r': {
                int result = sscanf(optarg, "%dx%d", &resolution[0], &resolution[1]);
                if (result != 2) {
                    printf("error: resolution: expected input as \"INTxINT\"\n");
                    exit(1);
                }
            } break;
            case 'l': {
                int result = sscanf(optarg, "%lfx%lf", &location[0], &location[1]);

                if (result != 2) {
                    printf("error: location: expected input as \"FLOAT,FLOAT\"\n");
                    exit(1);
                }
            } break;
            case 'i': {
                int result = sscanf(optarg, "%d", &iterations);

                if (result != 1) {
                    printf("error: iterations: expected input as \"INT\"\n");
                    exit(1);
                }
            } break;
            case 'm': {
                rendering_mode = rendering_mode_from_string(optarg);
                if (rendering_mode == 0) {
                    printf("error: rendering-mode: invalid rendering mode\n");
                    exit(1);
                }
            } break;
            case 'o':
                output_file_location = strdup(optarg);
                break;
            case 'h':
            case '?':
                print_usage(argv[0]);
                break;
        }
    }

    MandelbrotOptions options = {
        { resolution[0], resolution[1] },
        iterations,
        viewport_size,
        { location[0], location[1] },
        output_file_location,
        rendering_mode
    };

    return options;
}

int main(int argc, char *argv[]) {

    MandelbrotOptions options = parse_command_line_options(argc, argv);
    printf("resolution: %dx%d\niterations: %d\n"
           "viewport_size: %lf\nlocation: %lf,%lf\n"
           "output_file_location: %s\nrendering_mode: %d\n",
           options.resolution[0], options.resolution[1],
           options.iterations, options.viewport_size,
           options.location[0], options.location[1],
           options.output_file_location, options.rendering_mode);

    Mandelbrot mandelbrot = mandelbrot_new(options.resolution);
    set_mandelbrot_location(&mandelbrot, options.location);
    set_mandelbrot_zoom(&mandelbrot, options.viewport_size);
    set_mandelbrot_iterations(&mandelbrot, options.iterations);
    set_mandelbrot_rendering_mode(&mandelbrot, options.rendering_mode);

    bool result = mandelbrot_render_to_file(mandelbrot, options.output_file_location);
    if (result) {
        printf("error: could not open file %s\n", options.output_file_location);
        return 1;
    }
    return 0;
}
