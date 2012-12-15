#ifndef __mesh__
#define __mesh__
#include "shader.h"
#include "matrix.h"
typedef struct _Mesh Mesh;

struct _Mesh
{
  GLuint buffer_vertices;
  GLuint buffer_indices;
  GLuint buffer_normals;
  GLuint id_texture;
  GLuint buffer_texcoords;

  //*
  Eina_Inarray*  vertices;
  Eina_Inarray*  indices;
  Eina_Inarray*  normals;
  Eina_Inarray*  uvs;
  //*/
  /*
  GLfloat*  vertices;
  GLuint*  indices;
  GLfloat*  normals;
  GLfloat*  uvs;
  */

  Shader* shader;
  //matrices
  Matrix4GL matrix;
  Matrix4GL matrix_normal;
};


void load_model(Evas_Object *gl);
void readModel(char* path, Mesh* mesh);
void initModel(Mesh* mesh, Evas_GL_API *gl);

#endif
