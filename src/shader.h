#ifndef __shader__
#define __shader__
#include "gl.h"
#include "stdbool.h"

typedef struct _Shader Shader;
//typedef char GLchar; // currently not in 1.7.3 and 1.7.4 but is in svn.

struct _Shader
{
  const char* name;
  GLuint vert_shader;
  GLuint frag_shader;
  GLuint program;
  const char* vert_path;
  const char* frag_path;
  bool is_init;

  //TODO make it has_attribute("vertex");
  Eina_Bool has_vertex;
  Eina_Bool has_normal;
  Eina_Bool has_texcoord;

  Eina_Bool has_uniform_normal_matrix;

  //TODO here list, hash, something of uniform and attributes
  GLint uniform_matrix;
  GLint uniform_normal_matrix;
  GLint uniform_wireframe;
  GLint uniform_texture;
  GLint uniform_resolution;

  //TODO remove and make material
  GLint uniform_texture_all;
};


//void load_shader(Evas_Object *gl);
char* stringFromFile(const char* path);
void shader_init(Shader* shader);

void shader_init_string(
      Shader* s, 
      const char* vert, 
      const char* frag);

void shader_init_attribute(Shader* s, char* att_name, GLuint* att);
void shader_init_uniform(Shader* s, char* uni_name, GLint* uni);

void shader_use(Shader* s);

void shader_destroy(Shader* s);

Shader* create_shader(const char* name, const char* vert_path, const char* frag_path);
Eina_Bool shader_write(const Shader* s);
Shader* shader_read(const char* filename);

void shader_descriptor_init(void);

#endif
