# mandelbrot

![a mandelbrot rendered with default rendering](images/default.png)
![a mandelbrot rendered with checkerboard rendering](images/checkerboard.png)

# Building

Dependencies: libpng

```
git clone https://github.com/kil0meters/mandelbrot
cd mandelbrot
make
```

# Usage

```
./mandelbrot [ -s FLOAT | -r INTxINT | -l FLOATxFLOAT | -i INT
              -m RENDERING_MODE | -o STRING ]

  -s, --size            Viewport size multiplier [DEFAULT: 1.0]
  -r, --resolution      Render resolution [DEFAULT: 256x256]
  -l, --location        Viewport location [DEFAULT: 1.0x1.0]
  -i, --iterations      Number of iterations [DEFAULT: 1000]
  -m, --rendering-mode  Rendering mode [DEFAULT: default]
  -o, --output          Output file location [DEFAULT: output.png]
```
