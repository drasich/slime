#include "object.h"
#include "gl.h"
#include "read.h"
#include "camera.h"

void
object_init(Object* o)
{
  quat_set_identity(&o->Orientation);
  //o->name = eina_stringshare_add("dance");
}

void
object_destroy(Object* o)
{
  //TODO clean armature
  //TODO clean components
}

void
object_draw(Object* o, Matrix4 world, struct _CCamera* cam)
{
  Eina_List* l;
  Component* c;

  EINA_LIST_FOREACH(o->components, l, c) {
    if (c->funcs->draw)
    c->funcs->draw(c, world, cam);
  }
}

void
object_draw_edit(Object* o, Matrix4 world, struct _CCamera* cam)
{
  Matrix4* projection = &cam->projection;

  Eina_List* l;
  Component* c;

  EINA_LIST_FOREACH(o->components, l, c) {
    if (c->funcs->draw)
    c->funcs->draw(c, world, cam);
    if (c->funcs->draw_edit)
    c->funcs->draw_edit(c, world, cam);
  }

}

void
object_draw_edit_component(Object* o, Matrix4 world, struct _CCamera* cam, const char* name)
{
  Matrix4* projection = &cam->projection;

  Eina_List* l;
  Component* c;

  EINA_LIST_FOREACH(o->components, l, c) {
    if (strcmp(c->name, name)) continue;

    if (c->funcs->draw)
    c->funcs->draw(c, world, cam);
    if (c->funcs->draw_edit)
    c->funcs->draw_edit(c, world, cam);
  }

}



void
object_compute_matrix(Object* o, Matrix4 mat)
{
  o->Orientation = quat_angles_deg(o->angles.Y, o->angles.X, o->angles.Z);
  Matrix4 mt, mr;
  mat4_set_translation(mt, o->Position);
  mat4_set_rotation_quat(mr, o->Orientation);
  mat4_multiply(mt, mr, mat);
}

void
object_compute_matrix_with_angles(Object* o, Matrix4 mat)
{
  o->Orientation = quat_angles_deg(o->angles.Y, o->angles.X, o->angles.Z);
  object_compute_matrix_with_quat(o, mat);
   /*
  Matrix4 mt, mr;
  mat4_set_translation(mt, o->Position);
  mat4_set_rotation_quat(mr, o->Orientation);
  mat4_multiply(mt, mr, mat);
  */
}


void
object_compute_matrix_with_quat(Object* o, Matrix4 mat)
{
  Matrix4 mt, mr;
  mat4_set_translation(mt, o->Position);
  mat4_set_rotation_quat(mr, o->Orientation);
  mat4_multiply(mt, mr, mat);
}

void
object_compute_matrix_with_pos_quat(Object* o, Matrix4 mat, Vec3 v, Quat q)
{
  Matrix4 mt, mr;
  mat4_set_translation(mt, v);
  mat4_set_rotation_quat(mr, q);
  mat4_multiply(mt, mr, mat);
}



static void
_animation_update(Object* o, float dt)
{
  Animation* anim = o->animation;
  if (anim == NULL) return;

  if (anim->status == PLAY) {
    anim->time += dt;
    if (anim->time*30 >= anim->action_current->frame_end) {
      if (anim->mode == LOOP)
      anim->time = (anim->time*30.0f - anim->action_current->frame_end)/30.0f;
      else if (anim->mode == ONCE) {
        anim->time = anim->action_current->frame_end/30.0f;
        anim->status = STOP;
      }
    }

    object_set_pose(o, anim->current, anim->time);
  }
}

void
object_update(Object* o)
{
  o->Orientation = quat_angles_deg(o->angles.Y, o->angles.X, o->angles.Z);
  mat4_pos_ori(o->Position, o->Orientation, o->matrix);

  if (o->animation != NULL) {
    _animation_update(o, 0.007f);
    static float stime = 0;
    static bool played =false;
    stime += 0.05f;
    if (stime > 15 && !played) {
      animation_stop(o);
      played = true;
    }
  }

  Eina_List* l;
  Component* c;

  EINA_LIST_FOREACH(o->components, l, c) {
    if (c->funcs->update)
    c->funcs->update(c, 0.01f);
  }
  
}

void object_add_component_armature(Object* o, Armature* a)
{
  o->armature = a;
}


Object* create_object()
{
  Object* o = calloc(1, sizeof(Object));
  o->name = eina_stringshare_add("empty");
  object_init(o);
  eina_value_setup(&o->data_position, EINA_VALUE_TYPE_DOUBLE);
  eina_value_set(&o->data_position, 777);

  Component *oc =  create_component(&object_desc);
  oc->data = o;
  object_add_component(o, oc);
  
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
      //TODO add this to component
      Component* comp = create_component(&mesh_desc);
      object_add_component(o, comp);
      Mesh* mesh = comp->data;
      mesh_read_file(mesh,f);
      mesh->name = path;
      //Mesh* mesh = create_mesh_file(f);
      o->mesh = mesh;


      //TODO box component
      /*
      o->line = create_line();
      line_set_use_depth(o->line, true);
      line_add_box(o->line, mesh->box, vec4(0,1,0,0.2));
      */
    }
    else if (!strcmp(type, "armature")){
      Armature* armature = create_armature_file(f);
      object_add_component_armature(o, armature);
      o->animation = calloc(1, sizeof *o->animation);
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
  printf("remove this function for now!!!!!!!!!!!!\n");
  o->Orientation = q;
}

void
_object_update_mesh_vertex(Object* o)
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

  //mesh_resend(mesh);
  mesh_resend_no_indices(mesh);
}

void 
object_set_pose(Object* o, char* action_name, float time)
{
  if (o->mesh == NULL || o->armature == NULL) return;

  armature_set_pose(o->armature, action_name, time);
  _object_update_mesh_vertex(o);
}


void
object_play_animation(Object* o, char* action_name)
{
}

void
object_add_component(Object* o, Component* c)
{
  o->components = eina_list_append(o->components, c);
  c->object = o;
}

void
object_remove_component(Object* o, Component* c)
{
  o->components = eina_list_remove(o->components, c);
}

void*
object_component_get(const Object* o, const char* name)
{
  Eina_List* l;
  Component* c;

  EINA_LIST_FOREACH(o->components, l, c) {
    if (!strcmp(c->name, name))
    return c->data;
  }

  return NULL;

}

static PropertySet*
_vec3_array()
{
  PropertySet* ps = create_property_set();
  PROPERTY_SET_TYPE(ps, Vec3);

  ps->hint = HORIZONTAL;

  ADD_PROP_NAME(ps, Vec3, X, EET_T_DOUBLE, "x");
  ADD_PROP_NAME(ps, Vec3, Y, EET_T_DOUBLE, "y");
  ADD_PROP_NAME(ps, Vec3, Z, EET_T_DOUBLE, "z");

  return ps;
}

static PropertySet* 
_object_properties()
{
  PropertySet* ps = create_property_set();
  PROPERTY_SET_TYPE(ps, Object);

  PropertySet *vec3 = _vec3_array();
  //TODO clean the arrays

  ADD_PROP(ps, Object, name, EET_T_STRING);

  ADD_PROP_STRUCT_NESTED(ps, Object, Position, vec3);

  PropertySet *an = _vec3_array();
  ADD_PROP_STRUCT_NESTED(ps, Object, angles, an);

  return ps;
}


ComponentDesc object_desc = {
  "object",
  NULL,
  _object_properties
};

void object_descriptor_init()
{
  Eet_Data_Descriptor_Class eddc;

  EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Object);
  object_descriptor = eet_data_descriptor_file_new(&eddc);

#define ADD_BASIC(member, eet_type) \
  EET_DATA_DESCRIPTOR_ADD_BASIC     \
  (object_descriptor, Object, # member, member, eet_type)
  ADD_BASIC(name, EET_T_STRING);
#undef ADD_BASIC

}

void
object_descriptor_shutdown()
{
  eet_data_descriptor_free(object_descriptor);
}

