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

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} pixel_t;

//void save_png(GLfloat* pix)
void save_png(GLuint* pix, int width, int height)
{
  FILE * fp;
  png_structp png_ptr = NULL;
  png_infop info_ptr = NULL;
  size_t x, y;
  png_byte ** row_pointers = NULL;
  /* "status" contains the return value of this function. At first
     it is set to a value which means 'failure'. When the routine
     has finished its work, it is set to a value which means
     'success'. */
  int status = -1;
  /* The following number is set by trial and error only. I cannot
     see where it it is documented in the libpng manual.
     */
  int pixel_size = 3;
  int depth = 8;

  fp = fopen ("testdepth.png", "wb");
  if (! fp) {
    printf("cannot open\n");
    return;
  }

  png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    printf("error 0\n");
    return;
  }

  info_ptr = png_create_info_struct (png_ptr);
  if (info_ptr == NULL) {
    printf("error 1\n");
    return;
  }

  /* Set up error handling. */

  if (setjmp (png_jmpbuf (png_ptr))) {
    printf("error 2\n");
    return;
  }

  /* Set image attributes. */
  //int width = 1200;
  //int height = 400;

  png_set_IHDR (png_ptr,
        info_ptr,
        width,
        height,
        depth,
        PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    /* Initialize rows of PNG. */

  row_pointers = png_malloc (png_ptr, height * sizeof (png_byte *));
    for (y = 0; y < height; ++y) {
        png_byte *row = 
            png_malloc (png_ptr, sizeof (uint8_t) * width * pixel_size);
        row_pointers[y] = row;
        for (x = 0; x < width; ++x) {
          //float f = pix[y*width +x]*255;
          //float f = (pix[y*width +x]>>8) / 16777215.0f * 255.0f ;
          //float f = (pix[y*width +x]>>8);
          //float f = (pix[y*width +x]>>8) / 65536.0f * 255.0f ;
          float f = (pix[y*width +x]&0xff)*255;
          //float f = (pix[y*width +x]&0xff);
          uint32_t utest = (pix[y*width +x]&0xff);
          if (y*width +x == 0)  {
          //printf(" the value u : %u \n", pix[0]&0xff);
          printf(" the value f : %f \n", f);
          printf(" the value utest : %x \n", utest);
                }
            pixel_t yep = { f,f,f};
            pixel_t * pixel = &yep;
            *row++ = pixel->red;
            *row++ = pixel->green;
            *row++ = pixel->blue;
        }
    }
    
    /* Write the image data to "fp". */

    png_init_io (png_ptr, fp);
    png_set_rows (png_ptr, info_ptr, row_pointers);
    png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    /* The routine has successfully written the file, so we set
       "status" to a value which indicates success. */

    status = 0;
    
    for (y = 0; y < height; y++) {
        png_free (png_ptr, row_pointers[y]);
    }
    png_free (png_ptr, row_pointers);
    
    return;
}

Texture*
texture_new()
{
  Texture* t = calloc(1, sizeof *t);
  return t;
}

void
texture_init(Texture* tex)
{
  if (tex->is_init) return;

  /*
  if (texture_png_read(tex))
  return;
  */

  GLuint idtex;
  glGenTextures(1, &idtex);
  tex->id = idtex;

	glBindTexture(GL_TEXTURE_2D, idtex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glTexImage2D(
        GL_TEXTURE_2D,
        0,
        //GL_RGBA, //4,
        tex->internal_format,
        tex->width,
        tex->height,
        0,
        //GL_RGBA,
        tex->format,
        GL_UNSIGNED_BYTE,
        tex->data);

  free(tex->data);
  tex->is_init = true;
}

bool
texture_png_read(Texture* tex)
{
  FILE *f;
  png_byte magic[8];

  f = fopen(tex->filename, "rb");
  if (!f) {
    fprintf (stderr, "error: couldn't open \"%s\"!\n", tex->filename);
    return false;
  }

  /* read magic number */
  fread(magic, 1, sizeof(magic), f);

  /* check for valid magic number */
  if (!png_check_sig(magic, sizeof(magic))) {
    fprintf(stderr, "error: \"%s\" is not a valid PNG image!\n",
          tex->filename);
    fclose(f);
    return false;
  }

  /* create a png read struct */
  png_structp png_ptr = png_create_read_struct(
        PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr) {
    fclose(f);
    return false;
  }

  /* create a png info struct */
  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    fclose(f);
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return false;
  }

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

    return false;
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

  return true;

}

void
texture_fbo_link(Texture* t, GLuint* id)
{
  t->fbo_id = id;
  t->is_fbo = true;
  t->is_init = true;
}

GLuint texture_id_get(Texture* t)
{
  if (t->is_fbo) return *t->fbo_id;
  else return t->id;
}

