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
  bb.Max = vec3_mul(bb.Max, w);
  bb.Min = vec3_mul(bb.Min, w);
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
    printf("no mesh component\n");
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
    printf("no mesh component\n");
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
  ///////////
  Component* comp = create_component(&mesh_desc);
  object_add_component(o, comp);
  MeshComponent* mc = comp->data;
  mesh_component_shader_set(mc, "shader/dragger.shader");

  //mc->mesh_name = "model/dragger_rotate_half.mesh";
  //mc->mesh_name = "model/dragger_rotate_test.mesh";
  mc->mesh_name = "model/dragger_rotate_quarter.mesh";
  mc->mesh = resource_mesh_get(s_rm, mc->mesh_name);

  Vec4* v = calloc(1, sizeof *v);
  shader_instance_uniform_data_set(mc->shader_instance, "color", v);

  /////////

  comp = create_component(&mesh_desc);
  object_add_component(o, comp);
  MeshComponent* mcs = comp->data;
  mesh_component_shader_set(mcs, "shader/dragger.shader");

  mcs->mesh_name = "model/dragger_rotate_circle.mesh";
  mcs->mesh = resource_mesh_get(s_rm, mcs->mesh_name);

  v = calloc(1, sizeof *v);
  shader_instance_uniform_data_set(mcs->shader_instance, "color", v);

  ////////////////
  comp = create_component(dragger_desc());
  object_add_component(o,comp);
  Dragger* d = comp->data;
  d->box = mc->mesh->box;
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
  Component* comp = create_component(&mesh_desc);
  object_add_component(o, comp);
  MeshComponent* mc = comp->data;
  mesh_component_shader_set(mc, "shader/dragger.shader");

  if (plane) {
    mc->mesh_name = "model/dragger_plane.mesh";
    mc->mesh = resource_mesh_get(s_rm, mc->mesh_name);
  }
  else {
    mc->mesh_name = "model/dragger_scale.mesh";
    mc->mesh = resource_mesh_get(s_rm, mc->mesh_name);
  }

  Vec4* v = calloc(1, sizeof *v);
  shader_instance_uniform_data_set(mc->shader_instance, "color", v);


  comp = create_component(dragger_desc());
  object_add_component(o,comp);
  Dragger* d = comp->data;
  d->box = mc->mesh->box;
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
  Component* comp = create_component(&mesh_desc);
  object_add_component(o, comp);
  MeshComponent* mc = comp->data;
  mesh_component_shader_set(mc, "shader/dragger.shader");

  if (plane) {
    mc->mesh_name = "model/dragger_plane.mesh";
    mc->mesh = resource_mesh_get(s_rm, mc->mesh_name);
  }
  else {
    mc->mesh_name = "model/dragger_arrow.mesh";
    mc->mesh = resource_mesh_get(s_rm, mc->mesh_name);
  }

  Vec4* v = calloc(1, sizeof *v);
  shader_instance_uniform_data_set(mc->shader_instance, "color", v);

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
  d->box = mc->mesh->box;
  d->mc = mc;
  d->constraint = constraint;
  d->color_idle = color;
  d->type = DRAGGER_TRANSLATE;
  dragger_state_set(d, DRAGGER_IDLE);

  return o;
}

