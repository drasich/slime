#include "object.h"
#include "gl.h"
#include "read.h"
#include "camera.h"

void
object_init(Object* o)
{
  quat_set_identity(&o->Orientation);
  o->scale.X = 1;
  o->scale.Y = 1;
  o->scale.Z = 1;
  //o->name = eina_stringshare_add("dance");
}

void
object_destroy(Object* o)
{
  //TODO clean armature
  //TODO clean components
}

void
object_draw(
      Object* o,
      const Matrix4 cam_inv,
      const Matrix4 projection,
      const Matrix4 parent)
{
  Matrix4 world;
  object_compute_matrix(o, world);

  mat4_multiply(parent, world, world);

  Matrix4 mo;
  mat4_multiply(cam_inv, world, mo);

  Eina_List* l;
  Component* c;

  EINA_LIST_FOREACH(o->components, l, c) {
    if (c->funcs->draw)
    c->funcs->draw(c, mo, projection);
  }

  Object* child;
  Eina_List* lc;
  EINA_LIST_FOREACH(o->children, lc, child) {
    object_draw_edit(child, cam_inv, projection, world);
  }
}

void 
object_draw_edit(
      Object* o,
      const Matrix4 cam_inv,
      const Matrix4 projection,
      const Matrix4 parent)
{
  Matrix4 world;
  object_compute_matrix(o, world);

  mat4_multiply(parent, world, world);

  Matrix4 mo;
  mat4_multiply(cam_inv, world, mo);

  Eina_List* l;
  Component* c;

  EINA_LIST_FOREACH(o->components, l, c) {
    if (c->funcs->draw)
    c->funcs->draw(c, mo, projection);
    if (c->funcs->draw_edit)
    c->funcs->draw_edit(c, mo, projection);
  }

  /*
  Object* child;
  Eina_List* lc;
  EINA_LIST_FOREACH(o->children, lc, child) {
    object_draw_edit(child, cam_inv, projection, world);
  }
  */
}

void
object_draw_edit_component(
      Object* o,
      const Matrix4 cam_inv,
      const Matrix4 projection,
      const Matrix4 parent,
      const char* name)
{
  Matrix4 world;
  object_compute_matrix(o, world);

  mat4_multiply(parent, world, world);

  Matrix4 mo;
  mat4_multiply(cam_inv, world, mo);

  Eina_List* l;
  Component* c;

  EINA_LIST_FOREACH(o->components, l, c) {
    if (strcmp(c->name, name)) continue;

    if (c->funcs->draw)
    c->funcs->draw(c, mo, projection);
    if (c->funcs->draw_edit)
    c->funcs->draw_edit(c, mo, projection);
  }

  /*
  Object* child;
  Eina_List* lc;
  EINA_LIST_FOREACH(o->children, lc, child) {
    object_draw_edit(child, cam_inv, projection, world);
  }
  */

}



void
object_compute_matrix(Object* o, Matrix4 mat)
{
  if (o->orientation_type == ORIENTATION_EULER)
  o->Orientation = quat_angles_deg(o->angles.Y, o->angles.X, o->angles.Z);

  Matrix4 mt, mr, ms;
  mat4_set_scale(ms, o->scale);
  mat4_set_translation(mt, o->Position);
  mat4_set_rotation_quat(mr, o->Orientation);

  mat4_multiply(mr, ms, mat);
  mat4_multiply(mt, mat, mat);

  //mat4_multiply(mt, mr, mat);
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

  //TODO put in a transform component or don't add in the list...
  /*
  Component *oc =  create_component(&object_desc);
  oc->data = o;
  object_add_component(o, oc);
  */
  o->component =  create_component(&object_desc);
  o->component->data = o;
  o->component->object = o;
  
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
      Component* cline = create_component(&line_desc);
      //o->line = create_line();
      Line *line = cline->data; 
      line_set_use_depth(line, true);
      line_add_box(line, mesh->box, vec4(0,1,0,0.2));
      object_add_component(o,cline);
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

  //TODO shader vertex anim
  mesh_resend(mesh);
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
  if (!strcmp(c->name, "mesh")) {
    //TODO
    //MeshComponent* cm = c->data;
    //o->mesh = cm->mesh;
  }
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

static PropertySet* s_ps_obj = NULL; //TODO put the property set in component manager

PropertySet* 
property_set_object()
{
  if (s_ps_obj) return s_ps_obj;

  s_ps_obj = create_property_set();
  PropertySet* ps = s_ps_obj;
  PROPERTY_SET_TYPE(ps, Object);


  ADD_PROP(ps, Object, name, EET_T_STRING);

  //TODO clean the property sets
  PropertySet *vec3 = property_set_vec3();
  ADD_PROP_STRUCT_NESTED(ps, Object, Position, vec3);

  PropertySet *an = property_set_vec3();
  ADD_PROP_STRUCT_NESTED(ps, Object, angles, an);

  PropertySet *scale = property_set_vec3();
  ADD_PROP_STRUCT_NESTED(ps, Object, scale, scale);

  // other components
  Eet_Data_Descriptor *ob_descriptor = ps->descriptor;

  EET_DATA_DESCRIPTOR_ADD_LIST
   (ob_descriptor, Object, "components", components,
    component_descriptor);

  return ps;
}

void 
object_descriptor_delete()
{
  free(s_ps_obj->descriptor);
  free(s_ps_obj);
  s_ps_obj = NULL;
  //TODO also free the array
}


ComponentDesc object_desc = {
  "object",
  NULL,
  property_set_object
};

void 
object_post_read(Object* o)
{
  if (!o->component) {
    o->component =  create_component(&object_desc);
    o->component->data = o;
    o->component->object = o;
  }

  Eina_List* l;
  Component* c;

  EINA_LIST_FOREACH(o->components, l, c) {
    c->object = o;

    printf("component name : %s \n", c->name);
    c->funcs = component_manager_desc_get(s_component_manager, c->name);//TODO find from component manager;
    if (c->funcs)
    printf("component functions found, name : %s \n", c->name);
    else
    printf("component functions NOT found, name : %s \n", c->name);
    c->properties = c->funcs->properties();

    if (c->funcs->init)
    c->funcs->init(c);
  }
}

void
object_child_add(Object* parent, Object* child)
{
  parent->children = eina_list_append(parent->children, child);
}
