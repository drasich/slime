#ifndef __shader__
#define __shader__
#include "gl.h"
#include "stdbool.h"
#include "matrix.h"

typedef struct _Attribute Uniform;
typedef struct _Attribute Attribute;

struct _Attribute{
  const char* name;
  GLuint location;
};


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

  Eina_Inarray* attributes;
  Eina_Inarray* uniforms;
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
GLint shader_attribute_location_get(Shader* shader, const char* name);
void shader_matrices_set(Shader* mesh, Matrix4 mat, Matrix4 projection);

#endif
