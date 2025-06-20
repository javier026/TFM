# include "misc.h"
#include "compute.h"


unsigned pixels[NB_PIXELS][VEC_SIZE];
unsigned history_centers[NB_CLASS];
unsigned medians[NB_CLASS][(LARGO_IMG-1)*(ANCHO_IMG-1)*VEC_SIZE];
unsigned classification[NB_PIXELS];
unsigned nb_per_class[NB_CLASS]; 
unsigned nb_per_class2[NB_CLASS]; 

void ComputeImage(uint8_t *img_src, int nb_line, int nb_col, uint8_t *img_dst)
{
  ImageFormat fmt =
  {
    .nb_line = nb_line,
    .nb_col = nb_col,
    .nb_chan = 3
  };

  get_percent_clouds(img_dst, fmt); //img_src,
}

double get_percent_clouds(uint8_t *img_dst, ImageFormat fmt) //uint8_t *img_src
{
  grayscale(img_dst, fmt); 

  init_pixels(pixels, fmt, img_dst);

  int nb_clouds = kmeans(pixels, fmt, img_dst);
  double percent = ((double)nb_clouds) / NB_PIXELS * 100;

  return percent;
}


int kmeans(unsigned pixels[NB_PIXELS][VEC_SIZE], ImageFormat fmt, uint8_t* img)
{
  unsigned centers[NB_CLASS]; 
  unsigned range = 255 / (NB_CLASS - 1);

  for (size_t i = 0; i < NB_CLASS; i++)
    centers[i] = range * i;

  int loop_counter = 0;
  do // Loop of the K-means algorithm
  {
    copy_array(centers, history_centers); // Save previous mass centers

    // 1. Associate pixels to mass centers
    classify(nb_per_class, centers, classification, pixels, fmt);

    // 2. Update mass centers
    copy_array(nb_per_class, nb_per_class2);
    update_medians(nb_per_class, classification, medians, pixels, fmt); //cambiar malloc de dentro
    update_centers(nb_per_class2, centers, medians);

  } while (loop_counter++ < 100 && has_changed(centers, history_centers));
  printfNexys("Vueltas que ha dado: %d\n", loop_counter-1);
  classify(nb_per_class2, centers, classification, pixels, fmt);

  int nb_clouds = draw_clouds(img, fmt, classification);

  return nb_clouds;
}


int draw_clouds(uint8_t* img, ImageFormat fmt, unsigned* classification)
{
  int nb_clouds = 0;
  for (size_t x = 1; x < fmt.nb_col-1; x++)
  {
    for (size_t y = 1; y < fmt.nb_line-1; y++)
    {
      size_t pos = get_pos(x-1, y-1, fmt.nb_col-2);
      size_t class = classification[pos];

      if (class >= NB_CLASS - N_BEST)
      {
        nb_clouds++;
        size_t pos_img = get_pos(x, y, fmt.nb_col) * 3;
        img[pos_img] = 255;
        img[pos_img+1] = 0;
        img[pos_img+2] = 0;
      }
    }
  }

  return nb_clouds;
}

void update_centers(unsigned nb_per_class[NB_CLASS], unsigned* centers,
                    unsigned medians[NB_CLASS][(LARGO_IMG-1)*(ANCHO_IMG-1)*VEC_SIZE])
{
  for (size_t i = 0; i < NB_CLASS; i++)
  {
    sort(medians[i], nb_per_class[i]);
    if (nb_per_class[i])
      centers[i] = medians[i][nb_per_class[i] / 2];
  }
}

void update_medians(unsigned nb_per_class[NB_CLASS], unsigned* classification,
                    unsigned medians[NB_CLASS][(LARGO_IMG-1)*(ANCHO_IMG-1)*VEC_SIZE], unsigned pixels[NB_PIXELS][VEC_SIZE], ImageFormat fmt)
{  
  for (size_t x = 1; x < fmt.nb_col-1; x++)
  {
    for (size_t y = 1; y < fmt.nb_line-1; y++) //comprobar
    {
      size_t pos = get_pos(x-1, y-1, fmt.nb_col-2);
      size_t class = classification[pos];
      for (size_t i = 0; i < VEC_SIZE; i++){
        medians[class][(nb_per_class[class] - 1) * VEC_SIZE + i] = pixels[pos][i];
      }
      nb_per_class[class]--;
    }
  }
}


void classify(unsigned nb_per_class[NB_CLASS], unsigned* centers,
              unsigned* classification,unsigned pixels[NB_PIXELS][VEC_SIZE],
              ImageFormat fmt)
{
  for (size_t i = 0; i < NB_CLASS; i++)
    nb_per_class[i] = 0;

  unsigned best_class;
  int dist;//double dist = 1.7976931348623157e+308;
  int new_dist;

  for (size_t x = 1; x < fmt.nb_col-1; x++)
  {
    for (size_t y = 1; y < fmt.nb_line-1; y++)
    {
      size_t pos = get_pos(x-1, y-1, fmt.nb_col-2);

      best_class = 0;
      dist = 1275;

      //NB_CLASS

      new_dist = distance(pixels[pos], centers[0]);
      if (new_dist < dist)
      {
        dist = new_dist;
      }

      new_dist = distance(pixels[pos], centers[1]);
      if (new_dist < dist)
      {
        best_class = 1;
        dist = new_dist;
      }

      new_dist = distance(pixels[pos], centers[2]);
      if (new_dist < dist)
      {
        best_class = 2;
        dist = new_dist;
      }

      new_dist = distance(pixels[pos], centers[3]);
      if (new_dist < dist)
      {
        best_class = 3;
        dist = new_dist;
      }

      new_dist = distance(pixels[pos], centers[4]);
      if (new_dist < dist)
      {
        best_class = 4;
        dist = new_dist;
      }

      new_dist = distance(pixels[pos], centers[5]);
      if (new_dist < dist)
      {
        best_class = 5;
        dist = new_dist;
      }

      new_dist = distance(pixels[pos], centers[6]);
      if (new_dist < dist)
      {
        best_class = 6;
        dist = new_dist;
      }

      new_dist = distance(pixels[pos], centers[7]);
      if (new_dist < dist)
      {
        best_class = 7;
        dist = new_dist;
      }

      classification[pos] = best_class;
      nb_per_class[best_class]++;
    }
  }
}

int distance(unsigned pixels[VEC_SIZE], unsigned center)
{
  int dist =0;
  unsigned int a;

    //VEC_SIZE
    a = pixels[0];
    if(a>center) dist += (a - center);
    else dist += (center - a);

    a = pixels[1];
    if(a>center) dist += (a - center);
    else dist += (center - a);

    a = pixels[2];
    if(a>center) dist += (a - center);
    else dist += (center - a);

    a = pixels[3];
    if(a>center) dist += (a - center);
    else dist += (center - a);

    a = pixels[4];
    if(a>center) dist += (a - center);
    else dist += (center - a);
  
  return dist;
}

int has_changed(unsigned* a, unsigned b[NB_CLASS])
{
    //NB_CLASS
    if (a[0] != b[0])
      return 1;
    if (a[1] != b[1])
      return 1;
    if (a[2] != b[2])
      return 1;
    if (a[3] != b[3])
      return 1;
    if (a[4] != b[4])
      return 1;
    if (a[5] != b[5])
      return 1;
    if (a[6] != b[6])
      return 1;
    if (a[7] != b[7])
      return 1;

  return 0;
}



