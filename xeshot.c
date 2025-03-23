#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  Window root;
  XImage *image;
  Display *display;

  int screen;
  int x, y, height, width;
} App;

void help() {
  /* clang-format off */
  puts(
		"Usage: xeshot [OPTION]\n"
    "A minimal X11 screenshort tool written in C.\n"
    "\nOptions:\n"
    "    -h, --help                 show list of available options\n"
    "    -v, --version              show version of this cli-app\n"
		"    -g, --geometry <geometry>  set the region to capture screen"
	);
  /* clang-format on */
}

void version() { puts("xeshot v1.0.1-dev"); }

void setup_x11(App *app) {
  app->display = XOpenDisplay(NULL);

  app->screen = DefaultScreen(app->display);
  app->root = DefaultRootWindow(app->display);

  app->x = 0, app->y = 0;
  app->width = DisplayWidth(app->display, app->screen);
  app->height = DisplayHeight(app->display, app->screen);
}

void parse_geometry(const char *geometry, App *app) {
  if (sscanf(geometry, "%dx%d+%d+%d", &app->width, &app->height, &app->x, &app->y) != 4)
    fprintf(stderr, "Error: Invalid geometry format. Use WxH+X+Y (e.g., 300x300+0+0).\n"), exit(1);
}

void getImage(App *app) {
  /* clang-format off */
  app->image = XGetImage(
		app->display, app->root, app->x, app->y,
		app->width, app->height, AllPlanes, ZPixmap
	);
  /* clang-format on */
}

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

void cleanup(App *app) {
  if (app->image) XDestroyImage(app->image);
  if (app->display) XCloseDisplay(app->display);
}

int main(int argc, char **argv) {
  App app = {0};
  setup_x11(&app);

  /* Handling command line arguments */
  if (argc > 1) {
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
      help();
      return 0;
    } else if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
      version();
      return 0;
    } else if (strcmp(argv[1], "-g") == 0 || strcmp(argv[1], "--geometry") == 0) {
      if (argc < 3) {
        printf("Error: Missing arguments for -g.");
        return 0;
      }
      parse_geometry(argv[2], &app);
    } else {
      puts("Unknown option. Use --help to see available options.\n");
    }
  }

  getImage(&app);

  char *dir = getenv("XESHOT_DEFAULT_DIR");
  saveImageToPNG(app.image, dir ? strcat(dir, "xeshot.png") : "/tmp/xeshot.png");

  cleanup(&app);
  return 0;
}
