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
  /*
  static float test = 0;
  test += 0.05f;
  Vec3 axis = {0,1,0};
  o->Orientation = quat_angle_axis(test, axis);
  */
  //Vec3 axis = {1,0,0};
  //o->Orientation = quat_angle_axis(90, axis);

   static float ttt = 0;
   object_set_pose(o, "walkquat", ttt);
   ttt += 0.01f;
   if (ttt > 20.0f/30.0f) ttt = 0;
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
  printf("object nb '%d'\n",ob_nb);
  int i;
  for (i = 0; i <ob_nb; ++i) {

    char* type = read_name(f);
    printf("object of type '%s'\n",type);

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

void 
object_set_pose(Object* o, char* action_name, float time)
{
  if (o->mesh == NULL || o->armature == NULL) return;

  Action* action = armature_find_action(o->armature, action_name);

  if (action == NULL) return;
  
  int frame = time *30;
  if (frame < action->frame_start) frame = action->frame_start;
  if (frame > action->frame_end) frame = action->frame_end;

  Eina_List *l;
  Curve *curve;
  EINA_LIST_FOREACH(action->curves, l, curve) {
    Bone* bone = curve->bone;
    Frame *start, *end;
    curve_get_frames(curve, frame, &start, &end);

    float ratio = 0;
    if (start != end)
    ratio = (frame - start->time) / (end->time - start->time);

    if (curve->type == POSITION) {
      //bone->position = f->vec3;
      //printf("new pos : %f, %f, %f\n", bone->position.X, bone->position.Y, bone->position.Z);
      Vec3 v1 = vec3_mul(start->vec3, 1-ratio);
      Vec3 v2 = vec3_mul(end->vec3, ratio);
      bone->position = vec3_add(v1, v2);

    } else if (curve->type == QUATERNION) {
      //bone->rotation = f->quat;
      //Quat qr = bone->rotation;
      //printf("set pose rotation %f %f %f %f\n", qr.X, qr.Y, qr.Z, qr.W);
      bone->rotation = quat_slerp(start->quat, end->quat, ratio);

    } else if (curve->type == EULER) {
      /*
      Vec3 euler = f->vec3;
      printf("euler %f %f %f\n", euler.X, euler.Y, euler.Z);
      Vec3 axisz = {0,0,1};
      Quat qz = quat_angle_axis(euler.Z, axisz);
      Vec3 axisy = {0,1,0};
      Quat qy = quat_angle_axis(euler.Y, axisy);
      Vec3 axisx = {1,0,0};
      Quat qx = quat_angle_axis(euler.X, axisx);
      Quat q = quat_mul( quat_mul (qz, qy), qx);
      //Quat q = quat_mul( quat_mul (qx, qy), qz);

      //bone->rotation = q;
      */
    }

  }

 // object_update_mesh_from_armature(o);
  object_update_mesh_vertex(o);
}


void
object_update_mesh_vertex(Object* o)
{
  //TODO add the rotation/position of the armature in the exporter

  if (o->mesh == NULL || o->armature == NULL) return;
  Mesh* mesh = o->mesh;

  VertexInfo *vi;
  int i = 0;
  EINA_INARRAY_FOREACH(mesh->vertices_base, vi) {
    Weight* w;
    Vec3 translation = vec3_zero();
    Quat rotation = quat_identity();
    EINA_INARRAY_FOREACH(vi->weights, w) {
      VertexGroup* vg = eina_array_data_get(mesh->vertexgroups, w->index);
      Bone* bone = armature_find_bone(o->armature, vg->name);

      Vec3 bn = quat_rotate_vec3(bone->rotation_base, bone->position);

      Vec3 t = vec3_mul(bn, w->weight);
      Quat q = quat_slerp(quat_identity(), bone->rotation, w->weight);
      rotation = quat_mul(rotation, q);

      Vec3 bb = bone->position_base;
      Vec3 yep = vec3_sub(vi->position, bb);
      yep = quat_rotate_vec3(q, yep);
      yep = vec3_add(yep, bb);
      yep = vec3_sub(yep, vi->position);

      t = vec3_add(t,yep);
      translation = vec3_add(translation, t);
    }

    Vec3 newpos = vec3_add(vi->position,translation);
    Vec3 newnor = quat_rotate_vec3(rotation, vi->normal);

    mesh->vertices[i*3] = newpos.X;
    mesh->vertices[i*3+1] = newpos.Y;
    mesh->vertices[i*3+2] = newpos.Z;

    mesh->normals[i*3] = newnor.X;
    mesh->normals[i*3+1] = newnor.Y;
    mesh->normals[i*3+2] = newnor.Z;
    ++i;

  }

  mesh_resend(mesh);
}

void
object_play_animation(Object* o, char* action_name)
{
}
