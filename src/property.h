#ifndef __property__
#define __property__
#include <Eina.h>
#include <Eet.h>
#include "stdbool.h"

typedef struct _Property Property;

typedef enum _Hint Hint;
enum _Hint{
  VERTICAL,
  HORIZONTAL
};

typedef enum _ResourceType ResourceType;
enum _ResourceType{
  RESOURCE_TEXTURE,
  RESOURCE_MESH,
  RESOURCE_SHADER
};

struct _Property
{
  const char* name;
  int type;
  int offset;
  size_t size;
  Property* sub;

  bool is_resource;
  const char* resource_type;
  ResourceType resource_type_new;

  Eina_List* list;
  Hint hint;
  Eet_Data_Descriptor *descriptor;
};

int property_offset_get(const Property* p);

Property* create_property_set();
int property_type_check(int type);
Property* property_real_parent_get(Property* p);

enum {
  PROPERTY_FILENAME = EET_I_LIMIT,
  PROPERTY_STRUCT,
  PROPERTY_STRUCT_NESTED,
  PROPERTY_POINTER,
  PROPERTY_RESOURCE,
  PROPERTY_ROOT,
  TEST1,
  TEST2
};

#define PROPERTY_SET_TYPE(ps, type) \
 if (!ps->descriptor) { \
   Eet_Data_Descriptor_Class eddc; \
   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, type); \
   ps->descriptor = eet_data_descriptor_stream_new(&eddc); \
   ps->name = # type; \
 }

//TODO chris
#define PROPERTY_NEW(p, struct_type, member, member_type) \
 do { \
   struct_type ___ett; \
   p = calloc(1, sizeof *p); \
   p->name = # member; \
   p->type = member_type; \
   p->offset = (char *)(& (___ett.member)) - (char *)(& (___ett)); \
   p->size = sizeof ___ett.member; \
   \
 } while(0)


#define PROPERTY_BASIC_ADD_WITH_NAME(ps, struct_type, member, member_type, name_str) \
 do { \
   Property *p; \
   PROPERTY_NEW(p, struct_type, member, member_type); \
   p->name = name_str; \
   ps->list = eina_list_append(ps->list, p); \
   PROPERTY_SET_TYPE(ps, struct_type); \
   EET_DATA_DESCRIPTOR_ADD_BASIC(ps->descriptor, struct_type, name_str, member, member_type); \
 } while(0)

#define PROPERTY_BASIC_ADD(ps, struct_type, member, member_type) \
 PROPERTY_BASIC_ADD_WITH_NAME(ps, struct_type, member, member_type, # member)


#define PROPERTY_SUB_ADD(ps, struct_type, member, ssub) \
 do { \
   Property *p; \
   PROPERTY_NEW(p, struct_type, member, PROPERTY_STRUCT); \
   p->sub = ssub; \
   ps->list = eina_list_append(ps->list, p); \
   PROPERTY_SET_TYPE(ps, struct_type); \
   EET_DATA_DESCRIPTOR_ADD_SUB(ps->descriptor, struct_type, # member, member, ssub->descriptor); \
 } while(0)


//TODO remove the offset
#define PROPERTY_SUB_NESTED_ADD(ps, struct_type, member, ssub) \
 do { \
   Property *p; \
   PROPERTY_NEW(p, struct_type, member, PROPERTY_STRUCT_NESTED); \
   p->sub = ssub; \
   /*add_offset(ssub, p->offset); */ \
   ps->list = eina_list_append(ps->list, p); \
   PROPERTY_SET_TYPE(ps, struct_type); \
   EET_DATA_DESCRIPTOR_ADD_SUB_NESTED(ps->descriptor, struct_type, # member, member, ssub->descriptor); \
 } while(0)


#define PROPERTY_RESOURCE_ADD(ps, struct_type, member, resource_type_str) \
 do { \
   Property* p; \
   PROPERTY_NEW(p, struct_type, member, EET_T_STRING); \
   p->name = resource_type_str; \
   p->sub = NULL; \
   p->is_resource = true; \
   p->resource_type = resource_type_str; \
   ps->list = eina_list_append(ps->list, p); \
   PROPERTY_SET_TYPE(ps, struct_type); \
   EET_DATA_DESCRIPTOR_ADD_BASIC(ps->descriptor, struct_type, # member, member, EET_T_STRING);\
 } while(0)

#define PROPERTY_RESOURCE_ADD_NEW(ps, struct_type, member, resource_type_id) \
 do { \
   Property* p; \
   PROPERTY_NEW(p, struct_type, member, PROPERTY_RESOURCE); \
   p->name = # member; \
   p->sub = NULL; \
   /*p->is_resource = true;*/ \
   /*p->resource_type = resource_type_str;*/ \
   p->resource_type_new = resource_type_id; \
   ps->list = eina_list_append(ps->list, p); \
   PROPERTY_SET_TYPE(ps, struct_type); \
   /*EET_DATA_DESCRIPTOR_ADD_BASIC(ps->descriptor, struct_type, # member, member, EET_T_STRING);*/\
   EET_DATA_DESCRIPTOR_ADD_SUB(ps->descriptor, struct_type, # member, member, property_set_resource()->descriptor); \
 } while(0)


#define PROPERTY_HASH_ADD(ps, struct_type, member, psdata) \
 do { \
   Property* p; \
   PROPERTY_NEW(p, struct_type, member, EET_G_HASH); \
   p->sub = psdata; \
   ps->list = eina_list_append(ps->list, p); \
   PROPERTY_SET_TYPE(ps, struct_type); \
   EET_DATA_DESCRIPTOR_ADD_HASH(ps->descriptor, struct_type, # member, member, psdata->descriptor);\
 } while(0)


Property* property_set_vec3();
Property* property_set_vec4();
Property* property_set_quat();
Property* property_set_resource_handle();
Property* property_set_resource_mesh_handle();




#endif
