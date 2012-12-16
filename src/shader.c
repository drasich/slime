#include "shader.h"

char* 
stringFromFile(char* path)
{
  FILE* f;
  long length;
  char* buf;

  f = fopen(path, "rb");
  if (!f) return NULL;

  fseek(f, 0, SEEK_END);
  length = ftell(f);
  buf = (char*)malloc(length+1);

  fseek(f, 0, SEEK_SET);
  fread(buf, length, 1, f);
  fclose(f);
  buf[length] = 0;

  return buf;
}

void 
shader_init(Shader* shader, Evas_GL_API* gl, char* vert_path, char* frag_path)
{
  char* vert = stringFromFile(vert_path);
  char* frag = stringFromFile(frag_path);
  shader_init_string(shader, gl, vert, frag);
  free(vert);
  free(frag);
}

void
shader_init_string(Shader* s, Evas_GL_API* gl, const char* vert, const char* frag)
{
  //TODO factorize this by creating a function that get the shader id
  s->vert_shader = gl->glCreateShader(GL_VERTEX_SHADER);
  if (s->vert_shader == 0) 
    printf("there was en error creating the vertex shader\n");

  s->frag_shader = gl->glCreateShader(GL_FRAGMENT_SHADER);
  if (s->frag_shader == 0) 
    printf("there was en error creating the fragment shader\n");

  gl->glShaderSource(s->vert_shader, 1, &vert, 0);
  gl->glCompileShader(s->vert_shader);

  GLint status;
  GLint info_length;
  GLchar* message;

  gl->glGetShaderiv(s->vert_shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    printf("There was an error compiling the vertex shader\n");
    gl->glGetShaderiv(s->vert_shader, GL_INFO_LOG_LENGTH, &info_length);
    message = malloc(info_length);
    gl->glGetShaderInfoLog(s->vert_shader, info_length, 0, message);
    printf("%s\n",message);
    free(message);
  }

  gl->glShaderSource(s->frag_shader, 1, &frag, 0);
  gl->glCompileShader(s->frag_shader);

  gl->glGetShaderiv(s->frag_shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    printf("There was an error compiling the fragment shader\n");
    gl->glGetShaderiv(s->vert_shader, GL_INFO_LOG_LENGTH, &info_length);
    message = malloc(info_length);
    gl->glGetShaderInfoLog(s->frag_shader, info_length, 0, message);
    printf("message : %s\n",message);
    free(message);
  }

  s->program = gl->glCreateProgram();
  gl->glAttachShader(s->program, s->vert_shader);
  gl->glAttachShader(s->program, s->frag_shader);
  gl->glLinkProgram(s->program);

  gl->glGetProgramiv(s->program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    printf("There was an error in linking the program\n");
    gl->glGetProgramiv(s->program, GL_INFO_LOG_LENGTH, &info_length);
    message = malloc(info_length);
    gl->glGetProgramInfoLog(s->program, info_length, 0, message);
    printf("%s\n",message);
    free(message);
  }

  shader_init_attributes(s,gl);
  shader_init_uniforms(s,gl);
}

void
shader_init_attribute(Shader* s, char* att_name, GLuint* att, Evas_GL_API* gl)
{
  GLint att_tmp = gl->glGetAttribLocation(s->program, att_name);
  if (att_tmp == -1) {
    printf("Error in getting attribute %s \n", att_name);
  }
  else {
     *att = att_tmp;
    printf("attribute is now %d \n", *att);
  }
}

void 
shader_init_attributes(Shader* s, Evas_GL_API* gl)
{
  shader_init_attribute(s, "vertex", &s->attribute_vertex, gl);
  shader_init_attribute(s, "normal", &s->attribute_normal, gl);
  //shader_init_attribute(s, "texcoord", &s->attribute_texcoord, gl);
}

void 
shader_init_uniform(Shader* s, char* uni_name, GLint* uni, Evas_GL_API* gl)
{
  *uni = gl->glGetUniformLocation(s->program, uni_name);
  if (*uni == -1) 
    printf("Error in getting uniform %s \n", uni_name);
}

void 
shader_init_uniforms(Shader* s, Evas_GL_API* gl)
{
  //shader_init_uniform(s, "test", &s->uniform_test, gl);
  shader_init_uniform(s, "matrix", &s->uniform_matrix, gl);
  shader_init_uniform(s, "normal_matrix", &s->uniform_normal_matrix, gl);
}

void
shader_use(Shader* s, Evas_GL_API* gl)
{
  gl->glUseProgram(s->program);
}

