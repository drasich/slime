#include "object.h"
#include "gl.h"
#include "read.h"
#include "scene.h"
#include "log.h"
#include "prefab.h"
#include "resource.h"
#include "component/meshcomponent.h"
#include "component/armature_component.h"

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

  if (
        o->orientation.x == 0 &&
        o->orientation.y == 0 &&
        o->orientation.z == 0 &&
        o->orientation.w == 0 )
   {
    EINA_LOG_ERR("TOFIX quaternion became 0,0,0,0");
    o->orientation = quat_identity();
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
  Animation* anim = object_animation_get(o);
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
  Animation* anim = object_animation_get(o);
  if (anim != NULL) {
    _animation_update(o, 0.05f);
    static float stime = 0;
    static bool played =false;
    stime += 0.05f;
    if (stime > 1115 && !played) {
      printf("anim stop!!!\n");
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

  Object* child;
  EINA_LIST_FOREACH(o->children, l, child) {
    object_update(child);
  }

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

  o->component =  create_component(component_object_desc());
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
    /*
    else if (!strcmp(type, "armature")){
      Armature* armature = create_armature_file(f);
      object_add_component_armature(o, armature);
      o->animation = calloc(1, sizeof *o->animation);
    }
    */
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
  Mesh* mesh = object_mesh_get(o);
  Armature* armature = object_armature_get(o);

  if (mesh == NULL || armature == NULL) return;

  VertexInfo *vi;
  int i = 0;
  EINA_INARRAY_FOREACH(mesh->vertices_base, vi) {
    Weight* w;
    Vec3 translation = vec3_zero();
    Quat rotation = quat_identity();
    EINA_INARRAY_FOREACH(vi->weights, w) {
      VertexGroup* vg = eina_array_data_get(mesh->vertexgroups, w->index);
      Bone* bone = armature_find_bone(armature, vg->name);

      //TODO
      //WARNING!!!!!!!!!!!!!
      // armature rotation doesn't have any effect (for now? I don't know yet if it should)

      if (w->weight == 0) continue;
      //if (w->weight != 1) continue;
      //if (bone->position_base.x == 0) continue;

      
      /*
      printf("__bone position BASE : %f, %f, %f \n",
            bone->position_base.x,
            bone->position_base.y,
            bone->position_base.z);

      printf("__bone position : %f, %f, %f \n",
            bone->position.x,
            bone->position.y,
            bone->position.z);
            */
      /*
      printf("____bone rotation base : %f, %f, %f, %f  \n",
            bone->rotation_base.x,
            bone->rotation_base.y,
            bone->rotation_base.z,
            bone->rotation_base.w);

      printf("____bone rotation : %f, %f, %f, %f  \n",
            bone->rotation.x,
            bone->rotation.y,
            bone->rotation.z,
            bone->rotation.w);
      */

      ///////////////

      //Vec3 bone_trans = vec3_sub(bone->position,bone->position_base);
      Vec3 bone_trans = vec3_vec3_mul(bone->position, armature->scale);
      Vec3 bone_trans_weight = vec3_mul(bone_trans, w->weight);

      Vec3 mytranslation = quat_rotate_vec3(quat_inverse(bone->rotation_base), bone_trans_weight);

      /*
      if (mytranslation.x != 0
        ||mytranslation.y != 0
        ||mytranslation.z != 0)
      printf("____mytranslation : %f, %f, %f \n",
            mytranslation.x,
            mytranslation.y,
            mytranslation.z);
            */


      //Quat bone_rot = quat_between_quat(bone->rotation_base,bone->rotation);
      //Quat bone_rot = quat_between_quat(quat_identity(),bone->rotation);
      //Quat bone_rot_weight = quat_slerp(quat_identity(), bone_rot, w->weight);
      //Quat bone_rot_weight = quat_slerp(quat_identity(), bone->rotation, w->weight);
      Quat bone_rot_weight = quat_slerp(quat_identity(), quat_inverse(bone->rotation), w->weight);
      //Vec3 realposbase = quat_rotate_vec3(quat_inverse(bone->rotation_base), bone->position_base);
      Vec3 realposbase = vec3_vec3_mul(bone->position_base, armature->scale);
      realposbase = vec3_add(realposbase, armature->position);
      //Vec3 vipos_bone = vec3_sub(vi->position, bone->position_base);
      Vec3 vipos_bone = vec3_sub(vi->position, realposbase);
      Vec3 tr_rot = quat_rotate_vec3(bone_rot_weight, vipos_bone);
      tr_rot = vec3_sub(tr_rot, vipos_bone);
      mytranslation = vec3_add(mytranslation, tr_rot);

      /*
      printf("START %d ____vi position : %f, %f, %f \n",
            i,
            vi->position.x,
            vi->position.y,
            vi->position.z);

      printf("____realpose base : %f, %f, %f \n",
            realposbase.x,
            realposbase.y,
            realposbase.z);
      printf("____bone rot w : %f, %f, %f \n",
            vipos_bone.x,
            vipos_bone.y,
            vipos_bone.z);

      printf("____rot w : %f, %f, %f \n",
            bone_rot_weight.x,
            bone_rot_weight.y,
            bone_rot_weight.z,
            bone_rot_weight.w);

      printf("____tr rot : %f, %f, %f \n",
            tr_rot.x,
            tr_rot.y,
            tr_rot.z);

      printf("____translate : %f, %f, %f \n",
            mytranslation.x,
            mytranslation.y,
            mytranslation.z);
            */

      translation = vec3_add(translation, mytranslation);
      //rotation = quat_inverse(bone->rotation);

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
  MeshComponent* mc = object_component_get(o, "mesh");
  ArmatureComponent* ac = object_component_get(o, "armature");
  if (!mc || !ac) return;

  Mesh* mesh = mesh_component_mesh_get(mc);
  Armature* armature = armature_component_armature_get(ac);
  if (!mesh || !armature) return;

  armature_set_pose(armature, action_name, time);
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
  c->object = o;
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

ComponentDesc* component_object_desc()
{
  static ComponentDesc* cd = NULL;
  if (cd) return cd;

  cd = calloc(1, sizeof * cd);
  cd->name = "object";
  cd->properties = property_set_object;
  return cd;
}


void 
object_post_read(Object* o)
{
  if (!o->component) {
    o->component =  create_component(component_object_desc());
    o->component->data = o;
    o->component->object = o;
  }

  /*
  o->scene = s;
  if (s) {
    if (o->id > s->last_id)
    s->last_id = o->id;
  }
  */

  if (o->prefab.name && strcmp(o->prefab.name,"")) {
  //if (o->prefab.name) {
    o->prefab.prefab = resource_prefab_get(s_rm, o->prefab.name);
    if (!o->prefab.prefab) o->prefab.name = NULL;
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
        if (c->funcs->init_edit)
        c->funcs->init_edit(c);
        EINA_LOG_DOM_DBG(log_object_dom, "___________________ component functions found, name : %s", c->name);
        if (c->funcs->post_read)
        c->funcs->post_read(c);
        EINA_LOG_DOM_DBG(log_object_dom, ">>>>>>>>>>>>>> component functions found, name : %s", c->name);
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
    object_post_read(child);
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
  if (o->prefab.name && strcmp(o->prefab.name,"")) {
    return (Eina_List**) &o->prefab.prefab->components;
  }

  return (Eina_List**) &o->components;
}

void object_prefab_unlink(Object* o)
{
  Prefab* p = o->prefab.prefab;
  if (p) {
    ComponentList* cl = components_copy(p->components);
    o->components = cl->list;
    free(cl);
    o->prefab.prefab = NULL;
    //o->prefab.name = "";
    o->prefab.name = NULL;
    object_post_read(o);
  }

}

Mesh*
object_mesh_get(Object* o)
{
  MeshComponent* mc = object_component_get(o, "mesh");
  if (mc) {
    return mesh_component_mesh_get(mc);
  }

  return NULL;
}

Armature*
object_armature_get(Object* o)
{
  ArmatureComponent* ac = object_component_get(o, "armature");
  if (ac) {
    return armature_component_armature_get(ac);
  }

  EINA_LOG_ERR("object '%s' has no armature component", o->name);
  return NULL;
}

Animation*
object_animation_get(Object* o)
{
  ArmatureComponent* ac = object_component_get(o, "armature");
  if (ac) {
    return armature_component_animation_get(ac);
  }

  return NULL;
}

void
object_components_init(Object* o)
{
  Eina_List* l;
  Component* c;

  Eina_List** components = object_components_get(o);

  EINA_LIST_FOREACH(*components, l, c) {
    if (c->funcs->init)
    c->funcs->init(c);
  }

}
