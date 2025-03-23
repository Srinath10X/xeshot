#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <png.h>
#include <stdlib.h>

Display *display;

void saveImageToPNG(XImage *img, const char *path) {
  FILE *fp = fopen(path, "wb");
  if (!fp) return;

  png_structp p = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
  png_infop info = p ? png_create_info_struct(p) : 0;

  png_init_io(p, fp);
  png_set_IHDR(p, info, img->width, img->height, 8, PNG_COLOR_TYPE_RGB, 0, 0, 0), png_write_info(p, info);

  png_bytep r = malloc(img->width * 3);

  for (int y = 0; r && y < img->height; png_write_row(p, r), y++) {
    for (int x = 0; x < img->width; x++) {
      unsigned px = XGetPixel(img, x, y);
      r[x * 3] = px >> 16, r[x * 3 + 1] = px >> 8, r[x * 3 + 2] = px;
    }
  }

  png_write_end(p, 0), png_destroy_write_struct(&p, &info), free(r), fclose(fp);
}

int main(int argc, char *argv[]) {
  if (!(display = XOpenDisplay(NULL))) return 1;

  Window root = DefaultRootWindow(display);
  int screen = DefaultScreen(display);
  int width = DisplayWidth(display, screen);
  int height = DisplayHeight(display, screen);

  /* clang-format off */
	XImage *image = XGetImage(
		display, root, 0, 0, width, height,
		AllPlanes, ZPixmap
	);
  /* clang-format on */

  saveImageToPNG(image, "/tmp/screenshot.png");

  XDestroyImage(image);
  XCloseDisplay(display);
  return 0;
}
