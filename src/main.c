#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include <string.h>
#include <png.h>

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
}
pixel_t;
    
typedef struct
{
    pixel_t *pixels;
    size_t width;
    size_t height;
}
bitmap_t;

static pixel_t * pixel_at (bitmap_t * bitmap, int x, int y)
{
    return bitmap->pixels + bitmap->width * y + x;
}

static int save_png_to_file (bitmap_t *bitmap, const char *path)
{
    FILE * fp;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    size_t x, y;
    png_byte ** row_pointers = NULL;
    int status = -1;
    int pixel_size = 3;
    int depth = 8; 
    fp = fopen (path, "wb");
    if (! fp) {
        goto fopen_failed;
    }
    png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        goto png_create_write_struct_failed;
    }
    info_ptr = png_create_info_struct (png_ptr);
    if (info_ptr == NULL) {
        goto png_create_info_struct_failed;
    }
    if (setjmp (png_jmpbuf (png_ptr))) {
        goto png_failure;
    }
    png_set_IHDR (png_ptr,
                  info_ptr,
                  bitmap->width,
                  bitmap->height,
                  depth,
                  PNG_COLOR_TYPE_RGB,
                  PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_DEFAULT,
                  PNG_FILTER_TYPE_DEFAULT);
    row_pointers = png_malloc (png_ptr, bitmap->height * sizeof (png_byte *));
    for (y = 0; y < bitmap->height; y++) {
        png_byte *row = 
            png_malloc (png_ptr, sizeof (uint8_t) * bitmap->width * pixel_size);
        row_pointers[y] = row;
        for (x = 0; x < bitmap->width; x++) {
            pixel_t * pixel = pixel_at (bitmap, x, y);
            *row++ = pixel->red;
            *row++ = pixel->green;
            *row++ = pixel->blue;
        }
    }
    png_init_io (png_ptr, fp);
    png_set_rows (png_ptr, info_ptr, row_pointers);
    png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    status = 0;
    for (y = 0; y < bitmap->height; y++) {
        png_free (png_ptr, row_pointers[y]);
    }
    png_free (png_ptr, row_pointers);
    
 png_failure:
 png_create_info_struct_failed:
    png_destroy_write_struct (&png_ptr, &info_ptr);
 png_create_write_struct_failed:
    fclose (fp);
 fopen_failed:
    return status;
}

int calc_mandelbrot(double complex c, int iter)
{
    int ret;
    double complex z = c;
    for(int i = 0; i < iter; i++)
    {
        if(cabs(z) > 2)
            return i;
        z = z*z + c;
    }
    return iter;
}

void fractal(char* imageFileName, double xmin, double xmax, double ymin, double ymax, int res, int iter)
{
    double* x = malloc(sizeof(double) * res + 1);
    double* y = malloc(sizeof(double) * res + 1);

    double rxmax = fabs(xmin - xmax);
    double rymax = fabs(ymin - ymax);

    double percent;

    for(int i = 0; i < res + 1; i++)
    {
        percent = ((double)i / res);
        x[i] = rxmax * percent + xmin;
        y[i] = rymax * percent + ymin;
    } 



    bitmap_t mandelbrot;

    mandelbrot.width = res;
    mandelbrot.height = res;

    mandelbrot.pixels = calloc (mandelbrot.width * mandelbrot.height, sizeof (pixel_t));

    if (! mandelbrot.pixels) {
        printf("gay\n");
    }

    double n;
    double pval;

    for (int i = 0; i < mandelbrot.height; i++) {
        for (int j = 0; j < mandelbrot.width; j++) {
            pixel_t * pixel = pixel_at (& mandelbrot, i, j);

            n = calc_mandelbrot(x[i] + I * y[j], iter);

            pval = (n / (double)iter) * 255;

            pixel->red = pval;
            pixel->green = pval;
            pixel->blue = pval;
        }
    }

    /* Write the image to a file 'mandelbrot.png'. */

    if (save_png_to_file (& mandelbrot, imageFileName)) {
	fprintf (stderr, "Error writing file.\n");
    }

    free (mandelbrot.pixels);

    return;
}

int main(int argc, char* argv[])
{
    //arguments
    char filename[50];
    double len = 2.0;
    double mult = 0.8;
    double zoomx = 0.0;
    double zoomy = -1.47;
    int n = 60;

    for(int i = 0; i < n; i++)
    {
        sprintf(filename, "img/%d.png", i);
        fractal(filename, zoomy-len, zoomy+len, zoomx-len, zoomx+len, 2000, 100);
        len *= mult;
        printf("\rcomputed %d/%d", i, n);
        fflush(stdout);
    }

    return 0;
}