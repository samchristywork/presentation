#include <cairo/cairo.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

double screenWidth = 1920;
double screenHeight = 1080;
int currentSlide = 0;

void clear_screen(cairo_t *cr, double r, double g, double b) {
  cairo_set_source_rgb(cr, r, g, b);
  cairo_paint(cr);
}

void set_defaults(cairo_t *cr) {
  cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                         CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, 40.0);
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
}

bool make_directory_if_not_exists(const char *dir) {
  struct stat st = {0};
  if (stat(dir, &st) == -1) {
    if (mkdir(dir, 0700) == -1) {
      return false;
    }
  }
  return true;
}

int main() {
}
