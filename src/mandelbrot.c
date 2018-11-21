#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include <png.h>

#include "mandelbrot.h"

#define RETURN_IF_NULL(value) if (value == NULL) return NULL;
// static mandelbrot_function()

typedef struct PixelInfo {
    double cx;
    double cy;
    double escape;
} PixelInfo;

// initializes mandelbrot with a viewport size
Mandelbrot mandelbrot_new(int resolution[2]) {
    Mandelbrot mandelbrot = {
        .width = resolution[0],
        .height = resolution[1],
        .c_width = 0.0,
        .c_height = 0.0,
        .offset_x = 0.0,
        .offset_y = 0.0,
        .iterations = 0
    };

    return mandelbrot;
}

void set_mandelbrot_location(Mandelbrot *mandelbrot,
                             double location[2]) {
    mandelbrot->offset_x = location[0];
    mandelbrot->offset_y = location[1];
}

void set_mandelbrot_zoom(Mandelbrot *mandelbrot,
                         double zoom) {
    double aspect_ratio = (double) mandelbrot->width
                        / (double) mandelbrot->height;

    mandelbrot->c_height = zoom;
    mandelbrot->c_width = zoom * aspect_ratio;
}

void set_mandelbrot_iterations(Mandelbrot *mandelbrot,
                               int iterations) {
    mandelbrot->iterations = iterations;
}

void set_mandelbrot_rendering_mode(Mandelbrot *mandelbrot,
                                   RenderingMode rendering_mode) {
    mandelbrot->rendering_mode = rendering_mode;
}

static png_structp png_ptr_from_filename(const char *file_location,
                                         int height, int width) {
    FILE *fp = fopen(file_location, "wb");
    RETURN_IF_NULL(fp);

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                  NULL, NULL, NULL);
    RETURN_IF_NULL(png_ptr);

    png_infop info_ptr = png_create_info_struct(png_ptr);
    RETURN_IF_NULL(info_ptr);

    png_init_io(png_ptr, fp);

    png_set_IHDR(png_ptr, info_ptr, width, height, 8,
                 PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    return png_ptr;
}

static double compute_real_from_x(Mandelbrot *mandelbrot,
                                  double x) {
    double width = (double) mandelbrot->width;
    return x * mandelbrot->c_width / width - mandelbrot->offset_x;
}

static double compute_imaginary_from_y(Mandelbrot *mandelbrot,
                                       double y) {
    double width = (double) mandelbrot->width;
    return y * mandelbrot->c_width / width - mandelbrot->offset_y;
}

static void map_escape_to_color(int escape, int *color) {
    color[0] = (escape * 8) & 0xff;
    color[1] = (escape * 9) & 0xff;
    color[2] = (0xff - (escape * 4)) & 0xff;

}

static void linear_interpolation(int color1[3],
                                 int color2[3],
                                 double interp,
                                 int *new_color) {
    double r1 = (double) color1[0];
    double r2 = (double) color2[0];
    double g1 = (double) color1[1];
    double g2 = (double) color2[1];
    double b1 = (double) color1[2];
    double b2 = (double) color2[2];

    double r = (r2 - r1) * interp + r1;
    double g = (g2 - g1) * interp + g1;
    double b = (b2 - b1) * interp + b1;

    new_color[0] = (int) round(r);
    new_color[1] = (int) round(g);
    new_color[2] = (int) round(b);
}

static void render_pixel_default(PixelInfo pixel, int *output) {
    double escape = pixel.escape;

    int color1[3];
    int color2[3];

    if (escape != 0.0) {
        map_escape_to_color(floor(escape), color1);
        map_escape_to_color(floor(escape)+1, color2);

        double integral = 1.0;
        double fractional = modf(escape, &integral);
        linear_interpolation(color1, color2, fractional, output);
    } else {
        output[0] = 0;
        output[1] = 0;
        output[2] = 0;
    }
}

static void render_pixel_checkerboard(PixelInfo pixel, int *output) {
    double escape = pixel.escape;
    double cx = pixel.cx;
    output[0] = (int) (escape * cx) & 0xff;
    output[1] = (int) (escape * cx * 2.0) & 0xff;
    output[2] = (int) (escape * cx * 4.0) & 0xff;
}

static void render_pixel(PixelInfo pixel,
                         RenderingMode rendering_mode,
                         int *output) {
    switch (rendering_mode) {
        case MANDELBROT_CHECKERBOARD:
            render_pixel_checkerboard(pixel, output);
            break;
        case MANDELBROT_DEFAULT:
            render_pixel_default(pixel, output);
            break;
    }
}

static PixelInfo compute_pixel(Mandelbrot *mandelbrot,
                               int x, int y) {
    double c_real = compute_real_from_x(mandelbrot, (double) x);
    double c_img = compute_imaginary_from_y(mandelbrot, (double) y);

    int max_count = mandelbrot->iterations;
    double count = 0.0;

    double cx = 0.0;
    double cy = 0.0;

    double cx_tmp;
    while (cx*cx + cy*cy <= mandelbrot->rendering_mode && count < max_count) {
        cx_tmp = cx*cx - cy*cy + c_real;
        cy = 2.0 * cx * cy + c_img;
        cx = cx_tmp;

        count += 1.0;
    }

    PixelInfo pixel_info = { cx, cy, 0.0 };
    double log_zn, nu;
    if (count < max_count) {
        log_zn = log(cx*cx + cy*cy) / 2;
        nu = log(log_zn / log(2)) / log(2);

        count += 1 - nu;
        pixel_info.escape = count;
    }

    return pixel_info;
}

bool mandelbrot_render_to_file(Mandelbrot mandelbrot,
                               const char *file_location) {

    png_structp png_ptr = png_ptr_from_filename(file_location,
                                                mandelbrot.height,
                                                mandelbrot.width);

    png_bytep row = (png_bytep) malloc(3 * mandelbrot.width * sizeof(png_byte));

    for (int y = 0; y < mandelbrot.height; y++) {
        for (int x = 0; x < mandelbrot.width; x++) {
            PixelInfo pixel_info = compute_pixel(&mandelbrot, x, y);
            int color[3];

            // sets "color"
            render_pixel(pixel_info, mandelbrot.rendering_mode, color);

            row[x*3+0] = color[0];
            row[x*3+1] = color[1];
            row[x*3+2] = color[2];
        }
        png_write_row(png_ptr, row);
    }

    // End write
    png_write_end(png_ptr, NULL);
    return 0;
}











