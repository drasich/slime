#ifndef __shader__
#define __shader__
#include "gl.h"

typedef struct _Shader Shader;
typedef char GLchar; // currently not in 1.7.3 and 1.7.4 but is in svn.

struct _Shader
{
  GLuint vert_shader;
  GLuint frag_shader;
  GLuint program;
};


//void load_shader(Evas_Object *gl);
char* stringFromFile(char* path);
void shader_init(Shader* shader, char* vert_path, char* frag_path);

void shader_init_string(
      Shader* s, 
      const char* vert, 
      const char* frag);

void shader_init_attribute(Shader* s, char* att_name, GLuint* att);
void shader_init_uniform(Shader* s, char* uni_name, GLint* uni);

void shader_use(Shader* s);

void shader_destroy(Shader* s);
#endif
