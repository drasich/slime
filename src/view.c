#include <Elementary.h>
#include "view.h"
#include "component/mesh.h"
#include "object.h"
#include "scene.h"
#include "gl.h"
#include "context.h"
#include "control.h"
#include "ui/property_view.h"
#include "ui/tree.h"
#include "intersect.h"
#include "glview.h"
#include "gameview.h"
#include "texture.h"
#define __UNUSED__

static bool s_view_destroyed = false;

// Callbacks
static void
_view_init_gl(Evas_Object *obj)
{
  //gl = elm_glview_gl_api_get(obj);
  View* v = evas_object_data_get(obj, "view");
  v->render = create_render();

  gl->glEnable(GL_DEPTH_TEST);
  gl->glEnable(GL_STENCIL_TEST);
  gl->glDepthFunc(GL_LEQUAL);
  gl->glClearDepthf(1.0f);
  gl->glClearStencil(0);
}

static void
_view_del_gl(Evas_Object *obj)
{
}

static void
_view_resize_gl(Evas_Object *obj)
{
  int w, h;
  elm_glview_size_get(obj, &w, &h);

  // GL Viewport stuff. you can avoid doing this if viewport is all the
  // same as last frame if you want
  gl->glViewport(0, 0, w, h);

  View* v = evas_object_data_get(obj, "view");
  ccamera_set_resolution(v->camera->camera_component, w, h);


  //quad_resize(v->render->quad_outline->mesh, w, h);
  //quad_resize(v->render->quad_color->mesh, w, h);
  
  MeshComponent* mc = object_component_get(v->render->quad_outline, "mesh");
  shader_use(mc->shader);

  GLint uni_resolution = shader_uniform_location_get(mc->shader, "resolution");
  if (uni_resolution >= 0) gl->glUniform2f(uni_resolution, w, h);

  quad_resize(mc->mesh, w, h);

  /*
  mc = object_component_get(v->render->quad_color, "mesh");
  shader_use(mc->shader);
  gl->glUniform2f(mc->mesh->uniform_resolution, w, h);
  quad_resize(mc->mesh, w, h);
  */

  //TODO
  fbo_resize(v->render->fbo_all, w, h);
  fbo_resize(v->render->fbo_selected, w, h);
}

static void
_view_draw_gl(Evas_Object *obj)
{
  int w, h;

  elm_glview_size_get(obj, &w, &h);

  gl->glViewport(0, 0, w, h);
  //gl->glClearColor(1.0,0.8,0.3,1);
  gl->glClearColor(0.2,0.2,0.2,1);
  gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Draw a Triangle
  gl->glEnable(GL_BLEND);
  gl->glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //TODO remove the function update here
  /*
  Scene* s = evas_object_data_get(obj, "scene");
  if (s)
  scene_update(s);
  */

  View* v = evas_object_data_get(obj, "view");
  view_update(v,0);
  view_draw(v);

   gl->glFinish();
}



static void
_key_down(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info)
{
  Evas_Event_Key_Down *ev = (Evas_Event_Key_Down*)event_info;
  printf("KEY: down, keyname: %s , key %s \n", ev->keyname, ev->key);

  View* v = evas_object_data_get(o, "view");
  Control* cl = v->control;
  control_key_down(cl, ev);
}

static float startx = 0, starty = 0;

/*
static void frustum_from_rect(
      Frustum* f, 
      Camera* c, 
      float left,
      float top,
      float width,
      float height)
{
  Vec3 camdir = quat_rotate_vec3(c->object.Orientation, vec3(0,0,-1));
  Vec3 camup = quat_rotate_vec3(c->object.Orientation, vec3(0,1,0));
  Vec3 camright = vec3_cross(camdir, camup);
  double cam_height = tan(c->fovy/2.0)*c->near*2.0f;
  double cam_width = cam_height* c->aspect;

  float width_ratio = cam_width/c->width;
  float height_ratio = cam_height/c->height;

  double cx = -c->width + left + width/2.0f;
  cx *= width_ratio;
  double cy = c->height - top - height/2.0f;
  cy *= height_ratio;

  Vec3 ccenter = vec3_add(c->object.Position, vec3_mul(camdir, c->near));
  Vec3 center = vec3_add(ccenter, vec3_mul(camright, cx));
  center = vec3_add(center, vec3_mul(camup, cy));
  
  Vec3 dir = vec3_sub(center, c->object.Position);
  dir = vec3_normalized(dir);
  Quat qq = quat_between_vec(ccenter, center);
  qq = quat_mul(c->object.Orientation, qq);

  frustum_set(
        f,
        c->near, 
        c->far,
        c->object.Position, 
        quat_rotate_vec3(c->object.Orientation, vec3(0,0,-1)),
        quat_rotate_vec3(c->object.Orientation, vec3(0,1,0)),
        c->fovy,
        c->aspect);

}
*/

static void _handle_rect_select(View* v, Evas_Event_Mouse_Move* ev)
{
  Evas_Object* rect = v->select_rect;

  int yepx = ev->cur.canvas.x - startx;
  int yepy = ev->cur.canvas.y - starty;

  int sx = startx;
  int sy = starty;
  if (yepx <0) {
    sx = ev->cur.canvas.x;
    yepx *= -1;
  }
  if (yepy <0) {
    sy = ev->cur.canvas.y;
    yepy *= -1;
  }

  evas_object_move(rect, sx, sy);
  evas_object_resize(rect, yepx, yepy);


  ViewCamera* c = v->camera;
  //Frustum f;
  //frustum_from_rect(&f, c, sx, sy, yepx, yepy);

  Plane planes[6];
  camera_get_frustum_planes_rect(c, planes, sx, sy, yepx, yepy );

  Render* r = v->render;

  Eina_List* newlist = NULL;
  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(r->objects, l, o) {
    if (planes_is_in_object(planes, 6, o)) {
      newlist = eina_list_append(newlist, o);
    }
  }

  context_objects_set(v->context, newlist);
  /*
  bool b = frustum_is_in(&f, o->Position);
  if (!b) continue;
      */
}

static void
_mouse_move(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o, void *event_info)
{
  //elm_object_focus_set(o, EINA_TRUE);
  Evas_Event_Mouse_Move *ev = (Evas_Event_Mouse_Move*) event_info;

  View* v = evas_object_data_get(o, "view");

  const Evas_Modifier * mods = ev->modifiers;
  if ( evas_key_modifier_is_set(mods, "Control") &&
        (ev->buttons & 1) != 0 ) {
    _handle_rect_select(v,ev);
    return;
  }

  Control* cl = v->control;
  control_mouse_move(cl, ev);
}

static void
_mouse_in(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o, void *event_info)
{
  elm_object_focus_set(o, EINA_TRUE);
}


static void
_view_select_object(View *v, Object *o)
{
  context_objects_clean(v->context);
  context_object_add(v->context, o);
}

static void
_makeRect(View* v, Evas_Event_Mouse_Down* ev)
{
  printf("make rect : \n");

  Evas_Object* r = v->select_rect;

  evas_object_resize(r, 1,1);
  evas_object_move(r, ev->canvas.x, ev->canvas.y);
  startx = ev->canvas.x;
  starty = ev->canvas.y;
  evas_object_show(r);
}

static Evas_Object*
_context_menu_create(Evas_Object* win, View* v)
{
  Evas_Object* menu;
  Elm_Object_Item *menu_it,*menu_it1;

  menu = elm_menu_add(win);
  evas_object_data_set(menu, "view", v);

  Context* c = v->context;
  Eina_List* objects = context_objects_get(c);
  int count = eina_list_count(objects);
  if (count == 0) {
    elm_menu_item_add(menu, NULL, NULL, "Add empty", NULL, NULL);
  }
  else {
    if (count == 1) {
    }
    else if (count >1) {
      elm_menu_item_add(menu, NULL, NULL, "Set parent", NULL, NULL);
    }
    elm_menu_item_add(menu, NULL, NULL, "Duplicate", NULL, NULL);
    elm_menu_item_add(menu, NULL, NULL, "Remove", NULL, NULL);
    elm_menu_item_add(menu, NULL, NULL, "Add Component", NULL, NULL);
  }



  /*
  Eina_List* l;
  ComponentDesc* c;
  EINA_LIST_FOREACH(components, l, c) {
    printf("component name : %s\n", c->name);
    elm_menu_item_add(menu, NULL, NULL, c->name, _addcomp, c);
  }
  */
  return menu;
}

static void
_mouse_down(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o, void *event_info)
{
  Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down*) event_info;
  //elm_object_focus_set(o, EINA_TRUE);

  View* v = evas_object_data_get(o, "view");
  Scene* s = v->context->scene;
  Control* cl = v->control;
  if (control_mouse_down(cl, ev))
    return;

  //if (ev->button == 3 ){
  const Evas_Modifier * mods = ev->modifiers;
  if ( ev->button == 1 && evas_key_modifier_is_set(mods, "Control")) {
    _makeRect(v, ev);
    return;
  }

  if (ev->button == 3) {
    Evas_Object* win = evas_object_top_get (evas_object_evas_get(o));
    Evas_Object* menu = _context_menu_create(win, v);
    evas_object_show(menu);
    elm_menu_move(menu, ev->canvas.x, ev->canvas.y);
    return;
  }

  if (ev->button != 1){
    return;
  }
  else printf("click \n");

  Ray r = ray_from_screen(v->camera, ev->canvas.x, ev->canvas.y, 1000);

  bool found = false;
  double d;
  Object* clicked = NULL;

  Eina_List *list;
  //Object *ob;
  RenderObject *ro;
  //EINA_LIST_FOREACH(s->objects, list, ob) {
  EINA_LIST_FOREACH(v->render->render_objects, list, ro) {
    //IntersectionRay ir = intersection_ray_object(r, ob);
    IntersectionRay ir = intersection_ray_object(r, ro->object);
    
    if (ir.hit) {
      double diff = vec3_length2(vec3_sub(ir.position, v->camera->object->Position));
      if ( (found && diff < d) || !found) {
        found = true;
        d = diff;
        //clicked = ob;
        clicked = ro->object;
      }
    }
  }

  if (clicked != NULL) {
    _view_select_object(v, clicked);
  }

}

static void
_mouse_up(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o, void *event_info)
{
  Evas_Event_Mouse_Up *ev = (Evas_Event_Mouse_Up*)event_info;
  //if (ev->button != 1) return;
  //printf("MOUSE: up   @ %4i %4i\n", ev->canvas.x, ev->canvas.y);
  //evas_object_hide(indicator[0]);
  //
  View* v = evas_object_data_get(o, "view");
  Evas_Object* rect = v->select_rect;
  evas_object_hide(rect);

  Control* cl = v->control;
  if (control_mouse_up(cl, ev))
  return;
}

static void
_mouse_wheel(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o, void *event_info)
{
  Evas_Event_Mouse_Wheel *ev = (Evas_Event_Mouse_Wheel*) event_info;

  View* v = evas_object_data_get(o, "view");
  //float x = ev->cur.canvas.x - ev->prev.canvas.x;
  //float y = ev->cur.canvas.y - ev->prev.canvas.y;
  Vec3 axis = {0, 0, ev->z};
  axis = vec3_mul(axis, 1.5f);
  camera_pan(v->camera, axis);
}

static Object* 
_create_repere(float u, Camera* camera)
{
  Object* o = create_object();
  Component* comp = create_component(&line_desc);
  object_add_component(o,comp);
  Line* l = comp->data;
  l->camera = camera;
  line_add_color(l, vec3(0,0,0), vec3(u,0,0), vec4(1,0,0,1));
  line_add_color(l, vec3(0,0,0), vec3(0,u,0), vec4(0,1,0,1));
  line_add_color(l, vec3(0,0,0), vec3(0,0,u), vec4(0,0,1,1));
  line_set_use_depth(l, false);
  return o;
}

#include "resource.h"
#include "component/dragger.h"
typedef Object* (*dragger_create_fn)(Vec3 constraint, Vec4 color, bool plane);

static Object* 
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

  object_add_component(o, comp);

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

static Object* 
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

  object_add_component(o, comp);

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

static Object* 
_dragger_rotate_create(Vec3 constraint, Vec4 color, bool plane)
{
  Object* o = create_object();
  Component* comp = create_component(&mesh_desc);
  object_add_component(o, comp);
  MeshComponent* mc = comp->data;
  mesh_component_shader_set(mc, "shader/dragger.shader");

  //mc->mesh_name = "model/dragger_rotate_half.mesh";
  mc->mesh_name = "model/dragger_rotate_test.mesh";
  mc->mesh = resource_mesh_get(s_rm, mc->mesh_name);

  Vec4* v = calloc(1, sizeof *v);
  shader_instance_uniform_data_set(mc->shader_instance, "color", v);

  object_add_component(o, comp);

  comp = create_component(dragger_desc());
  object_add_component(o,comp);
  Dragger* d = comp->data;
  d->box = mc->mesh->box;
  d->mc = mc;
  d->constraint = constraint;
  d->color_idle = color;
  d->type = DRAGGER_ROTATE;
  d->collider = resource_mesh_get(s_rm, "model/dragger_rotate_collider.mesh");
  dragger_state_set(d, DRAGGER_IDLE);

  return o;
}




static Object* 
_create_grid(Camera* camera)
{
  Object* grid = create_object();
  Component* comp = create_component(&line_desc);
  object_add_component(grid,comp);
  Line* l = comp->data;
  l->camera = camera;

  line_add_grid(l, 100, 10);
  return grid;
}

static void
_file_chosen(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
  printf("todo %s %s\n",  __FILE__, __LINE__);
  /*
  const char *file = event_info;
  View* v = data;
  if (file)
   {
    Object* yep = create_object_file(file);
    const char *slash = strrchr(file, '/');
    const char *dot = strrchr(file, '.');
    if (slash && dot) {
      int size = dot - (slash+1);
      char* dup = strndup(slash+1, size);
      yep->name = eina_stringshare_add(dup);
      free(dup);
    }
    else
    yep->name = eina_stringshare_add("new_object");

    yep->mesh->shader = v->control->shader_simple;

    Vec3 cp = v->camera->object->Position;
    Vec3 direction = quat_rotate_vec3(v->camera->object->Orientation, vec3(0,0,-1));
    cp = vec3_add(
          cp,
          vec3_mul(direction, 30));
    
    object_set_position(yep, cp);
    control_object_add(v->control, v->context->scene, yep);

    context_objects_clean(v->context);
    context_object_add(v->context, yep);
   }
   */
}


static void
_new_empty(void *data,
      Evas_Object *obj,
      void *event_info)
{
  Object* yep = create_object();
  yep->name = eina_stringshare_add("empty");
  View* v = (View*) data;

  Vec3 cp = v->camera->object->Position;
  Vec3 direction = quat_rotate_vec3(v->camera->object->Orientation, vec3(0,0,-1));
  cp = vec3_add(
        cp,
        vec3_mul(direction, 30));

  object_set_position(yep, cp);
  control_object_add(v->control, v->context->scene, yep);

  context_objects_clean(v->context);
  context_object_add(v->context, yep);

}

static void
_gameview_closed(void *data, Evas_Object *obj, void *event_info)
{
  printf("gameview closed\n");
  if (s_view_destroyed) return;

  View* v = data;
  Context* context = v->context;

  scene_destroy(context->scene);
  Scene* s = scene_read("scenecur.eet");
  scene_post_read(s);

  context_objects_clean(context);
  context->scene = s;
}


Evas_Object* gameview_;

static void
_play(void *data,
      Evas_Object *obj,
      void *event_info)
{
  View* v = data;

  if (!gameview_) {
    scene_write(v->context->scene, "scenecur.eet");

    gameview_ = create_gameview_window(v->context->scene, &gameview_, v->control );
    evas_object_smart_callback_add(gameview_, "delete,request", _gameview_closed, v);
  }
  else {
    evas_object_show(gameview_);
    elm_win_raise(gameview_);
  }
  //elm_win_borderless_set(gameview_, EINA_TRUE);
}

static void
_pause(void *data,
      Evas_Object *obj,
      void *event_info)
{
  printf("pause\n");
}

static void
_reload(void *data,
      Evas_Object *obj,
      void *event_info)
{
  if (gameview_) return; //TODO if state is play

  View* v = data;
  scene_write(v->context->scene, "scenetmp.eet");
  context_objects_clean(v->context);
  scene_destroy(v->context->scene);
  scene_descriptor_delete();
  component_manager_unload(s_component_manager);
  component_manager_load(s_component_manager);
  scene_descriptor_init();
  Scene* s = scene_read("scenetmp.eet");
  scene_post_read(s);
  v->context->scene = s;
  scene_print(s);
}


/*
static void
_show(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Event_Mouse_Down *ev = event_info;
  elm_menu_move(data, ev->canvas.x, ev->canvas.y);
  evas_object_show(data);
}
*/

static void
_addcomp(void *data,
      Evas_Object *obj,
      void *event_info)
{
  View* v = evas_object_data_get(obj, "view");
  ComponentDesc* cd = data;

  Object* o = context_object_get(v->context);
  if (o) {
    //object_add_component(v->context->object, c); //TODO
    //
    Component* c = create_component(cd);
    control_component_add(v->control, o, c);
  }
        //find the object currently selected
}

static Evas_Object*
_create_component_menu(Evas_Object* win, Eina_List* components)
{
  Evas_Object* menu;
  Elm_Object_Item *menu_it,*menu_it1;

  menu = elm_menu_add(win);

  Eina_List* l;
  ComponentDesc* c;
  EINA_LIST_FOREACH(components, l, c) {
    printf("component name : %s\n", c->name);
    elm_menu_item_add(menu, NULL, NULL, c->name, _addcomp, c);
  }

  /*
  elm_menu_item_add(menu, NULL, NULL, "component name 1", NULL, NULL);
  menu_it = elm_menu_item_add(menu, NULL, NULL, "second item", NULL, NULL);
  //elm_menu_item_separator_add(menu, menu_it);
  elm_menu_item_add(menu, NULL, NULL, "third item", NULL, NULL);
  elm_menu_item_add(menu, NULL, NULL, "fourth item", NULL, NULL);
  elm_menu_item_add(menu, NULL, NULL, "sub menu", NULL, NULL);

  menu_it = elm_menu_item_add(menu, NULL, NULL, "third item", NULL, NULL);
  //elm_object_item_disabled_set(menu_it, EINA_TRUE);

  //evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_DOWN, _show, menu);
  */
  return menu;
}

static void
_addcomponent(void *data,
      Evas_Object *obj,
      void *event_info)
{
  View* v = data;

  printf("add component\n");
  Evas_Object* win = evas_object_top_get (evas_object_evas_get(obj));
  //Evas_Object* menu = v->menu;
  Evas_Object* menu = _create_component_menu(win, s_component_manager->components);
  evas_object_data_set(menu, "view", v);
  evas_object_show(menu);

  Evas_Coord x,y,w,h;
  evas_object_geometry_get(obj, &x, &y, &w, &h);
  elm_menu_move(menu, x, y);
}


static void
_add_buttons(View* v, Evas_Object* win)
{
  Evas_Object* fs_bt, *ic, *bt;

  ic = elm_icon_add(win);
  elm_icon_standard_set(ic, "file");
  evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);

  fs_bt = elm_fileselector_button_add(win);
  elm_object_focus_allow_set(fs_bt, 0);
  elm_fileselector_button_path_set(fs_bt, "/home/chris/code/slime/model");
  elm_object_text_set(fs_bt, "Add object");
  elm_object_part_content_set(fs_bt, "icon", ic);
  elm_fileselector_button_expandable_set(fs_bt, EINA_TRUE);
  //elm_fileselector_mode_set(fs_bt, ELM_FILESELECTOR_LIST);
  elm_fileselector_button_inwin_mode_set(fs_bt, EINA_TRUE);


  //elm_box_pack_end(vbox, fs_bt);
  evas_object_show(fs_bt);
  evas_object_show(ic);

  int r,g,b,a;
  evas_object_color_get(fs_bt, &r,&g,&b,&a);
  a = 150;
  evas_object_color_set(fs_bt, r,g,b,a);
  evas_object_color_set(ic, r,g,b,a);
  evas_object_resize(fs_bt, 100, 25);
  evas_object_move(fs_bt, 15, 15);

  evas_object_smart_callback_add(fs_bt, "file,chosen", _file_chosen, v);


  bt = elm_button_add(win);
  elm_object_focus_allow_set(bt, 0);

  elm_object_text_set(bt, "Add empty");
  evas_object_show(bt);

  evas_object_color_set(bt, r,g,b,a);
  evas_object_resize(bt, 100, 25);
  evas_object_move(bt, 15, 45);
  evas_object_data_set(bt, "view", v);
  evas_object_smart_callback_add(bt, "clicked", _new_empty, v);
  //view->addObjectToHide(bt);
  //view->addObjectToHide(fs_bt);


  bt = elm_button_add(win);
  elm_object_focus_allow_set(bt, 0);

  elm_object_text_set(bt, "play");
  evas_object_show(bt);

  evas_object_color_set(bt, r,g,b,a);
  evas_object_resize(bt, 50, 25);
  evas_object_move(bt, 155, 15);
  evas_object_data_set(bt, "view", v);
  evas_object_smart_callback_add(bt, "clicked", _play, v);

  ///////////////////////////

  bt = elm_button_add(win);
  elm_object_focus_allow_set(bt, 0);

  elm_object_text_set(bt, "pause");
  evas_object_show(bt);

  evas_object_color_set(bt, r,g,b,a);
  evas_object_resize(bt, 50, 25);
  evas_object_move(bt, 205, 15);
  evas_object_data_set(bt, "view", v);
  evas_object_smart_callback_add(bt, "clicked", _pause, v);

  ////////////////////////////////
  bt = elm_button_add(win);
  elm_object_focus_allow_set(bt, 0);

  elm_object_text_set(bt, "add component");
  evas_object_show(bt);

  evas_object_color_set(bt, r,g,b,a);
  evas_object_resize(bt, 100, 25);
  evas_object_move(bt, 275, 15);
  evas_object_data_set(bt, "view", v);
  evas_object_smart_callback_add(bt, "clicked", _addcomponent, v);

  ////////////////////////////////
  bt = elm_button_add(win);
  elm_object_focus_allow_set(bt, 0);

  elm_object_text_set(bt, "reload");
  evas_object_show(bt);

  evas_object_color_set(bt, r,g,b,a);
  evas_object_resize(bt, 50, 25);
  evas_object_move(bt, 405, 15);
  evas_object_data_set(bt, "view", v);
  evas_object_smart_callback_add(bt, "clicked", _reload, v);

}

static Eina_List*
_view_draggers_create(dragger_create_fn dfn, bool create_plane )
{
  Eina_List* draggers = NULL;

  Vec4 red = vec4(1.0,0.247,0.188,1);
  Vec4 green = vec4(0.2117,0.949,0.4156,1);
  Vec4 blue = vec4(0,0.4745,1,1);

  Object* dragger;

  dragger = dfn(
        vec3(1,0,0),
        red,
        false);
  dragger->angles.Y = -90;
  //must be the component
  Dragger* dc = object_component_get(dragger, "dragger");
  dc->ori = quat_angles_deg(-90,0,0);
  draggers = eina_list_append(draggers, dragger);

  dragger = dfn(
        vec3(0,1,0),
        green,
        false);
  dragger->angles.X = 90;
  dc = object_component_get(dragger, "dragger");
  dc->ori = quat_angles_deg(0, 90,0);
  draggers = eina_list_append(draggers, dragger);

  dragger = dfn(
        vec3(0,0,1),
        blue,
        false);
  dc = object_component_get(dragger, "dragger");
  dc->ori = quat_identity();
  draggers = eina_list_append(draggers, dragger);

  if (!create_plane) return;

  red.W = 0.1f;
  green.W = 0.1f;
  blue.W = 0.1f;

  dragger = dfn(
        vec3(0,1,1),
        blue,
        true);
  dc = object_component_get(dragger, "dragger");
  dc->ori = quat_identity();
  draggers = eina_list_append(draggers, dragger);

  dragger = dfn(
        vec3(1,1,0),
        red,
        true);
  dragger->angles.Y = -90;
  dc = object_component_get(dragger, "dragger");
  dc->ori = quat_angles_deg(-90, 0,0);
  draggers = eina_list_append(draggers, dragger);

  dragger = dfn(
        vec3(1,0,1),
        green,
        true);
  dragger->angles.Z = 90;
  dc = object_component_get(dragger, "dragger");
  dc->ori = quat_angles_deg(0, 0,90);
  draggers = eina_list_append(draggers, dragger);
  return draggers;
}

static void
_create_view_objects(View* v)
{
  v->camera = view_camera_new();
  Vec3 p = {20,5,20};
  //v->camera->origin = p;
  //v->camera->object.Position = p;
  camera_pan(v->camera, p);
  Vec3 at = {0,0,0};
  camera_lookat(v->camera, at);

  v->repere = _create_repere(1, v->camera->camera_component);
  Line* l = object_component_get(v->repere, "line");
  if (l) line_set_size_fixed(l, true);

  v->dragger_translate = _view_draggers_create(_dragger_translate_create, true);
  v->dragger_scale = _view_draggers_create(_dragger_scale_create, true);
  //v->dragger_rotate = _view_draggers_create(_dragger_rotate_create, false);

  Object* dragger;

  dragger = _dragger_rotate_create(
        vec3(1,0,0),
        vec4(0,0,1,1),
        false);
  v->dragger_rotate = eina_list_append(v->dragger_rotate, dragger);
  v->draggers = v->dragger_translate;


  v->camera_repere = _create_repere(40, v->camera->camera_component);
  v->camera_repere->Position = vec3(10,10, -10);
  v->camera_repere->orientation_type = ORIENTATION_QUAT;

  v->grid = _create_grid(v->camera->camera_component);


  Evas* e = evas_object_evas_get(v->glview);
  Evas_Object* r = evas_object_rectangle_add(e);
  int a = 15;
  evas_object_color_set(r, a, a, a, a);
  evas_object_resize(r, 0,0);
  evas_object_hide(r);
  v->select_rect = r;
}

static void
_set_callbacks(Evas_Object* glview)
{
  //TODO remove this animator
  Ecore_Animator *ani;
  ani = ecore_animator_add(_anim, glview);
  evas_object_data_set(glview, "ani", ani);

  elm_glview_init_func_set(glview, _view_init_gl);
  elm_glview_del_func_set(glview, _view_del_gl);
  elm_glview_resize_func_set(glview, _view_resize_gl);
  elm_glview_render_func_set(glview, _view_draw_gl);

  evas_object_event_callback_add(glview, EVAS_CALLBACK_DEL, _del, glview);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_KEY_DOWN, _key_down, NULL);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_MOUSE_MOVE, _mouse_move, NULL);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_MOUSE_DOWN, _mouse_down, NULL);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_MOUSE_UP, _mouse_up, NULL);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_MOUSE_WHEEL, _mouse_wheel, NULL);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_MOUSE_IN, _mouse_in, NULL);
}

View*
create_view(Evas_Object *win)
{
  View *view = calloc(1,sizeof *view);

  view->context = calloc(1,sizeof *view->context);
  view->control = create_control(view);
  s_component_manager = create_component_manager(win, view->control); //TODO
  component_manager_load(s_component_manager);

  view->box = elm_box_add(win);
  evas_object_size_hint_weight_set(view->box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  //elm_win_resize_object_add(win, view->box);
  evas_object_show(view->box);

  view->glview = _create_glview(win);
  elm_box_pack_end(view->box, view->glview);
  _set_callbacks(view->glview);

  _add_buttons(view, win);

  view->property = create_property(win, view->context, view->control);
  view->tree = tree_widget_new(win, view);
  evas_object_data_set(view->glview, "view", view);

  _create_view_objects(view);
  //view->menu = _create_component_menu(win);

  return view;
}


void
view_destroy(View* v)
{
  printf("destroy view\n");
  //TODO free camera
  //TODO free scene here?
  if (!gameview_) {
    scene_write(v->context->scene, "scene/scene.eet");
  }

  free(v->context);
  //TODO release control
  //free(v->control);
  free(v);
  s_view_destroyed = true;
}

static void
_render_object_add(View* v, Object* o, Matrix4 root)
{
  RenderObject* ro = calloc(1, sizeof *ro);
  ro->object = o;
  object_compute_matrix(o, ro->world);
  mat4_multiply(root, ro->world, ro->world);
  v->render->render_objects = eina_list_append(v->render->render_objects, ro);
  if (context_object_contains(v->context, o))
  v->render->render_objects_selected = eina_list_append(v->render->render_objects_selected, ro);
}

static void
_render_objects_add(View* v, Matrix4 root, Plane* planes, Eina_List* objects)
{
  Eina_List* l;
  Object* o;
  EINA_LIST_FOREACH(objects, l, o) {
    MeshComponent* mc = object_component_get(o, "mesh");
    if (!mc ) {
      //TODO
      _render_object_add(v, o, root);
      continue;
    }

    Mesh* m = mc->mesh;
    if (!m) {
      _render_object_add(v, o, root);
      continue;
    }

    OBox b;
    aabox_to_obox(m->box, b, o->Position, o->Orientation, o->scale);

    if (planes_is_box_in_allow_false_positives(planes, 6, b)) {
    //if (planes_is_in(planes, 6, o->Position)) {
      _render_object_add(v, o, root);
    }

    Matrix4 world;
    object_compute_matrix(o, world);

    _render_objects_add(v, world, planes, o->children);
  }
}

void
view_update(View* v, double dt)
{
  //TODO check this
  object_update(v->camera->object);

  Context* cx = v->context;
  Render* r = v->render;
  Scene* s = cx->scene;

  Eina_List *l;
  Object *o;

  Plane planes[6];
  camera_get_frustum_planes(v->camera, planes);
  Matrix4 id4;
  mat4_set_identity(id4);
  v->render->render_objects = eina_list_free(v->render->render_objects);
  v->render->render_objects_selected = eina_list_free(v->render->render_objects_selected);
  _render_objects_add(v, id4, planes, s->objects);
  r->objects = eina_list_free(r->objects);

  EINA_LIST_FOREACH(s->objects, l, o) {
    MeshComponent* mc = object_component_get(o, "mesh");
    if (!mc ) {
      //TODO
      r->objects = eina_list_append(r->objects, o);
      continue;
    }

    Mesh* m = mc->mesh;
    if (!m) {
      r->objects = eina_list_append(r->objects, o);
      continue;
    }

    OBox b;
    aabox_to_obox(m->box, b, o->Position, o->Orientation, o->scale);

    if (planes_is_box_in_allow_false_positives(planes, 6, b)) {
    //if (planes_is_in(planes, 6, o->Position)) {
      r->objects = eina_list_append(r->objects, o);
    }
  }
  
}

#include "resource.h"

Render*
create_render()
{
  Render* r = calloc(1, sizeof *r);
  r->fbo_selected = create_fbo();
  r->fbo_all = create_fbo();

  r->quad_outline = create_object();
  Component* comp = create_component(&mesh_desc);
  MeshComponent* mc = comp->data;
  mc->mesh_name = "quad";
  mc->mesh = resource_mesh_get(s_rm, mc->mesh_name);
  r->quad_outline->mesh = mc->mesh;
  object_add_component(r->quad_outline, comp);
  Vec3 t3 = {0,0,-100};
  object_set_position(r->quad_outline, t3);
  r->quad_outline->name = eina_stringshare_add("quad");

  Texture* tsel = texture_new();
  Texture* tall = texture_new();
  eina_hash_add(mc->textures, "texture", tsel);
  eina_hash_add(mc->textures, "texture_all", tall);
  
  texture_fbo_link(tsel, &r->fbo_selected->texture_depth_stencil_id);
  texture_fbo_link(tall, &r->fbo_all->texture_depth_stencil_id);

  mc->shader = create_shader("stencil", "shader/stencil.vert", "shader/stencil.frag");
  shader_attribute_add(mc->shader, "vertex", 3, GL_FLOAT);
  shader_uniform_add(mc->shader, "matrix");
  shader_uniform_add(mc->shader, "resolution");
  shader_uniform_type_add(mc->shader, "texture", UNIFORM_TEXTURE);
  shader_uniform_type_add(mc->shader, "texture_all", UNIFORM_TEXTURE);


  /*
  r->quad_color = create_object();
  comp = create_component(&mesh_desc);
  mc = comp->data;
  mc->name = "quad";
  mc->mesh = resource_mesh_get(s_rm, mc->name);
  object_add_component(r->quad_color, comp);
  r->quad_color->mesh = mc->mesh;
  object_set_position(r->quad_color, t3);
  r->quad_color->name = eina_stringshare_add("quad");

  mc->shader = create_shader("shader/stencil.vert", "shader/quad.frag");
  mc->shader->has_vertex = true;
  mc->shader->has_normal = false;
  mc->shader->has_texcoord = false;
  mc->shader->has_uniform_normal_matrix = false;

  shader_use(mc->shader);
  shader_init_attribute(mc->shader, "vertex", &r->quad_color->mesh->attribute_vertex);
  shader_init_uniform(mc->shader, "matrix", &r->quad_color->mesh->uniform_matrix);
  shader_init_uniform(mc->shader, "resolution", &r->quad_color->mesh->uniform_resolution);
  */

  return r;

}

static void
_object_camera_face(Object* o, ViewCamera* c)
{
  /*
  Quat q = quat_lookat(o->Position, c->object->Position, vec3(0,1,0));
  Vec3 a = quat_to_euler(q);
  a = vec3_mul(a, 180.0f/3.141519f);
  //o->angles = a;
  //o->Orientation = q;
  //printf("angles : %f, %f, %f\n", a.X, a.Y,a.Z);
  */

  Vec3 diff = vec3_sub(o->Position, c->object->Position);
  double dot = vec3_dot(diff, vec3(1,0,0));
  if (dot > 0)
  o->angles.Y = 90;
  else
  o->angles.Y = 0;

  dot = vec3_dot(diff, vec3(0,1,0));
  if (dot > 0)
  o->angles.X = -90;
  else
  o->angles.X = 0;

}

void
view_draw(View* v)
{
  ViewCamera* c = v->camera;
  Object* co = c->object;
  Camera* cc = c->camera_component;
  Context* cx = v->context;
  Render* r = v->render;
  Scene* s = cx->scene;

  Matrix4 cam_mat_inv, mo;
  Matrix4 id4;
  mat4_set_identity(id4);

  mat4_inverse(co->matrix, cam_mat_inv);
  Matrix4* projection = &cc->projection;
  Matrix4* ortho = &cc->orthographic;

  //Render just selected to fbo
  Eina_List *l;
  Object *o;
  Eina_List* cxol = context_objects_get(cx);

  fbo_use(r->fbo_selected);
  gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT) ;
  /*
  EINA_LIST_FOREACH(cxol, l, o) {
    object_draw_edit_component(o, cam_mat_inv, cc->projection , id4, "mesh");
  }
  */

  RenderObject* ro;
  EINA_LIST_FOREACH(r->render_objects_selected, l, ro) {
    object_draw_edit_component2(ro->object, cam_mat_inv, cc->projection, ro->world, "mesh");
  }

  fbo_use_end();


  //Render all objects to fbo to get depth for the lines.
  fbo_use(r->fbo_all);
  gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT) ;
  //gl->glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
  //gl->glClearStencil(0);
  gl->glEnable(GL_STENCIL_TEST);
  gl->glStencilFunc(GL_ALWAYS, 0x1, 0x1);
  gl->glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  /*
  EINA_LIST_FOREACH(r->objects, l, o) {
    object_draw_edit_component(o, cam_mat_inv, cc->projection , id4, "mesh");
  }
  */

  EINA_LIST_FOREACH(r->render_objects, l, ro) {
    object_draw_edit_component2(ro->object, cam_mat_inv, cc->projection, ro->world, "mesh");
  }

  //gl->glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

  //was a test, can be removed
  /*
  int w = c->width;
  int h = c->height;
  printf(" w , h : %d, %d \n", w, h);
  GLuint mypixels[w*h];
  gl->glReadPixels(
        0, 
        0, 
        w, 
        h, 
        GL_DEPTH_STENCIL_OES, 
        GL_UNSIGNED_INT_24_8_OES, 
        mypixels);
  save_png(mypixels, w, h);
  */

  fbo_use_end();

  //draw grid
  gl->glClear(GL_DEPTH_BUFFER_BIT);
  object_compute_matrix(v->grid, mo);
  //TODO 1)find a better/faster way to get a component and set the texture
  //TODO 2)and also we find the first component but there might be more of the same component
  //TODO 3)same below with other objects
  //TODO 4)also object_draw_edit_component is so so
  //Line* line = object_component_get(v->grid, "line");
  //if (line) line->id_texture = r->fbo_all->texture_depth_stencil_id;
  object_draw_edit(v->grid, cam_mat_inv, cc->projection, id4);

  //Render objects
  /*
  EINA_LIST_FOREACH(r->objects, l, o) {
    //Frustum f;
    //camera_get_frustum(v->camera, &f);
    //bool b = frustum_is_in(&f, o->Position);
    //if (!b) continue;
    object_draw_edit(o, cam_mat_inv, cc->projection, id4);
  }
  */

  EINA_LIST_FOREACH(r->render_objects, l, ro) {
    object_draw_edit2(ro->object, cam_mat_inv, cc->projection, ro->world);
  }

  //TODO avoid compute matrix 2 times

  //Render lines only selected
  gl->glClear(GL_DEPTH_BUFFER_BIT);
  Vec3 repere_position = vec3_zero();
  Quat repere_ori = quat_identity();
  EINA_LIST_FOREACH(cxol, l, o) {
    object_compute_matrix(o, mo);
    mat4_multiply(cam_mat_inv, mo, mo);
    Line* line = object_component_get(o, "line");
    if (line) {
      line->id_texture = r->fbo_all->texture_depth_stencil_id;
      object_draw_edit_component(o, cam_mat_inv, cc->projection , id4, "line");
    }
    repere_position = vec3_add(repere_position, object_world_position_get(o));
    repere_ori = quat_mul(repere_ori, object_world_orientation_get(o));
  }

  int cxol_size = eina_list_count(cxol);
  Object* last_obj = NULL;
  if (cxol_size > 0) {
    repere_position = vec3_mul(repere_position, 1.0/(float)cxol_size);
    last_obj = (Object*) eina_list_last(cxol)->data;
  }

  //Render outline with quad
  if (last_obj) {
    object_draw_edit(r->quad_outline, id4, cc->orthographic, id4);
  }

  //repere
  /*
  if (last_obj) {
    gl->glClear(GL_DEPTH_BUFFER_BIT);
    v->repere->Position = repere_position;
    v->repere->angles = last_obj->angles;
    object_compute_matrix(v->repere, mo);
    line = object_component_get(v->repere, "line");
    if (line) line->id_texture = r->fbo_all->texture_depth_stencil_id;
    object_draw_edit(v->repere, cam_mat_inv, cc->projection, id4);
  }
  */

  if (last_obj) {
    Object* dragger;
    Eina_List* l;
    gl->glClear(GL_DEPTH_BUFFER_BIT);
    EINA_LIST_FOREACH(v->draggers, l, dragger){
      dragger->Position = repere_position;
      Dragger* d = object_component_get(dragger, "dragger");
      if (d) {
        if (v->control->dragger_is_local) {
          dragger->Orientation = quat_mul(repere_ori, d->ori);
          dragger->orientation_type = ORIENTATION_QUAT;
        }
      }
      if (d && d->type == DRAGGER_ROTATE) {
        _object_camera_face(dragger, c);
      }
      object_draw_edit(dragger, cam_mat_inv, cc->projection, id4);
    }
  }


  //Render camera repere
  gl->glClear(GL_DEPTH_BUFFER_BIT);
  float m = 40;
  Matrix4 id;
  mat4_set_identity(id);
  mat4_inverse(id, cam_mat_inv);
  v->camera_repere->Position = vec3(-cc->width/2.0 +m, -cc->height/2.0 + m, -10);
  v->camera_repere->Orientation = quat_inverse(co->Orientation);
  object_compute_matrix_with_quat(v->camera_repere, mo);
  Line* line = object_component_get(v->camera_repere, "line");
  if (line) line->id_texture = r->fbo_all->texture_depth_stencil_id;

  object_draw_edit(v->camera_repere, id4, cc->orthographic, id4);
 
}


