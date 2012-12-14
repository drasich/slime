#ifndef __shader__
#define __shader__
//#include "GL/gl.h"
#include <Elementary.h>

typedef struct _Shader Shader;

struct _Shader
{
  GLuint vert_shader;
  GLuint frag_shader;
  GLuint program;

  GLuint attribute_vertex;
  GLuint attribute_normal;

  GLint uniform_matrix;
  GLint uniform_normal_matrix;

  GLint uniform_texture;
  GLuint attribute_texcoord;
};


void load_shader(Evas_Object *gl);
char* stringFromFile(char* path);
void shader_init(Shader* shader, Evas_GL_API* gl, char* vert_path, char* frag_path);

void shader_init_string(
      Shader* s, 
      Evas_GL_API* gl, 
      const char* vert, 
      const char* frag);

void shader_init_attribute(Shader* s, Evas_GL_API* gl);
void shader_init_uniform(Shader* s, Evas_GL_API* gl);

#endif
