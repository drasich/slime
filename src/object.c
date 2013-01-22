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
  //*
  static float test = 0;
  test += 0.05f;
  Vec3 axis = {0,1,0};
  o->Orientation = quat_angle_axis(test, axis);
  //*/
  //Vec3 axis = {1,0,0};
  //o->Orientation = quat_angle_axis(90, axis);
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
object_set_pose(Object* o, char* action_name)
{
  if (o->mesh == NULL || o->armature == NULL) return;

  Action* action = armature_find_action(o->armature, action_name);

  if (action == NULL) return;
  
  printf("found action\n");

  int frame = 10;

  Eina_List *l;
  Curve *curve;
  EINA_LIST_FOREACH(action->curves, l, curve) {
    Bone* bone = curve->bone;
    printf("bone name : %s \n", bone->name);
    Frame* f = curve_find_frame(curve,0);
    if (curve->type == POSITION) {

    } else if (curve->type == QUATERNION) {
      printf("quat\n");
      bone->rotation = f->quat;

    } else if (curve->type == EULER) {
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

      //TODO we have the bone rotation for this frame, now we have to
      // modify the vertices associated to this bone
      // in the next function we update this
      
      bone->rotation = q;
    }

  }

  object_update_mesh_from_armature(o);
}

void
object_update_mesh_from_armature(Object* o)
{
  if (o->mesh == NULL || o->armature == NULL) return;
  Mesh* mesh = o->mesh;

  Eina_List *l;
  Bone *bone;
  EINA_LIST_FOREACH(o->armature->bones, l, bone) {
    char* bone_name = bone->name;
    printf("update bone name : %s\n", bone_name);
    Quat q = bone->rotation_base;
    printf("  rotation base : %f, %f, %f, %f\n", q.X, q.Y, q.Z, q.W);
    q = bone->rotation;
    printf("  rotation : %f, %f, %f, %f\n", q.X, q.Y, q.Z, q.W);
    //TODO optimize/change this for loop
    VertexGroup* vg = mesh_find_vertexgroup(mesh, bone_name);
    Weight* w;
    int i =0;
    EINA_INARRAY_FOREACH(vg->weights, w){
      if (w->weight < 0.9f) continue;
      printf("index, weight : %d, %f nb %d\n", w->index, w->weight, i);
      i++;
      Vec3* v = eina_inarray_nth(mesh->vertices_base, w->index);
      //printf("vertex to change : %f, %f, %f\n", v->X, v->Y, v->Z);
      Quat qdiff = quat_slerp(bone->rotation_base, bone->rotation, w->weight);
      qdiff = quat_between_quat(bone->rotation_base, qdiff);
      //printf("  qdiff rotation : %f, %f, %f, %f\n", qdiff.X, qdiff.Y, qdiff.Z, qdiff.W);
      Vec3 nv = vec3_sub(*v, bone->position_base);
      nv  = quat_rotate_vec3(qdiff, nv);
      nv = vec3_add(nv, bone->position_base);

      mesh->vertices[w->index*3] = nv.X;
      mesh->vertices[w->index*3+1] = nv.Y;
      mesh->vertices[w->index*3+2] = nv.Z;
    }
  }

  mesh_init(mesh);

}

