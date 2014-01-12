#include "object.h"
#include "gl.h"
#include "read.h"
#include "scene.h"
#include "log.h"
#include "prefab.h"
#include "resource.h"

void
object_init(Object* o)
{
  quat_set_identity(&o->orientation);
  o->scale.x = 1;
  o->scale.y = 1;
  o->scale.z = 1;
  //o->name = eina_stringshare_add("dance");
}

void
object_del(Object* o)
{
  //TODO clean armature
  components_del(o->components);
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

  Eina_List** components = object_components_get(o);

  EINA_LIST_FOREACH(*components, l, c) {
    if (c->funcs->draw)
    c->funcs->draw(c, mo, projection);
  }

  Object* child;
  Eina_List* lc;
  EINA_LIST_FOREACH(o->children, lc, child) {
    object_draw(child, cam_inv, projection, world);
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

  Eina_List** components = object_components_get(o);

  EINA_LIST_FOREACH(*components, l, c) {
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
object_draw_edit2(
      Object* o,
      const Matrix4 cam_inv,
      const Matrix4 projection,
      const Matrix4 world)
{
  Matrix4 mo;
  mat4_multiply(cam_inv, world, mo);

  Eina_List* l;
  Component* c;

  Eina_List** components = object_components_get(o);

  EINA_LIST_FOREACH(*components, l, c) {
    if (c->funcs->draw)
    c->funcs->draw(c, mo, projection);
    if (c->funcs->draw_edit)
    c->funcs->draw_edit(c, mo, projection);
  }
}

void 
object_draw_edit_component2(
      Object* o,
      const Matrix4 cam_inv,
      const Matrix4 projection,
      const Matrix4 world,
      const char* name)
{
  Matrix4 mo;
  mat4_multiply(cam_inv, world, mo);

  Eina_List* l;
  Component* c;

  Eina_List** components = object_components_get(o);

  EINA_LIST_FOREACH(*components, l, c) {
    if (strcmp(c->name, name)) continue;

    if (c->funcs->draw)
    c->funcs->draw(c, mo, projection);
    if (c->funcs->draw_edit)
    c->funcs->draw_edit(c, mo, projection);
  }
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

  Eina_List** components = object_components_get(o);

  EINA_LIST_FOREACH(*components, l, c) {
    if (strcmp(c->name, name)) continue;

    if (c->funcs->draw)
    c->funcs->draw(c, mo, projection);
    if (c->funcs->draw_edit)
    c->funcs->draw_edit(c, mo, projection);
  }
}



void
object_compute_matrix(Object* o, Matrix4 mat)
{
  if (o->orientation_type == ORIENTATION_EULER) {
    o->orientation = quat_yaw_pitch_roll_deg(o->angles.y, o->angles.x, o->angles.z);
  }

  Matrix4 mt, mr, ms;
  mat4_set_scale(ms, o->scale);
  mat4_set_translation(mt, o->position);
  mat4_set_rotation_quat(mr, o->orientation);

  mat4_multiply(mr, ms, mat);
  mat4_multiply(mt, mat, mat);

  //mat4_multiply(mt, mr, mat);
}

void
object_compute_matrix_with_quat(Object* o, Matrix4 mat)
{
  Matrix4 mt, mr;
  mat4_set_translation(mt, o->position);
  mat4_set_rotation_quat(mr, o->orientation);
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

  Eina_List** components = object_components_get(o);

  EINA_LIST_FOREACH(*components, l, c) {
    if (c->funcs->update)
    c->funcs->update(c, 0.01f);
  }

  object_compute_matrix(o, o->matrix);
}

void object_add_component_armature(Object* o, Armature* a)
{
  o->armature = a;
}

Object*
object_new()
{
  return create_object();
}

Object*
create_object()
{
  Object* o = calloc(1, sizeof(Object));
  o->name = eina_stringshare_add("new_obj");
  object_init(o);
  //eina_value_setup(&o->data_position, EINA_VALUE_TYPE_DOUBLE);
  //eina_value_set(&o->data_position, 777);

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

//TODO remove
Object* create_object_file(const char* path)
{
  Object* o = create_object();
  o->name = path;
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
      /*
      Component* comp = create_component(&mesh_desc);
      object_add_component(o, comp);
      Mesh* mesh = comp->data;
      mesh_read_file(mesh,f);
      mesh->name = path;
      //Mesh* mesh = create_mesh_file(f);
      o->mesh = mesh;
      */


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
  o->position = v;
}

void 
object_set_orientation(Object* o, Quat q)
{
  printf("remove this function for now!!!!!!!!!!!!\n");
  o->orientation = q;
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

    Vec3f newpos = vec3d_to_vec3f(vec3_add(vi->position,translation));
    Vec3 newnor = quat_rotate_vec3(rotation, vi->normal);

    /*
    mesh->vertices[i*3] = newpos.x;
    mesh->vertices[i*3+1] = newpos.y;
    mesh->vertices[i*3+2] = newpos.z;
    */

    eina_inarray_replace_at(mesh->vertices, i*3, &newpos.x);
    eina_inarray_replace_at(mesh->vertices, i*3 +1, &newpos.y);
    eina_inarray_replace_at(mesh->vertices, i*3 +2, &newpos.z);

    mesh->normals[i*3] = newnor.x;
    mesh->normals[i*3+1] = newnor.y;
    mesh->normals[i*3+2] = newnor.z;
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
  Eina_List** components = object_components_get(o);
  *components = eina_list_append(*components, c);
  //o->components = eina_list_append(o->components, c);
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
  Eina_List** components = object_components_get(o);
  *components = eina_list_remove(*components, c);
}

void*
object_component_get(const Object* o, const char* name)
{
  Eina_List* l;
  Component* c;

  Eina_List** components = object_components_get(o);

  EINA_LIST_FOREACH(*components, l, c) {
    if ( c->name && !strcmp(c->name, name))
    return c->data;
  }

  return NULL;

}

static Property* s_ps_obj = NULL; //TODO put the property set in component manager

Property* 
property_set_object()
{
  if (s_ps_obj) return s_ps_obj;

  s_ps_obj = property_set_new();
  Property* ps = s_ps_obj;
  ps->name = "objectblablabla";
  PROPERTY_SET_TYPE(ps, Object);

  PROPERTY_BASIC_ADD(ps, Object, name, EET_T_STRING);
  PROPERTY_BASIC_ADD(ps, Object, id, EET_T_ULONG_LONG);

  //TODO clean the property sets
  Property *vec3 = property_set_vec3();
  PROPERTY_SUB_NESTED_ADD(ps, Object, position, vec3);

  /*
  Property *an = property_set_vec3();
  ADD_PROP_STRUCT_NESTED(ps, Object, angles, an);
  */

  Property *q = property_set_quat();
  PROPERTY_SUB_NESTED_ADD(ps, Object, orientation, q);

  Property *scale = property_set_vec3();
  PROPERTY_SUB_NESTED_ADD(ps, Object, scale, scale);

  // other components
  Eet_Data_Descriptor *ob_descriptor = ps->descriptor;

  EET_DATA_DESCRIPTOR_ADD_LIST
   (ob_descriptor, Object, "components", components,
    component_descriptor);

  EET_DATA_DESCRIPTOR_ADD_LIST
   (ob_descriptor, Object, "children", children,
    ob_descriptor);

  Property* prefab = property_set_resource_handle(RESOURCE_PREFAB);
  EET_DATA_DESCRIPTOR_ADD_SUB_NESTED(
        ob_descriptor, Object, "prefab", prefab, prefab->descriptor);

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
object_post_read(Object* o, struct _Scene* s)
{
  if (!o->component) {
    o->component =  create_component(&object_desc);
    o->component->data = o;
    o->component->object = o;
  }

  o->scene = s;
  if (s) {
    if (o->id > s->last_id)
    s->last_id = o->id;
  }

  if (o->prefab.name) {
    o->prefab.prefab = resource_prefab_get(s_rm, o->prefab.name);
  }

  Eina_List* l;
  Eina_List* lnext;
  Component* c;

  Eina_List** components = object_components_get(o);

  EINA_LIST_FOREACH_SAFE(*components, l, lnext, c) {

    if (!c->name) {
      EINA_LOG_DOM_WARN(log_object_dom, "It seems this component does not exist anymore: %s\n==> removing component", c->name);
      free(c);
      *components = eina_list_remove_list(*components, l);
    }
    else {
      EINA_LOG_DOM_DBG(log_object_dom, "post read, object : %s , component name : %s", o->name, c->name);
      c->funcs = component_manager_desc_get(s_component_manager, c->name);//TODO find from component manager;
      if (c->funcs) {
        c->object = o;
        EINA_LOG_DOM_DBG(log_object_dom, "component functions found, name : %s", c->name);
        c->properties = c->funcs->properties();
        if (c->funcs->init)
        c->funcs->init(c);
      }
      else {
        EINA_LOG_DOM_WARN(log_object_dom, "component functions NOT found, name: %s\n==> removing component", c->name);
        free(c);
        *components = eina_list_remove_list(*components, l);
      }
    }
  }

  Object* child;
  EINA_LIST_FOREACH(o->children, l, child) {
    object_post_read(child, s);
    child->parent = o;
  }
}

void
object_child_add(Object* parent, Object* child)
{
  parent->children = eina_list_append(parent->children, child);
  child->parent = parent;
  parent->dirty = true;
  child->dirty = true;
}

Vec3 object_world_position_get(Object* o)
{
  if (o->parent) {
    Quat wo = object_world_orientation_get(o->parent);
    Vec3 p = quat_rotate_vec3(wo, o->position);
    //return vec3_add(o->position, object_world_position_get(o->parent));
    return vec3_add(p, object_world_position_get(o->parent));
  }
  else
  return o->position;
}

Quat object_world_orientation_get(Object* o)
{
  if (o->parent)
  return quat_mul(object_world_orientation_get(o->parent), o->orientation);
  else
  return o->orientation;
}

Vec3 object_world_scale_get(Object* o)
{
  if (o->parent)
  return vec3_vec3_mul(o->scale, object_world_scale_get(o->parent));
  else
  return o->scale;
}

void
object_world_position_set(Object* o, Vec3 worldpos)
{
  if (o->parent) {
    Vec3 parentworld = object_world_position_get(o->parent);
    Quat wo = object_world_orientation_get(o->parent);
    Vec3 diff = vec3_sub(worldpos, parentworld);
    o->position = quat_rotate_vec3(quat_inverse(wo), diff);
  }
  else
  o->position = worldpos;
}

static Eina_List*
_object_parents_path_get(Object* o, Eina_List* l)
{
  if (o->parent) {
    l = eina_list_prepend(l, o->parent);
    l = _object_parents_path_get(o->parent, l);
  }

  return l;
}

Eina_List*
object_parents_path_get(Object* o)
{
  Eina_List* l = NULL;
  l = _object_parents_path_get(o, l);
  return l;
}

Object*
object_copy(const Object* oo)
{
  int size;
  void *encoded = eet_data_descriptor_encode(
        s_ps_obj->descriptor,
        oo,
        &size);

  Object* o = eet_data_descriptor_decode(
        s_ps_obj->descriptor,
        encoded,
        size);

  return o;
}

static const char OBJECT_FILE_ENTRY[] = "object";

Eina_Bool
object_write(const Object* o, const char* filename)
{
  Eina_Bool ret;
  Eet_File *ef = eet_open(filename, EET_FILE_MODE_WRITE);
  if (!ef) {
    EINA_LOG_DOM_ERR(log_object_dom, "error reading file %s", filename);
    return EINA_FALSE;
  }

  ret = eet_data_write(ef, s_ps_obj->descriptor, OBJECT_FILE_ENTRY, o, EINA_TRUE);
  eet_close(ef);

  return ret;
}

Object*
object_read(const char* filename)
{
  Object* o;

  Eet_File *ef = eet_open(filename, EET_FILE_MODE_READ);
  if (!ef) {
    EINA_LOG_DOM_ERR(log_object_dom, "error reading file '%s'.", filename);
    return NULL;
  }

  o = eet_data_read(ef, s_ps_obj->descriptor, OBJECT_FILE_ENTRY);
  //printf("scene read data dump\n");
  //eet_data_dump(ef, SCENE_FILE_ENTRY, _output, NULL);
  //printf("scene read data dump end\n");
  eet_close(ef);
 
  return o;  
}

Eina_List**
object_components_get(const Object* o)
{
  if (o->prefab.prefab) {
    return (Eina_List**) &o->prefab.prefab->components;
  }

  return (Eina_List**) &o->components;
}
