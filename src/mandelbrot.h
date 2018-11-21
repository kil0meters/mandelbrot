#ifndef MANDELBROT_H_
#define MANDELBROT_H_

typedef enum RenderingMode {
    MANDELBROT_DEFAULT = 1 << 16,
    MANDELBROT_CHECKERBOARD = 4,
} RenderingMode;

typedef struct Mandelbrot {
    int height;
    int width;
    double c_height; // complex plane height
    double c_width;  // complex plane width
    double offset_x;
    double offset_y;
    RenderingMode rendering_mode;
    int iterations;
} Mandelbrot;

Mandelbrot mandelbrot_new(int resolution[2]);
void set_mandelbrot_location(Mandelbrot *mandelbrot,
                             double location[2]);
void set_mandelbrot_zoom(Mandelbrot *mandelbrot,
                         double zoom);
void set_mandelbrot_iterations(Mandelbrot *mandelbrot,
                               int iterations);
void set_mandelbrot_rendering_mode(Mandelbrot *mandelbrot,
                                   RenderingMode rendering_mode);
_Bool mandelbrot_render_to_file(Mandelbrot mandelbrot,
                                const char *file_location);

#endif
