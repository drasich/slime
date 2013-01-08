#include "object.h"
#include "gl.h"
#include "read.h"

void
object_init(Object* o)
{
  mat4_set_identity(o->Matrix);
  quat_set_identity(&o->Orientation);
  o->name = "dance";
}

void
object_destroy(Object* o)
{
  //TODO
}

void
object_draw(Object* o, int w, int h)
{
  Matrix4 mt, mr, mat;
  mat4_set_translation(mt, o->Position);
  mat4_set_rotation_quat(mr, o->Orientation);
  mat4_multiply(mt, mr, mat);

  //TODO put the projection matrix in the draw? or directly in scene draw (or even view)
  float hw = w*0.5f;
  float aspect = (float)w/(float)h;
  Matrix4 projection;
  mat4_set_frustum(projection, -aspect,aspect,-1,1,1,1000.0f);

  if (o->mesh != NULL) {
    mesh_set_matrices(o->mesh, mat, projection);
    mesh_draw(o->mesh);
  }
}

void
object_update(Object* o)
{
  //TODO remove this code and this function anyway
  static float test = 0;
  test += 0.05f;
  Vec3 axis = {0,1,0};
  o->Orientation = quat_angle_axis(test, axis);
}

/*
void
object_add_component(Component* c)
{
  //TODO
}
*/

void object_add_component_mesh(Object* o, Mesh* m)
{
  o->mesh = m;
}

void object_add_component_armature(Object* o, Armature* a)
{
  o->armature = a;
}


Object* create_object()
{
  Object* o = calloc(1, sizeof(Object));
  object_init(o);
  return o;
}

Object* create_object_file(const char* path)
{
  Object* o = create_object();
  FILE *f;
  f = fopen(path, "rb");
  fseek(f, 0, SEEK_SET);

  int ob_nb = read_uint16(f);
  int i;
  for (i = 0; i <ob_nb; ++i) {

    char* type = read_name(f);

    if (!strcmp(type, "mesh")){
      Mesh* mesh = create_mesh_file(f);
      object_add_component_mesh(o, mesh);
    }
    else if (!strcmp(type, "armature")){
      Armature* armature = create_armature_file(f);
      object_add_component_armature(o, armature);
    }
    free(type);
  }


  return o;
}


void
object_set_position(Object* o, Vec3 v)
{
  o->Position = v;
}

void 
object_set_orientation(Object* o, Quat q)
{
  o->Orientation = q;
}
