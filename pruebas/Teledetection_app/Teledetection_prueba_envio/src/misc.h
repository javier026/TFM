#ifndef MISC_H
# define MISC_H

#include <bsp/bsp_printf.h>

/*
 * All functions not directly related to the logic of the algorithm.
*/

# define NB_CLASS 8 // numero de clases
# define VEC_SIZE 5 // XS?
# define LARGO_IMG 100
# define ANCHO_IMG 100
# define NB_PIXELS 9604//(LARGO_IMG-2)*(ANCHO_IMG-2) //= (fmt.nb_line - 2) * (fmt.nb_col - 2);

# define N_BEST 2
# define DEBUG 1



typedef struct
{
    unsigned nb_line;
    unsigned nb_col;
    unsigned nb_chan;
} ImageFormat;

typedef unsigned int size_t;
typedef unsigned char uint8_t;

size_t get_pos(size_t x, size_t y, int nb_col);
void sort(unsigned* values, unsigned len);
void grayscale(uint8_t* img_dst, ImageFormat fmt); //uint8_t* img_src,

unsigned* init_centers();
unsigned* init_classification(ImageFormat fmt);
unsigned** init_medians();
void init_pixels(unsigned pixels[NB_PIXELS][VEC_SIZE], ImageFormat fmt, uint8_t* img);

void free_medians(unsigned** medians);

void copy_array(unsigned src[NB_CLASS], unsigned dst[NB_CLASS]);
#endif /* MISC_H */
