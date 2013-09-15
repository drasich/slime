#include "shader.h"
#include "Eet.h"

char* 
stringFromFile(const char* path)
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
shader_init(Shader* s)
{
  char* vert = stringFromFile(s->vert_path);
  char* frag = stringFromFile(s->frag_path);
  shader_init_string(s, vert, frag);
  free(vert);
  free(frag);
  s->is_init = true;
}

void
shader_init_string(Shader* s, const char* vert, const char* frag)
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
    gl->glGetShaderiv(s->frag_shader, GL_INFO_LOG_LENGTH, &info_length);
    message = malloc(info_length);
    gl->glGetShaderInfoLog(s->frag_shader, info_length, 0, message);
    printf("message : %s\n", message);
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

}

void
shader_init_attribute(Shader* s, char* att_name, GLuint* att)
{
  GLint att_tmp = gl->glGetAttribLocation(s->program, att_name);
  if (att_tmp == -1) {
    printf("Error in getting attribute %s \n", att_name);
  }
  else {
     *att = att_tmp;
  }
}

void 
shader_init_uniform(Shader* s, char* uni_name, GLint* uni)
{
  *uni = gl->glGetUniformLocation(s->program, uni_name);
  if (*uni == -1) 
    printf("Error in getting uniform %s \n", uni_name);
}

void
shader_use(Shader* s)
{
  if (!s->is_init) {
    shader_init(s);
  }
  gl->glUseProgram(s->program);
}

void
shader_destroy(Shader* s)
{
  gl->glDeleteShader(s->vert_shader);
  gl->glDeleteShader(s->frag_shader);
  gl->glDeleteProgram(s->program);
}

Shader* 
create_shader(const char* name, const char* vert_path, const char* frag_path)
{
  Shader* s = calloc(1,sizeof(Shader));
  s->vert_path = vert_path;
  s->frag_path = frag_path;
  s->name = name;
  return s;
};


static Eet_Data_Descriptor *_shader_descriptor;
static const char SHADER_FILE_ENTRY[] = "shader";

void
shader_descriptor_init(void)
{
  Eet_Data_Descriptor_Class eddc;

  EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Shader);
  _shader_descriptor = eet_data_descriptor_stream_new(&eddc);

#define ADD_BASIC(member, eet_type) \
  EET_DATA_DESCRIPTOR_ADD_BASIC             \
  (_shader_descriptor, Shader, # member, member, eet_type)

  ADD_BASIC(name, EET_T_STRING);
  ADD_BASIC(vert_path, EET_T_STRING);
  ADD_BASIC(frag_path, EET_T_STRING);
  ADD_BASIC(has_vertex, EET_T_UCHAR);
  ADD_BASIC(has_normal, EET_T_UCHAR);
  ADD_BASIC(has_texcoord, EET_T_UCHAR);
  ADD_BASIC(has_uniform_normal_matrix, EET_T_UCHAR);

#undef ADD_BASIC
}


Eina_Bool
shader_write(const Shader* s)
{
  const char* filename = s->name;
  printf("shader filename %s\n", filename);

  Eina_Bool ret;
  Eet_File *ef = eet_open(filename, EET_FILE_MODE_WRITE);
  if (!ef) {
    fprintf(stderr, "error reading file %s \n", filename);
    return EINA_FALSE;
  }

  ret = eet_data_write(ef, _shader_descriptor, SHADER_FILE_ENTRY, s, EINA_TRUE);
  eet_close(ef);
  if (ret) {
    printf("return value for save looks ok \n");
  }
  else
    printf("return value for save NOT OK \n");

  return ret;
}

static void 
_output(void *data, const char *string)
{
  printf("%s", string);
}


Shader*
shader_read(const char* filename)
{
  Shader* s;

  Eet_File *ef = eet_open(filename, EET_FILE_MODE_READ);
  if (!ef) {
    fprintf(stderr, "error reading file %s \n", filename);
    return NULL;
  }

  s = eet_data_read(ef, _shader_descriptor, SHADER_FILE_ENTRY);
  printf("shader read data dump\n");
  eet_data_dump(ef, SHADER_FILE_ENTRY, _output, NULL);
  printf("shader read data dump end\n");
  eet_close(ef);

  if (s) {
    //printf("Shader %s \n", s->name);
    printf("Shader %s \n", s->vert_path);
    printf("Shader %s \n", s->frag_path);
  }
  else
  printf("s is null\n");
 
  return s;  
}


