#include "meshcomponent.h"
#include "component.h"
#include "resource.h"

static void
_mesh_component_shader_changed_cb(const char* oldname, const char* newname, void* data)
{
  MeshComponent* mc = data;
  printf("this callback is called? %s, %s\n", mc->mesh_handle.name, newname);
  mesh_component_shader_set_by_name(mc, newname);
}


static void*
_mesh_component_create()
{
  MeshComponent* m = calloc(1,sizeof *m);
  return m;
}

static Property* 
_mesh_component_properties()
{
  Property* ps = create_property_set();

  Property* mh = property_set_resource_handle(RESOURCE_MESH);
  mh->name = "mesh";
  PROPERTY_SUB_NESTED_ADD(ps, MeshComponent, mesh_handle, mh);

  Property* sh = property_set_resource_handle(RESOURCE_SHADER);
  sh->name = "shader";
  PROPERTY_SUB_NESTED_ADD(ps, MeshComponent, shader_handle, sh);
  
  Property *shader_instance_property_set = property_set_shader_instance();
  PROPERTY_SUB_ADD(ps, MeshComponent, shader_instance, shader_instance_property_set);

  return ps;
}

static Eina_Bool uniform_send(
      const Eina_Hash *hash,
      const void *key,
      void *data,
      void *fdata)
{
  Shader* s = fdata;

  Uniform* uni = shader_uniform_get(s, key);
  GLint uniloc =  uni->location;
  if (uniloc < 0) {
    printf("no such uniform '%s' \n", key);
    return EINA_FALSE;
  }

  //TODO data
  if (uni->type == UNIFORM_VEC4) {
    Vec4* v = data;
    gl->glUniform4f(uniloc, v->x,v->y,v->z,v->w);
  }

  return EINA_TRUE;
}


static void 
_mesh_component_draw(Component* c, Matrix4 world, const Matrix4 projection)
{
  MeshComponent* mc = c->data;
  if (!mc) return;

  if (mc->hide) return;

  /*
  Mesh* m = mc->mesh;
  if (!m) {
    m = resource_mesh_get(s_rm, mc->mesh_name);
    if (m) mc->mesh = m;
    else return;
  }
  */

  MeshHandle mh = mc->mesh_handle;
  Mesh* m = mh.mesh;
  if (!m) {
    //m = resource_mesh_get(s_rm, mc->mesh_name);
    //if (m) mc->mesh = m;
    //else 
    return;
  }


  ShaderHandle sh = mc->shader_handle;
  Shader* s = sh.shader;
  if (!s) {
    /*
    s = resource_shader_get(s_rm, mc->shader_name);
    if (s) mc->shader = s;
    else*/ return;
  }
  if (sh.state == RESOURCE_STATE_CHANGED) {
    mesh_component_shader_set(mc, s);
  }

  shader_use(s);

  if (!mc->shader_instance)
  mc->shader_instance = shader_instance_create(s);


  if (mc->shader_instance) {
    eina_hash_foreach(mc->shader_instance->uniforms, uniform_send, s);
  }

  if (!m->is_init) {
    mesh_init(m);
  }

  shader_matrices_set(s, world, projection);
  shader_mesh_draw(s,mc);
}

static void
_mesh_component_init(Component* c)
{
  MeshComponent* mc = c->data;
  if (!mc) return;

  mc->shader_handle.cb = _mesh_component_shader_changed_cb;
  mc->shader_handle.data = mc;
}

ComponentDesc mesh_desc = {
  "mesh",
  _mesh_component_create,
  _mesh_component_properties,
  _mesh_component_init,
  NULL,
  _mesh_component_draw,
  NULL,
  NULL,
};

Shader*
mesh_component_shader_get(MeshComponent* mc)
{
  //return mc->shader;
  return mc->shader_handle.shader;
}

void
mesh_component_shader_set_by_name(MeshComponent* mc, const char* name)
{
  Shader* s = resource_shader_get(s_rm, name);

  if (!s) {
    printf("cannot find shader '%s'\n", name);
    return;
  }

  mesh_component_shader_set(mc, s);
}

void
mesh_component_shader_set(MeshComponent* mc, Shader* s)
{
  //mc->shader_name = s->name;
  //mc->shader = s;

  mc->shader_handle.name = s->name;
  mc->shader_handle.shader = s;
  mc->shader_handle.state = RESOURCE_STATE_OK;

  if (mc->shader_instance) {
    //TODO save the instance for later use? if someone else will link to this shader we already have
    //an instance
    eina_hash_free(mc->shader_instance->textures);
    eina_hash_free(mc->shader_instance->uniforms);
    mc->shader_instance->textures = NULL;
    mc->shader_instance->uniforms = NULL;
    free(mc->shader_instance);
    mc->shader_instance = NULL;
  }

  mc->shader_instance = shader_instance_create(s);
}


Mesh* mesh_component_mesh_get(const MeshComponent* mc)
{
  return mc->mesh_handle.mesh;
}

void mesh_component_mesh_set(MeshComponent* mc, Mesh* m)
{
  mc->mesh_handle.mesh = m;
  mc->mesh_handle.name = m->name;
}

void mesh_component_mesh_set_by_name(MeshComponent* mc, const char* name)
{
  resource_mesh_handle_set(s_rm, &mc->mesh_handle, name);
}

