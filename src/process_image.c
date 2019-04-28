#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

int get_pixel_index(image im, int x, int y, int c) {
  if (x >= im.w) {
    x = im.w - 1;
  } else if (x < 0) {
    x = 0;
  }

  if (y >= im.h) {
    y = im.h - 1;
  } else if (y < 0) {
    y = 0;
  }

  return x + im.w*y + im.w*im.h*c;
}

float get_pixel(image im, int x, int y, int c)
{
    int index = get_pixel_index(im, x, y, c);
    float pixel = im.data[index];
    return pixel;
}

void set_pixel(image im, int x, int y, int c, float v)
{
    int index = get_pixel_index(im, x, y, c);
    im.data[index] = v;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    memcpy(copy.data, im.data, im.h*im.w*im.c*sizeof(float));
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);

    int x, y;
    float rPixel, gPixel, bPixel, grayscalePixel;
    const float rWeight = 0.299,
      gWeight = 0.587,
      bWeight = 0.114;

    for(x = 0; x < im.w; x++) {
      for(y = 0; y < im.h; y++) {
        rPixel = get_pixel(im, x, y, 0);
        gPixel = get_pixel(im, x, y, 1);
        bPixel = get_pixel(im, x, y, 2);

        // Y' = 0.299 R' + 0.587 G' + .114 B'
        grayscalePixel = (rPixel * rWeight) +
          (gPixel * gWeight) +
          (bPixel * bWeight);

        set_pixel(gray, x, y, 0, grayscalePixel);
      }
    }

    return gray;
}

void shift_image(image im, int c, float v)
{
    int x, y, pixel_index;
    for (x = 0; x < im.w; x++) {
      for (y = 0; y < im.h; y++) {
        pixel_index = get_pixel_index(im, x, y, c);
        float existing_value = im.data[pixel_index];
        // float increase = existing_value * v;
        float new_value = existing_value + v;
        if (new_value > 255) {
          new_value = new_value / 255.;
        }
        im.data[pixel_index] = new_value;
      }
    }
}

void clamp_image(image im)
{
  int x, y, c;
  float pix_value;
  for (c = 0; c < im.c; c++) {
    for (x = 0; x < im.w; x++) {
      for (y = 0; y < im.h; y++) {
        // pixel_index = get_pixel_index(im, x, y, c);
        // pix_value = im.data[pixel_index];
        pix_value = get_pixel(im, x, y, c);
        if (pix_value < 0) {
          pix_value = 0;
        }
        if (pix_value > 1) {
          pix_value = 1;
        }

        set_pixel(im, x, y, c, pix_value);
      }
    }
  }
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    int x, y;
    float hue,
          hue_prime,
          saturation,
          max_pixel_value,
          min_pixel_value,
          delta,
          rPixel,
          gPixel,
          bPixel;

    for (x = 0; x < im.w; x++) {
      for (y = 0; y < im.h; y++) {

        int checkHue = 1;

        rPixel = get_pixel(im, x, y, 0);
        gPixel = get_pixel(im, x, y, 1);
        bPixel = get_pixel(im, x, y, 2);

        max_pixel_value = three_way_max(rPixel, gPixel, bPixel);
        min_pixel_value = three_way_min(rPixel, gPixel, bPixel);

        delta = max_pixel_value - min_pixel_value;

        if (max_pixel_value == 0) {
          saturation = 0;
        }
        else {
          // printf("delta is not 0\n");
          saturation = delta / max_pixel_value;
        }

        if (delta == 0) {
          hue = 0;
          // printf("delta 0\n");
          checkHue = 0;
        }
        else if(max_pixel_value == rPixel) {
          // printf("Max is red\n");
          hue_prime = ((gPixel - bPixel) / delta);
        }
        else if(max_pixel_value == gPixel) {
          // printf("Max is green\n");
          hue_prime = ((bPixel - rPixel) / delta) + 2;
        }
        else if(max_pixel_value == bPixel) {
          // printf("Max is blue\n");
          hue_prime = ((rPixel - gPixel) / delta) + 4;
        }

        if (checkHue > 0) {
          if (hue_prime < 0) {
            hue = (hue_prime / 6) + 1;
          } else {
            hue = hue_prime / 6;
          }
        }

        // Now set the pixels back on the channel
        set_pixel(im, x, y, 0, hue);
        set_pixel(im, x, y, 1, saturation);
        set_pixel(im, x, y, 2, max_pixel_value);
      }
    }
}

void hsv_to_rgb(image im)
{
    int x, y;
    float hue,
          hue_prime,
          saturation,
          value,
          chroma,
          intermed;

    for(x = 0; x < im.w; x++) {
      for(y = 0; y < im.h; y++) {
        hue = get_pixel(im, x, y, 0);
        saturation = get_pixel(im, x, y, 1);
        value = get_pixel(im, x, y, 2);

        if (hue == 0 && saturation == 0 && value == 0) {
          set_pixel(im, x, y, 0, 0);
          set_pixel(im, x, y, 1, 0);
          set_pixel(im, x, y, 2, 0);
          return;
        }
        else {
          chroma = value * saturation;
          hue_prime = hue / 60.0;
          intermed = chroma * (1 - (abs(fmod(hue_prime, 2) - 1)));

          // TODO: Finish this.
        }
      }
    }
}
