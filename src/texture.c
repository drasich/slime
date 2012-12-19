#include "texture.h"
#include <png.h>

Texture*
texture_read_png_file(const char *path)
{
  FILE *f;
  png_byte magic[8];

  f = fopen(path, "rb");
  if (!f) {
    fprintf (stderr, "error: couldn't open \"%s\"!\n", path);
    return NULL;
  }

  /* read magic number */
  fread(magic, 1, sizeof(magic), f);

  /* check for valid magic number */
  if (!png_check_sig(magic, sizeof(magic))) {
    fprintf(stderr, "error: \"%s\" is not a valid PNG image!\n",
          path);
    fclose(f);
    return NULL;
  }

  /* create a png read struct */
  png_structp png_ptr = png_create_read_struct(
        PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr) {
    fclose(f);
    return NULL;
  }

  /* create a png info struct */
  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    fclose(f);
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return NULL;
  }

  Texture *tex = (Texture*) malloc(sizeof(Texture));

  /* initialize the setjmp for returning properly after a libpng
     error occured */
  if (setjmp(png_jmpbuf(png_ptr))) {
    fclose(f);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    if (tex) {
      if (tex->data)
      free(tex->data);

      free(tex);
    }

    return NULL;
  }

  /* setup libpng for using standard C fread() function
     with our FILE pointer */
  png_init_io(png_ptr, f);

  /* tell libpng that we have already read the magic number */
  png_set_sig_bytes(png_ptr, sizeof(magic));

  /* read png info */
  png_read_info(png_ptr, info_ptr);

  /* get some usefull information from header */
  int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  int color_type = png_get_color_type(png_ptr, info_ptr);

  /* convert index color images to RGB images */
  if (color_type == PNG_COLOR_TYPE_PALETTE)
  png_set_palette_to_rgb(png_ptr);

  /* convert 1-2-4 bits grayscale images to 8 bits
     grayscale. */
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
  png_set_expand_gray_1_2_4_to_8(png_ptr);

  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
  png_set_tRNS_to_alpha(png_ptr);

  if (bit_depth == 16)
  png_set_strip_16(png_ptr);
  else if (bit_depth < 8)
  png_set_packing(png_ptr);

  /* update info structure to apply transformations */
  png_read_update_info(png_ptr, info_ptr);

  /* retrieve updated information */
  png_get_IHDR(png_ptr, info_ptr,
        (png_uint_32*)(&tex->width),
        (png_uint_32*)(&tex->height),
        &bit_depth, &color_type,
        NULL, NULL, NULL);

  switch (color_type) {
    case PNG_COLOR_TYPE_GRAY:
      tex->format = GL_LUMINANCE;
      tex->internal_format = 1;
      break;

    case PNG_COLOR_TYPE_GRAY_ALPHA:
      tex->format = GL_LUMINANCE_ALPHA;
      tex->internal_format = 2;
      break;

    case PNG_COLOR_TYPE_RGB:
      tex->format = GL_RGB;
      tex->internal_format = 3;
      break;

    case PNG_COLOR_TYPE_RGB_ALPHA:
      tex->format = GL_RGBA;
      tex->internal_format = 4;
      break;

    default:
      printf("color type not found\n");
      break;
  }


  /* we can now allocate memory for storing pixel data */
  tex->data = (GLubyte*) malloc(sizeof(GLubyte) * tex->width
        * tex->height * tex->internal_format);

  png_bytep* row_pointers;

  /* setup a pointer array.  Each one points at the begening of a row. */
  row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * tex->height);

  int i;
  for (i = 0; i < tex->height; ++i) {
    row_pointers[i] = (png_bytep)(tex->data +
          ((tex->height - (i + 1)) * tex->width * tex->internal_format));
  }

  /* read pixel data using row pointers */
  png_read_image(png_ptr, row_pointers);

  /* we don't need row pointers anymore */
  free(row_pointers);

  /* finish decompression and release memory */
  png_read_end(png_ptr, NULL);
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

  fclose(f);

  return tex;
}


