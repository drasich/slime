#include "linecomponent.h"
#include "component.h"
#include "resource.h"

static void*
_line_component_create()
{
  LineComponent* l = calloc(1, sizeof *l);
  l->mesh = mesh_new(); 
  l->mesh->mode = GL_LINES;
  l->mesh->vertices = eina_inarray_new(sizeof(GLfloat), 3);
  l->mesh->colors = eina_inarray_new(sizeof(GLfloat), 4);
  Shader* s = resource_shader_get(s_rm, "line");
  if (s) {
    l->shader_handle.name = s->name;
    l->shader_handle.shader = s;
    l->shader_handle.state = RESOURCE_STATE_OK;
    l->shader_instance = shader_instance_create(s);
  }
  return l;
}

LineComponent*
line_component_create()
{
  return _line_component_create();
}

/*
static Eina_Bool uniform_send(
      const Eina_Hash *hash,
      const void *key,
      void *data,
      void *fdata)
{
  Shader* s = fdata;

  Uniform* uni = shader_uniform_get(s, key);
  if (!uni) {
    printf("%s : cannot find uniform '%s' \n", __FUNCTION__, key);
    return EINA_FALSE;
  }
  GLint uniloc =  uni->location;
  if (uniloc < 0) {
    printf("no such uniform '%s' \n", key);
    return EINA_FALSE;
  }

  UniformValue* uv = data;
  if (uni->type == UNIFORM_VEC4) {
    Vec4* v = &uv->value.vec4;
    glUniform4f(uniloc, v->x,v->y,v->z,v->w);
  }
  else if (uni->type == UNIFORM_INT) {
    glUniform1i(uniloc, uv->value.i);
  }
  else if (uni->type == UNIFORM_FLOAT) {
    glUniform1f(uniloc, uv->value.f);
  }
  if (uni->type == UNIFORM_VEC3) {
    Vec3* v = &uv->value.vec3;
    glUniform3f(uniloc, v->x,v->y,v->z);
  }
  else {
    printf("line component uniform send not yet \n");
  }

  return EINA_TRUE;
}
*/


static void 
_line_component_draw(Component* c, Matrix4 world, const Matrix4 projection)
{
  LineComponent* lc = c->data;
  if (!lc) {
    printf("no line component data\n");
    return;
  }

  line_component_draw(lc, world, projection);
}

void
line_component_draw(LineComponent* lc, Matrix4 world, const Matrix4 projection)
{
  if (!lc) {
    printf("no line component data\n");
    return;
  }

  //if (lc->hide) return;

  Mesh* m = lc->mesh;
  if (!m) {
    printf("no mesh \n");
    return;
  }


  ShaderHandle sh = lc->shader_handle;
  Shader* s = sh.shader;
  if (!s) {
    printf("no shader \n");
    return;
  }

  /*
  if (sh.state == RESOURCE_STATE_CHANGED) {
    mesh_component_shader_set(mc, s);
  }
  */

  shader_use(s);

  if (!lc->shader_instance) {
    lc->shader_instance = shader_instance_create(s);
  }

  //TODO this is needed for draggers
  if (lc->shader_instance && lc->shader_instance->uniforms) {
    eina_hash_foreach(lc->shader_instance->uniforms, uniform_send, s);
  }

  if (!m->is_init) {
    mesh_init(m);
  }

  shader_matrices_set(s, world, projection);
  shader_mesh_nocomp_draw(s, lc->shader_instance, m);
}

static void
_line_component_init(Component* c)
{
  LineComponent* lc = c->data;
  if (!lc) return;

  ShaderHandle* sh = &lc->shader_handle;
  if (sh->name) {
    sh->shader = resource_shader_get(s_rm, sh->name);
  }

  if (lc->shader_instance)
  shader_instance_init(lc->shader_instance);
}

static ComponentDesc* _line_desc = NULL;
ComponentDesc*
component_line()
{
  if (_line_desc) return _line_desc;

  ComponentDesc* cd = calloc(1, sizeof *cd);
  cd->name = "line";
  cd->create = _line_component_create;
  cd->init = _line_component_init;
  cd->draw = _line_component_draw;

  _line_desc = cd;
  return _line_desc;
}


void 
line_add_grid(LineComponent* l, int num, int space)
{
  Vec4 color = vec4(1,1,1,0.1);
  Vec4 xc = vec4(1.0,0.247,0.188,0.4);
  Vec4 zc = vec4(0,0.4745,1,0.4);

  int i;
  for ( i = -num; i <= num; ++i) {
    Vec3 p1 = vec3(i*space, 0, -space*num);
    Vec3 p2 = vec3(i*space, 0, space*num);
    if (i == 0)
    line_add_color(l, p1, p2, zc);
    else
    line_add_color(l, p1, p2, color);
  }

  for ( i = -num; i <= num; ++i) {
    Vec3 p1 = vec3(-space*num, 0, i*space);
    Vec3 p2 = vec3(space*num, 0, i*space);
    if (i == 0)
    line_add_color(l, p1, p2, xc);
    else
    line_add_color(l, p1, p2,color);
  }

}

void
line_add_color(LineComponent* lc, Vec3 p1, Vec3 p2, Vec4 color)
{
  Eina_Inarray* vertices = lc->mesh->vertices;
  Eina_Inarray* colors = lc->mesh->colors;
  
  GLfloat f;
  f = p1.x;
  eina_inarray_push(vertices, &f);
  f = p1.y;
  eina_inarray_push(vertices, &f);
  f = p1.z;
  eina_inarray_push(vertices, &f);

  f = p2.x;
  eina_inarray_push(vertices, &f);
  f = p2.y;
  eina_inarray_push(vertices, &f);
  f = p2.z;
  eina_inarray_push(vertices, &f); 

  int i;
  for (i = 0; i < 2; ++i) { 
    f = color.x;
    eina_inarray_push(colors, &f);
    f = color.y;
    eina_inarray_push(colors, &f);
    f = color.z;
    eina_inarray_push(colors, &f);
    f = color.w;
    eina_inarray_push(colors, &f);
  }

  mesh_buffer_add(
        lc->mesh,
        "vertex",
        GL_ARRAY_BUFFER,
        vertices->members,
        vertices->len * vertices->member_size);

  mesh_buffer_add(
        lc->mesh,
        "color",
        GL_ARRAY_BUFFER,
        colors->members,
        colors->len * colors->member_size);
}

void 
line_clear(LineComponent* l)
{
  eina_inarray_free(l->mesh->vertices);
  eina_inarray_free(l->mesh->colors);

  l->mesh->vertices = eina_inarray_new(sizeof(GLfloat), 3);
  l->mesh->colors = eina_inarray_new(sizeof(GLfloat), 4);
  
  Buffer* b;
  EINA_INARRAY_FOREACH(l->mesh->buffers, b) {
    b->need_resend = true;
  }
}

void
line_add_box(LineComponent* l, AABox box, Vec4 color)
{
  //printf("begin addbox : %d \n", l->mesh->vertices->len);

  //TODO
  Vec3 min = box.min;
  Vec3 max = box.max;

  Vec3 p1, p2;
  
  p1 = vec3(min.x, min.y, max.z);
  p2 = vec3(max.x, min.y, max.z);
  line_add_color(l, p1, p2, color);

  p1 = vec3(min.x, max.y, max.z);
  p2 = vec3(max.x, max.y, max.z);
  line_add_color(l, p1, p2, color);

  p1 = vec3(min.x, min.y, max.z);
  p2 = vec3(min.x, max.y, max.z);
  line_add_color(l, p1, p2, color);

  p1 = vec3(max.x, min.y, max.z);
  p2 = vec3(max.x, max.y, max.z);
  line_add_color(l, p1, p2, color);

  ////////////////

  p1 = vec3(min.x, min.y, min.z);
  p2 = vec3(max.x, min.y, min.z);
  line_add_color(l, p1, p2, color);

  p1 = vec3(min.x, max.y, min.z);
  p2 = vec3(max.x, max.y, min.z);
  line_add_color(l, p1, p2, color);

  p1 = vec3(min.x, min.y, min.z);
  p2 = vec3(min.x, max.y, min.z);
  line_add_color(l, p1, p2, color);

  p1 = vec3(max.x, min.y, min.z);
  p2 = vec3(max.x, max.y, min.z);
  line_add_color(l, p1, p2, color);

  /////////////////////////

  p1 = vec3(min.x, min.y, min.z);
  p2 = vec3(min.x, min.y, max.z);
  line_add_color(l, p1, p2, color);

  p1 = vec3(min.x, max.y, min.z);
  p2 = vec3(min.x, max.y, max.z);
  line_add_color(l, p1, p2, color);

  p1 = vec3(max.x, min.y, min.z);
  p2 = vec3(max.x, min.y, max.z);
  line_add_color(l, p1, p2, color);

  p1 = vec3(max.x, max.y, min.z);
  p2 = vec3(max.x, max.y, max.z);
  line_add_color(l, p1, p2, color);

  //printf("finish addbox : %d \n", l->mesh->vertices->len);
}


