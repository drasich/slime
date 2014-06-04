#include "armature_component.h"
#include "component.h"
#include "resource.h"

/*
static void
_mesh_component_shader_changed_cb(const char* oldname, const char* newname, void* data)
{
  MeshComponent* mc = data;
  mesh_component_shader_set_by_name(mc, newname);
}
*/


static void*
_armature_component_create()
{
  ArmatureComponent* a = calloc(1, sizeof *a);
  a->animation = calloc(1, sizeof *a->animation);
  return a;
}

static Property* 
_armature_component_properties()
{
  Property* ps = property_set_new();

  Property* ah = property_set_resource_handle(RESOURCE_ARMATURE);
  ah->name = "armature";
  PROPERTY_SUB_NESTED_ADD(ps, ArmatureComponent, armature_handle, ah);

  return ps;
}

static void _armature_component_post_read(Component* c)
{
  ArmatureComponent* ac = c->data;
  ac->animation = calloc(1, sizeof *ac->animation);

  ArmatureHandle* ah = &ac->armature_handle;
  if (ah->name) {
    if (ah->armature)
    resource_armature_get(s_rm, ah->name);
    ah->armature = resource_armature_get(s_rm, ah->name);
  }
}


ComponentDesc*
component_armature_desc()
{
  static ComponentDesc* cd = NULL;
  if (cd) return cd;

  cd = calloc(1, sizeof * cd);
  cd->name = "armature";
  cd->create = _armature_component_create;
  cd->properties = _armature_component_properties;
  cd->post_read = _armature_component_post_read;
  return cd;
}


Armature*
armature_component_armature_get(const ArmatureComponent* ac)
{
  return ac->armature_handle.armature;
}

Animation*
armature_component_animation_get(const ArmatureComponent* ac)
{
  return ac->animation;
}

/*
void armature_component_armature_set(ArmatureComponent* ac, Armature* a)
{
  ac->armature_handle.armature = a;
  ac->armature_handle.name = a->name;
}
*/

/*
void armature_component_armature_set_by_name(ArmatureComponent* ac, const char* name)
{
  resource_armature_handle_set(s_rm, &ac->armature_handle, name);
}
*/

