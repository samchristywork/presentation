#include <cairo/cairo.h>
#include <linux/limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "args.h"

#define VERSION_STRING "presentation-1.0.0"

#define LICENSE_STRING                                                         \
  "Copyright (C) 2024 Sam Christy.\n"                                          \
  "License GPLv3+: GNU GPL version 3 or later "                                \
  "<http://gnu.org/licenses/gpl.html>\n"                                       \
  "\n"                                                                         \
  "This is free software; you are free to change and redistribute it.\n"       \
  "There is NO WARRANTY, to the extent permitted by law."

double screenWidth;
double screenHeight;
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
  char *filename = malloc(PATH_MAX);
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

void image_slide(cairo_t *cr, const char *filename) {
  cairo_surface_t *image = cairo_image_surface_create_from_png(filename);
  if (cairo_surface_status(image) != CAIRO_STATUS_SUCCESS) {
    fprintf(stderr, "Failed to load image\n");
    return;
  }

  double margin = 0.1;

  double imageWidth = cairo_image_surface_get_width(image);
  double imageHeight = cairo_image_surface_get_height(image);

  double targetWidth = screenWidth * (1 - 2 * margin);
  double targetHeight = screenHeight * (1 - 2 * margin);

  clear_screen(cr, 1.0, 0.9, 1.0);

  cairo_save(cr);
  cairo_translate(cr, screenWidth * margin, screenHeight * margin);
  cairo_scale(cr, targetWidth / imageWidth, targetHeight / imageHeight);
  cairo_set_source_surface(cr, image, 0, 0);
  cairo_paint(cr);
  cairo_restore(cr);

  write_image(cr);

  cairo_surface_destroy(image);
}

void bullet_slide(cairo_t *cr, const char *title, ...) {
  va_list args;
  va_start(args, title);

  set_defaults(cr);
  clear_screen(cr, 1.0, 0.9, 1.9);
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.3);
  draw_text(cr, title, 200, 100, 300);

  double y = 400;
  const char *bullet;
  while ((bullet = va_arg(args, const char *)) != NULL) {
    y += 150;
    draw_text(cr, bullet, 100, 200, y);
  }

  write_image(cr);

  va_end(args);
}

cairo_t *init_cairo() {
  cairo_surface_t *surface = cairo_image_surface_create(
      CAIRO_FORMAT_ARGB32, screenWidth, screenHeight);
  if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
    fprintf(stderr, "Failed to create surface\n");
    return NULL;
  }

  cairo_t *cr = cairo_create(surface);
  if (cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
    fprintf(stderr, "Failed to create cairo context\n");
    return NULL;
  }

  return cr;
}

int main(int argc, char *argv[]) {
  add_arg('h', "help", "Print this help message", ARG_NONE);
  add_arg('v', "version", "Print the version number", ARG_NONE);
  add_arg('o', "output", "Output directory", ARG_REQUIRED);
  add_arg('x', "width", "Screen width", ARG_REQUIRED);
  add_arg('y', "height", "Screen height", ARG_REQUIRED);

  bool helpFlag = get_arg_bool(argc, argv, 'h', false);
  bool versionFlag = get_arg_bool(argc, argv, 'v', false);
  char *directory = get_arg_string(argc, argv, 'o', "output");

  screenWidth = get_arg_int(argc, argv, 'x', 1920);
  screenHeight = get_arg_int(argc, argv, 'y', 1080);

  cairo_t *cr = init_cairo();
  if (cr == NULL) {
    return 1;
  }

  if (helpFlag) {
    usage(argv[0]);
    exit(EXIT_SUCCESS);
  }

  if (versionFlag) {
    version(VERSION_STRING, LICENSE_STRING);
    exit(EXIT_SUCCESS);
  }

  if (!make_directory_if_not_exists(directory)) {
    fprintf(stderr, "Failed to create output directory\n");
  }

  title_slide(cr, "Hello, World!");
  bullet_slide(cr, "Fizz", "• foo", "• bar", "• baz", NULL);
  image_slide(cr, "image.png");
  title_slide(cr, "Goodbye, World!");

  cairo_destroy(cr);
  cairo_surface_destroy(cairo_get_target(cr));
}
