#include "dragger.h"
#include "property.h"
#include "component.h"
#include "object.h"
#include "resource.h"

static void*
_dragger_create()
{
  Dragger* d = calloc(1, sizeof *d);
  //d->line = create_line();
  return d;
}


static void
_dragger_init(Component* component)
{
  Dragger* d = component->data;
  dragger_state_set(d, DRAGGER_IDLE);
}

static double _resize_to_cam(Matrix4 world, const Matrix4 projection, double factor)
{
  Matrix4 tm;
  mat4_multiply(projection, world, tm);
  mat4_transpose(tm, tm);

  Vec4 zero = vec4(0,0,0,1);
  Vec4 vw = mat4_vec4_mul(tm, zero);
  double w = vw.w * factor;
  return w;
}

static void
_dragger_draw_edit(Component* comp, Matrix4 world, const Matrix4 projection)
{
  Dragger* d = comp->data;

  /*
  Line* l = d->line;
  line_set_use_depth(l, false);
  line_set_size_fixed(l, false);
  line_clear(l);
  */
  AABox bb = d->box;
  double w = _resize_to_cam(world, projection, 0.05);
  bb.max = vec3_mul(bb.max, w);
  bb.min = vec3_mul(bb.min, w);
  d->scale = w;


  /*
  line_add_box(l, bb, vec4(0,1,0,1));
  line_resend(l);

  //TODO draw the line
  //TODO remove this function later.
  line_prepare_draw(l, world, projection);
  line_draw(l);
  */
}

ComponentDesc* dragger_desc()
{
  ComponentDesc* cd = calloc(1,sizeof *cd);
  cd->name = "dragger";
  cd->create = _dragger_create;
  //cd->properties 
  cd->init = _dragger_init,
  cd->draw_edit = _dragger_draw_edit;

  return cd;
};


void
dragger_highlight_set(Dragger* d, bool highlight)
{
  if (!d->mc) {
    EINA_LOG_ERR("no mesh component");
    return;
  }

  Vec4* v = shader_instance_uniform_data_get(d->mc->shader_instance, "color");
  if (!v) return;

  if (highlight)
  *v = vec4(0,1,1,1);
  else
  *v = vec4(0,0,1,1);
}

void
dragger_state_set(Dragger* d, DraggerState state)
{
  if (state == d->state) return;

  if (!d->mc) {
    EINA_LOG_ERR("no mesh component");
    return;
  }

  Vec4* v = shader_instance_uniform_data_get(d->mc->shader_instance, "color");
  if (!v) return;

  if (state == DRAGGER_HIGHLIGHT)
  *v = vec4(1,0.97,0,1);
  else if (state == DRAGGER_IDLE)
  *v = d->color_idle;
  else if (state == DRAGGER_SELECTED)
  *v = vec4(1,1,1,1);
  else if (state == DRAGGER_LOWLIGHT) {
    *v = d->color_idle;
    v->w = 0.05f;
  }

  d->mc->hide = state == DRAGGER_HIDE || state == DRAGGER_SHOW_SECOND;

  if (d->mc_second) {
    v = shader_instance_uniform_data_get(d->mc_second->shader_instance, "color");
    if (state == DRAGGER_SHOW_SECOND) {
      *v = vec4(1,1,1,0.05);
      d->mc_second->hide = false;
    }
    else
      d->mc_second->hide = true;
  }

  d->state = state;
}


Object* 
_dragger_rotate_create(Vec3 constraint, Vec4 color, bool plane)
{
  Object* o = create_object();
  o->name = "rotate";
  ///////////
  Component* comp = create_component(component_mesh_desc());
  object_add_component(o, comp);
  MeshComponent* mc = comp->data;
  mesh_component_shader_set_by_name(mc, "shader/dragger.shader");

  mesh_component_mesh_set_by_name(mc, "model/dragger_rotate_quarter.mesh");

  UniformValue* uv = calloc(1, sizeof *uv);
  uv->type = UNIFORM_VEC4;
  shader_instance_uniform_data_set(mc->shader_instance, "color", uv);

  /////////

  comp = create_component(component_mesh_desc());
  object_add_component(o, comp);
  MeshComponent* mcs = comp->data;
  mesh_component_shader_set_by_name(mcs, "shader/dragger.shader");

  mesh_component_mesh_set_by_name(mcs, "model/dragger_rotate_circle.mesh");

  uv = calloc(1, sizeof *uv);
  uv->type = UNIFORM_VEC4;
  shader_instance_uniform_data_set(mcs->shader_instance, "color", uv);

  ////////////////
  comp = create_component(dragger_desc());
  object_add_component(o,comp);
  Dragger* d = comp->data;
  //d->box = mc->mesh->box;
  Mesh* m = mesh_component_mesh_get(mc);
  d->box = m->box;
  d->mc = mc;
  d->mc_second = mcs;
  d->constraint = constraint;
  d->color_idle = color;
  d->type = DRAGGER_ROTATE;
  //d->collider = resource_mesh_get(s_rm, "model/dragger_rotate_collider.mesh");
  d->collider = resource_mesh_get(s_rm, "model/dragger_rotate_collider_quarter.mesh");
  dragger_state_set(d, DRAGGER_IDLE);

  return o;
}


Object* 
_dragger_scale_create(Vec3 constraint, Vec4 color, bool plane)
{
  Object* o = create_object();
  o->name = "scale";
  Component* comp = create_component(component_mesh_desc());
  object_add_component(o, comp);
  MeshComponent* mc = comp->data;
  mesh_component_shader_set_by_name(mc, "shader/dragger.shader");

  if (plane) {
    mesh_component_mesh_set_by_name(mc, "model/dragger_plane.mesh");
  }
  else {
    mesh_component_mesh_set_by_name(mc, "model/dragger_scale.mesh");
  }

  UniformValue* uv = calloc(1, sizeof *uv);
  uv->type = UNIFORM_VEC4;
  shader_instance_uniform_data_set(mc->shader_instance, "color", uv);


  comp = create_component(dragger_desc());
  object_add_component(o, comp);
  Dragger* d = comp->data;
  //d->box = mc->mesh->box;
  Mesh* m = mesh_component_mesh_get(mc);
  d->box = m->box;
  d->mc = mc;
  d->constraint = constraint;
  d->color_idle = color;
  d->type = DRAGGER_SCALE;
  dragger_state_set(d, DRAGGER_IDLE);

  return o;
}

Object* 
_dragger_translate_create(Vec3 constraint, Vec4 color, bool plane)
{
  Object* o = create_object();
  o->name = "translate";
  Component* comp = create_component(component_mesh_desc());
  object_add_component(o, comp);
  MeshComponent* mc = comp->data;
  mesh_component_shader_set_by_name(mc, "shader/dragger.shader");

  if (plane) {
    mesh_component_mesh_set_by_name(mc, "model/dragger_plane.mesh");
  }
  else {
    mesh_component_mesh_set_by_name(mc, "model/dragger_arrow.mesh");
  }

  UniformValue* uv = calloc(1, sizeof *uv);
  uv->type = UNIFORM_VEC4;
  shader_instance_uniform_data_set(mc->shader_instance, "color", uv);

  /*
  comp = create_component(&line_desc);
  object_add_component(o,comp);
  Line* l = comp->data;
  l->camera = camera;
  AABox b = mc->mesh->box;
  line_add_box(l, b, vec4(0,1,0,1));
  line_set_use_depth(l, false);
  line_set_size_fixed(l, true);
  */

  comp = create_component(dragger_desc());
  object_add_component(o,comp);
  Dragger* d = comp->data;
  //d->line->camera = camera;
  Mesh* m = mesh_component_mesh_get(mc);
  d->box = m->box;
  d->mc = mc;
  d->constraint = constraint;
  d->color_idle = color;
  d->type = DRAGGER_TRANSLATE;
  dragger_state_set(d, DRAGGER_IDLE);

  return o;
}

