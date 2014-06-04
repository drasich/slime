#ifndef __armature_component__
#define __armature_component__
#include "component.h"
#include "armature.h"
#include "resource.h"

typedef struct _ArmatureComponent ArmatureComponent;
struct _ArmatureComponent
{
  ArmatureHandle armature_handle;
  Animation* animation;
};

ComponentDesc* component_armature_desc();

Armature* armature_component_armature_get(const ArmatureComponent* mc);
Animation* armature_component_animation_get(const ArmatureComponent* mc);
void armature_component_armature_set(ArmatureComponent* ac, Armature* a);
void armature_component_armature_set_by_name(ArmatureComponent* ac, const char* name);

#endif
