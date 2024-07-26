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

void draw_text(cairo_t *cr, const char *text, double size, double x, double y) {
  cairo_set_font_size(cr, size);
  cairo_move_to(cr, x, y);
  cairo_show_text(cr, text);
}

void draw_centered_text(cairo_t *cr, const char *text, double size, double x,
                        double y) {
  cairo_set_font_size(cr, size);

  cairo_text_extents_t extents;
  cairo_text_extents(cr, text, &extents);
  cairo_move_to(cr, x - extents.width / 2 - extents.x_bearing,
                y - extents.height / 2 - extents.y_bearing);
  cairo_show_text(cr, text);
}

void write_image(cairo_t *cr) {
  char *filename = malloc(100);
  sprintf(filename, "output/%03d.png", currentSlide);
  cairo_surface_write_to_png(cairo_get_target(cr), filename);
  currentSlide++;
}

void title_slide(cairo_t *cr, const char *title) {
  double centerX = screenWidth / 2;
  double centerY = screenHeight / 2;

  set_defaults(cr);
  clear_screen(cr, 1.0, 0.9, 1.0);
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.3);
  draw_centered_text(cr, title, 200, centerX, centerY);

  write_image(cr);
}

int main() {
}
