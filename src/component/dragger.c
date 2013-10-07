#include "dragger.h"
#include "property.h"
#include "component.h"
#include "object.h"

static void*
_dragger_create()
{
  Dragger* d = calloc(1, sizeof *d);
  d->line = create_line();
  d->color_wanted = vec4(0,0,1,1);
  return d;
}


static void
_dragger_init(Component* component)
{
  //Dragger* d = component->data;
}

static double _resize_to_cam(Matrix4 world, Matrix4 projection, double factor)
{
  Matrix4 tm;
  mat4_multiply(projection, world, tm);
  mat4_transpose(tm, tm);

  Vec4 zero = vec4(0,0,0,1);
  Vec4 vw = mat4_vec4_mul(tm, zero);
  double w = vw.W * factor;
  return w;
}

static void
_dragger_draw_edit(Component* comp, Matrix4 world, Matrix4 projection)
{
  Dragger* d = comp->data;

  Line* l = d->line;
  line_set_use_depth(l, false);
  line_set_size_fixed(l, false);
  line_clear(l);
  AABox bb = d->box;
  double w = _resize_to_cam(world, projection, 0.1);
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

  /*
  if (!vec4_equal(d->color,d->color_wanted)) {
    d->color = d->color_wanted;
    Object* o = comp->object;
    MeshComponent* mc = object_component_get(o, "mesh");
    if (!mc) {
      printf("no mesh component\n");
      return;
    }

    Shader* s = mc->shader;
    GLint color_location = shader_uniform_location_get(s, "color");
    if (color_location >= 0)
    gl->glUniform4f(color_location, d->color.X,d->color.Y,d->color.Z, d->color.W);
    else
    printf("no such uniform \n");
  }
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
  /*
  if (highlight)
  d->color_wanted = vec4(0,1,1,1);
  else
  d->color_wanted = vec4(0,0,1,1);
  */

  if (!d->mc) {
    printf("no mesh component\n");
    return;
  }

  //Vec4* v = calloc(1, sizeof *v);
  Vec4* v = mesh_component_shader_uniform_data_get(d->mc, "color");
  if (!v) return;

  if (highlight)
  *v = vec4(0,1,1,1);
  else
  *v = vec4(0,0,1,1);


}
