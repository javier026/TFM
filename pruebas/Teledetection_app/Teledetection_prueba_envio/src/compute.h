#ifndef COMPUTE_H
# define COMPUTE_H

//# include <gtk/gtk.h>
# include "misc.h"


# define LargeurImage 360

void ComputeImage(uint8_t *img_src, int nb_col, int nb_line, uint8_t *img_dst);
double get_percent_clouds(uint8_t *img_dst, ImageFormat fmt); //uint8_t *img_src,
int draw_clouds(uint8_t* img, ImageFormat fmt, unsigned* classification);

int kmeans(unsigned pixels[NB_PIXELS][VEC_SIZE], ImageFormat fmt, uint8_t* img);
int has_changed(unsigned* a, unsigned b[NB_CLASS]);

void classify(unsigned nb_per_class[NB_CLASS], unsigned* centers,
              unsigned* classification, unsigned pixels[NB_PIXELS][VEC_SIZE],
              ImageFormat fmt);
int distance(unsigned pixels[VEC_SIZE], unsigned center);

void update_medians(unsigned nb_per_class[NB_CLASS], unsigned* classification,
                    unsigned medians[NB_CLASS][(LARGO_IMG-1)*(ANCHO_IMG-1)*VEC_SIZE], unsigned pixels[NB_PIXELS][VEC_SIZE], ImageFormat fmt);
void update_centers(unsigned nb_per_class[NB_CLASS], unsigned* centers,
                    unsigned medians[NB_CLASS][(LARGO_IMG-1)*(ANCHO_IMG-1)*VEC_SIZE]);

#endif /* COMPUTE_H */
