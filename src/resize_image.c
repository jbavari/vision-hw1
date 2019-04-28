#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    return get_pixel(im, floor(x), floor(y), c);
}

image nn_resize(image im, int w, int h)
{
    // TODO Fill in (also fix that first line)
    int channel, x, y;
    image new_image = make_image(w, h, im.c);

    float scale_x = w / im.w,
        scale_y = h / im.h;

    printf("Scaling image from %d/%d, to : %d/%d -> scaling %f, %f",
      im.w, im.h, w, h,
      scale_x, scale_y);

    for(channel = 0; channel < im.c; channel++) {
      for(x = 0; x < w; x++) {
        for(y = 0; y < h; y++) {
          float corresponding_x = x / scale_x,
            corresponding_y = y / scale_y;

          float new_value = nn_interpolate(im, corresponding_x,
            corresponding_y, channel);

          set_pixel(new_image, x, y, channel, new_value);
        }
      }
    }

    return new_image;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    // TODO - fill in this part with the bilinear interpolation.
    return 0;
}

image bilinear_resize(image im, int w, int h)
{
    int x, y, channel;
    image new_image = make_image(w, h, im.c);

    float scale_x = im.w / w,
          scale_y = h / im.h;

    for(channel = 0; channel < im.c; channel++) {
      for(x = 0; x < w; x++) {
        for(y = 0; y < h; y++) {
          float corresponding_x = x * scale_x,
            corresponding_y = y * scale_y;

          int topX, topY, bottomX, bottomY;

          // topX = x -

          float new_value = nn_interpolate(im, corresponding_x,
            corresponding_y, channel);

          set_pixel(new_image, x, y, channel, new_value);
        }
      }
    }
    return make_image(1,1,1);
}

