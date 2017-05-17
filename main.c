#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

#include <cairo.h>
#include <cairo-xlib.h>

#include <X11/Xlib.h>

int main(int argc, char **argv) {
  Display *display;
  Window root;
  cario_surface_t *surface;
  float mspf, dt;
  int screen;
  struct timeval t0, t1;
  struct timespec s;
  long ms;
  if (argc != 3) {
    fprintf(sdterr, "Usage: %s <display> <fps>", argv[0]);
    return 1;
  }
  if ((mspf = 1.0f / atof(argv[2]) * 1000.0f) <= 0.0f || errno != 0) {
    fprintf(stderr, "Invalid framerate: %s", argv[2]);
    XCloseDisplay(display);
    return 1;
  }
  display = XOpenDisplay(argv[1]);
  if (!display) {
    fprintf(stderr, "Cannot open display: %s %s", argv[1], strerror(errno));
    return 1;
  }
  screen = DefaultScreen(display);
  root = DefaultRootWindow(display);
  gettimeofday(&t0, NULL);
  for (;;) {
    surface = cairo_xlib_surface_create(display,
                                        root,
                                        DisplayVisual(display, screen),
                                        DisplayWidth(display, screen),
                                        DisplayHeight(display, screen));
    cairo_write_to_png(surface, "frame.png");
    cairo_surface_destroy(surface);
    system("lpr frame.png");
    unlink("frame.png");
    gettimeofday(&t1, NULL);
    dt = (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
    ms = (long)(mspf - dt);
    s.tv_sec = ms / 1000;
    s.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&s, NULL);
    gettimeofday(&t0, NULL);
  }
  XCloseDisplay(display);
  return 0;
}
