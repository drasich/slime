#include "shader.h"
#include "Eet.h"
#include "component/mesh.h"

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

static void
_shader_attribute_location_init(Shader* s, Attribute* att)
{
  GLint att_tmp = gl->glGetAttribLocation(s->program, att->name);
  if (att_tmp == -1) {
    printf("Shader %s, Error in getting attribute '%s' at line %d \n", s->name, att->name, __LINE__);
  }
  else {
     att->location = att_tmp;
  }
}

static void
_shader_uniform_location_init(Shader* s, Uniform* uni)
{
  GLint uni_tmp = gl->glGetUniformLocation(s->program, uni->name);
  if (uni_tmp == -1) {
    printf("Error in getting uniform '%s'\n", uni->name);
  }
  else {
     uni->location = uni_tmp;
  }
}


static void 
shader_attributes_locations_init(Shader* s)
{
  Attribute* att;
  EINA_INARRAY_FOREACH(s->attributes, att) {
    _shader_attribute_location_init(s, att);
  }
}

static void 
shader_uniforms_locations_init(Shader* s)
{
  Uniform* uni;
  EINA_INARRAY_FOREACH(s->uniforms, uni) {
    _shader_uniform_location_init(s, uni);
  }
}



void 
shader_init(Shader* s)
{
  char* vert = stringFromFile(s->vert_path);
  char* frag = stringFromFile(s->frag_path);
  shader_init_string(s, vert, frag);
  shader_attributes_locations_init(s);
  shader_uniforms_locations_init(s);
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
    printf("Error in getting attribute '%s' at line %d\n", att_name, __LINE__);
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
  s->attributes = eina_inarray_new(sizeof(Attribute), 0);
  s->uniforms = eina_inarray_new(sizeof(Uniform), 0);

  return s;
};

void
shader_attribute_add(Shader* s, const char* name, GLint size, GLenum type)
{
  Attribute att;
  att.name = name;
  att.location = 0;
  att.size = size;
  att.type = type;
  eina_inarray_push(s->attributes, &att);
}

void
shader_uniform_add(Shader* s, const char* name)
{
  Uniform uni;
  uni.name = name;
  uni.location = 0;
  uni.type = UNIFORM_BASIC;
  eina_inarray_push(s->uniforms, &uni);
}



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

#include "texture.h"
#include "resource.h"
void 
shader_mesh_draw(Shader* s, struct _MeshComponent* mc)
{
  Mesh* m = mc->mesh;

  Uniform* uni;
  GLuint i = 0;
  EINA_INARRAY_FOREACH(s->uniforms, uni) {
    if (uni->type == UNIFORM_TEXTURE) {
      const char* uniname = uni->name;

      //GLint uni_tex = shader_uniform_location_get(s, uniname);
      GLint uni_tex = uni->location;
      GLint tex_id = -1;
      TextureHandle* th = shader_instance_texture_data_get(mc->shader_instance, uniname);

      //Texture* t = shader_instance_texture_data_get(mc->shader_instance, uniname);
      if (th && th->texture) {
        Texture* t = th->texture;
        texture_init(t);
        tex_id = texture_id_get(t);
      }

      if (uni_tex >= 0 && tex_id >= 0) {
        gl->glUniform1i(uni_tex, i);
        gl->glActiveTexture(GL_TEXTURE0 + i);
        gl->glBindTexture(GL_TEXTURE_2D, tex_id);
        ++i;
      }
    }
  }

  Attribute* att;
  EINA_INARRAY_FOREACH(s->attributes, att) {
    Buffer* buf = mesh_buffer_get(m, att->name);
    if (buf) {
      if (buf->target == GL_ARRAY_BUFFER) {
        gl->glBindBuffer(buf->target, buf->id);
        gl->glEnableVertexAttribArray(att->location);

        gl->glVertexAttribPointer(
              att->location,
              att->size,
              att->type,
              GL_FALSE,
              0,
              0);
      }
    }
  }

  Buffer* buf_indices = mesh_buffer_get(m, "index");
  if (buf_indices) {
    gl->glBindBuffer(buf_indices->target, buf_indices->id);
    gl->glDrawElements(
          GL_TRIANGLES, 
          m->indices_len,
          GL_UNSIGNED_INT,
          0);
    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }
  else {
    gl->glDrawArrays(GL_TRIANGLES,0, m->vertices_len/3);
    gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  gl->glBindBuffer(GL_ARRAY_BUFFER, 0);

  EINA_INARRAY_FOREACH(s->attributes, att) {
    gl->glDisableVertexAttribArray(att->location);
  }

}


GLint
shader_attribute_location_get(Shader* s, const char* name)
{
  Attribute* att;
  EINA_INARRAY_FOREACH(s->attributes, att) {
    if (!strcmp(att->name, name)) 
    return att->location;
  }
  return -1;
}

GLint
shader_uniform_location_get(Shader* s, const char* name)
{
  Uniform* uni = shader_uniform_get(s, name);
  if (uni) return uni->location;
  else return -1;
}

Uniform*
shader_uniform_get(Shader* s, const char* name)
{
  Uniform* uni;
  EINA_INARRAY_FOREACH(s->uniforms, uni) {
    if (!strcmp(uni->name, name)) 
    return uni;
  }
  return NULL;
}



void
shader_matrices_set(Shader* s, Matrix4 mat, const Matrix4 projection)
{
  Matrix4GL matrix;
  Matrix3GL matrix_normal;

  Matrix3 normal_mat;
  mat4_to_mat3(mat, normal_mat);
  mat3_inverse(normal_mat, normal_mat);
  mat3_transpose(normal_mat, normal_mat);
  mat3_to_gl(normal_mat, matrix_normal);

  Matrix4 tm;
  mat4_multiply(projection, mat, tm);
  mat4_transpose(tm, tm);
  mat4_to_gl(tm, matrix);

  GLint uni_matrix = shader_uniform_location_get(s, "matrix");
  if (uni_matrix >= 0)
  gl->glUniformMatrix4fv(uni_matrix, 1, GL_FALSE, matrix);

  uni_matrix = shader_uniform_location_get(s, "normal_matrix");
  if (uni_matrix >= 0)
  gl->glUniformMatrix3fv(uni_matrix, 1, GL_FALSE, matrix_normal);

}


void
shader_uniform_type_add(Shader* s, const char* name, UniformType type)
{
  Uniform uni;
  uni.name = name;
  uni.location = 0;
  uni.type = type;
  eina_inarray_push(s->uniforms, &uni);
}

ShaderInstance*
shader_instance_create(Shader* s)
{
  ShaderInstance* si = calloc(1, sizeof *si);
  si->textures = eina_hash_string_superfast_new(NULL);
  si->uniforms = eina_hash_string_superfast_new(NULL);

  /*
  Uniform* uni;
  EINA_INARRAY_FOREACH(s->uniforms, uni) {
    printf("shader instance create %s, uniname %s \n", s->name, uni->name);
    if (uni->type == UNIFORM_TEXTURE) {
      eina_hash_add(si->textures, uni->name, NULL);
    }
    else {
      eina_hash_add(si->uniforms, uni->name, NULL);
    }
  }
  */

  return si;
}

void
shader_instance_uniform_data_set(ShaderInstance* si, const char* name, void* data)
{
  void* old = eina_hash_set(si->uniforms, name, data);
  //if (!old) printf("--warning, %s: there was no such key '%s' \n", __FUNCTION__, name);
}

void*
shader_instance_uniform_data_get(ShaderInstance* si, const char* name)
{
  return eina_hash_find(si->uniforms, name);
}

void
shader_instance_texture_data_set(ShaderInstance* si, const char* name, void* data)
{
  void* old = eina_hash_set(si->textures, name, data);
  //if (!old) printf("--warning, %s: there was no such key '%s' \n", __FUNCTION__, name);
}

void*
shader_instance_texture_data_get(ShaderInstance* si, const char* name)
{
  return eina_hash_find(si->textures, name);
}

Property*
property_set_shader_instance()
{
  Property* ps = create_property_set();
  PROPERTY_SET_TYPE(ps, ShaderInstance);

  //Property* ps_tex = property_set_texture();
  Property* ps_tex = property_set_resource_handle();

  PROPERTY_HASH_ADD(ps, ShaderInstance, textures, ps_tex);
  return ps;
}

