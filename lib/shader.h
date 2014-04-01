#ifndef __shader__
#define __shader__
#include "gl.h"
#include "stdbool.h"
#include "matrix.h"
#include "property.h"

typedef struct _Uniform Uniform;
typedef struct _Attribute Attribute;

struct _Attribute{
  const char* name;
  GLuint location;
  GLint size;
  GLenum type;
};


/*
typedef enum {
  UNIFORM_BASIC, //TODO remove
  UNIFORM_TEXTURE,
  UNIFORM_VEC3,
  UNIFORM_VEC4,
  UNIFORM_MATRIX3,
  UNIFORM_MATRIX4,
} UniformType;
*/

typedef enum _UniformType UniformType;
enum _UniformType {
  UNIFORM_UNKNOWN,
  UNIFORM_TEXTURE,
  UNIFORM_INT,
  UNIFORM_FLOAT,
  UNIFORM_VEC2,
  UNIFORM_VEC3,
  UNIFORM_VEC4,
  UNIFORM_MAT3,
  UNIFORM_MAT4
};


struct _Uniform{
  const char* name;
  GLuint location;
  UniformType type;
  bool visible;
};

typedef enum _ShaderState ShaderState;
enum _ShaderState {
  CREATED,
  USABLE,
  RESET
};


typedef struct _Shader Shader;
//typedef char GLchar; // was not in efl before... should be fine now


struct _Shader
{
  const char* name;
  GLuint vert_shader;
  GLuint frag_shader;
  GLuint program;
  const char* vert_path;
  const char* frag_path;
  ShaderState state;

  Eina_Inarray* attributes;
  Eina_Inarray* uniforms;
  //TODO uniforms texture
};

typedef struct _ShaderInstance ShaderInstance;
struct _ShaderInstance
{
  Eina_Hash* textures;
  Eina_Hash* uniforms;
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
struct _MeshComponent;
void shader_mesh_draw(Shader* s, struct _MeshComponent* mc);
struct _Mesh;
void shader_mesh_nocomp_draw(Shader* s, ShaderInstance* si, struct _Mesh* m);

void shader_destroy(Shader* s);

Shader* create_shader(const char* name, const char* vert_path, const char* frag_path);
Shader* shader_new();
void shader_read_txt(Shader* s, const char* filename);
Eina_Bool shader_write(const Shader* s);
Shader* shader_read(const char* filename);

void shader_descriptor_init(void);
GLint shader_attribute_location_get(Shader* shader, const char* name);
void shader_matrices_set(Shader* mesh, Matrix4 mat, const Matrix4 projection);

void shader_uniform_add(Shader* s, const char* name, bool visible);
void shader_uniform_type_add(Shader* s, const char* name, UniformType type, bool visible);
Uniform* shader_uniform_get(Shader* s, const char* name);
GLint shader_uniform_location_get(Shader* s, const char* name);

void shader_attribute_add(Shader* s, const char* name, GLint size, GLenum type);

ShaderInstance* shader_instance_create(Shader* s);
void shader_instance_uniform_data_set(ShaderInstance* si, const char* name, void* data);
void* shader_instance_uniform_data_get(ShaderInstance* si, const char* name);
void shader_instance_texture_data_set(ShaderInstance* si, const char* name, void* data);
void* shader_instance_texture_data_get(ShaderInstance* si, const char* name);

Property* property_set_shader_instance();

typedef struct _UniformValue UniformValue;
struct _UniformValue
{
  union {
    int i;
    float f;
    Vec2 vec2;
    Vec3 vec3;
    Vec4 vec4;
    Matrix3GL mat3;
    Matrix4GL mat4;
  } value;

  UniformType type;
};
Property* property_set_uniform();
void shader_instance_print(ShaderInstance* si);
void shader_instance_init(ShaderInstance* si);

Eina_Bool uniform_send(const Eina_Hash *hash, const void *key, void *data, void *fdata);
void shader_instance_update(ShaderInstance* si, Shader* s);

void shader_reload(Shader* s);

#endif
