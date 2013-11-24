#include <Elementary.h>
#include "view.h"
#include "component/meshcomponent.h"
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
#include "component/dragger.h"
#define __UNUSED__

static bool s_view_destroyed = false;

static const Vec4 RED = {1.0,0.247,0.188,1};
static const Vec4 GREEN = {0.2117,0.949,0.4156,1};
static const Vec4 BLUE = {0,0.4745,1,1};

// Callbacks
static void
_view_init_gl(Evas_Object *obj)
{
  //gl = elm_glview_gl_api_get(obj);
  View* v = evas_object_data_get(obj, "view");
  v->render = create_render();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glDepthFunc(GL_LEQUAL);
  glClearDepthf(1.0f);
  glClearStencil(0);
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
  glViewport(0, 0, w, h);

  View* v = evas_object_data_get(obj, "view");
  ccamera_set_resolution(v->camera->camera_component, w, h);

  //quad_resize(v->render->quad_outline->mesh, w, h);
  //quad_resize(v->render->quad_color->mesh, w, h);
  
  MeshComponent* mc = object_component_get(v->render->quad_outline, "mesh");
  Shader* s = mesh_component_shader_get(mc);
  shader_use(s);

  GLint uni_resolution = shader_uniform_location_get(s, "resolution");
  if (uni_resolution >= 0) glUniform2f(uni_resolution, w, h);

  //TODO the actual mesh is resized, see if we can scale it 
  //
  quad_resize(mesh_component_mesh_get(mc), w, h);

  /*
  mc = object_component_get(v->render->quad_color, "mesh");
  shader_use(mc->shader);
  glUniform2f(mc->mesh->uniform_resolution, w, h);
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

  glViewport(0, 0, w, h);
  //glClearColor(1.0,0.8,0.3,1);
  glClearColor(0.2,0.2,0.2,1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Draw a Triangle
  glEnable(GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //TODO remove the function update here
  /*
  Scene* s = evas_object_data_get(obj, "scene");
  if (s)
  scene_update(s);
  */

  View* v = evas_object_data_get(obj, "view");
  view_update(v,0);
  view_draw(v);

   glFinish();
}

static void
_tb_click_cb(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   //char buf[PATH_MAX];
   //snprintf(buf, sizeof(buf), "%s/images/panel_01.jpg", elm_app_data_dir_get());
   //elm_photo_file_set(data, buf);
   printf("you click an item of the toolbar\n");
}



static Evas_Object*
_create_toolbar(Evas_Object* win)
{
  Evas_Object* tb = elm_toolbar_add(win);
  elm_toolbar_shrink_mode_set(tb, ELM_TOOLBAR_SHRINK_MENU);
  evas_object_size_hint_weight_set(tb, 0.0, 0.0);
  evas_object_size_hint_weight_set(tb, 1.0, 1.0);
  evas_object_size_hint_align_set(tb, EVAS_HINT_FILL, 0.0);
  //   elm_object_scale_set(tb, 0.9);

  Elm_Object_Item* tb_it = elm_toolbar_item_append(tb, "document-print", "Hello", _tb_click_cb, NULL);
  elm_object_item_disabled_set(tb_it, EINA_TRUE);
  elm_toolbar_item_priority_set(tb_it, 100);

  evas_object_show(tb);

  /*
   tb_it = elm_toolbar_item_append(tb, "folder-new", "World", _tb_sel2_cb, ph1);
   elm_toolbar_item_priority_set(tb_it, -100);

   tb_it = elm_toolbar_item_append(tb, "object-rotate-right", "H", _tb_sel3_cb, ph4);
   elm_toolbar_item_priority_set(tb_it, 150);

   tb_it = elm_toolbar_item_append(tb, "mail-send", "Comes", _tb_sel4_cb, ph4);
   elm_toolbar_item_priority_set(tb_it, 0);

   tb_it = elm_toolbar_item_append(tb, "clock", "Elementary", _tb_sel5_cb, ph4);
   elm_toolbar_item_priority_set(tb_it, -200);

   tb_it = elm_toolbar_item_append(tb, "refresh", "Menu", NULL, NULL);
   elm_toolbar_item_menu_set(tb_it, EINA_TRUE);
   elm_toolbar_item_priority_set(tb_it, -9999);
   elm_toolbar_menu_parent_set(tb, win);
   menu = elm_toolbar_item_menu_get(tb_it);
   */


  return tb;

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
  Vec3 camdir = quat_rotate_vec3(c->object.orientation, vec3(0,0,-1));
  Vec3 camup = quat_rotate_vec3(c->object.orientation, vec3(0,1,0));
  Vec3 camright = vec3_cross(camdir, camup);
  double cam_height = tan(c->fovy/2.0)*c->near*2.0f;
  double cam_width = cam_height* c->aspect;

  float width_ratio = cam_width/c->width;
  float height_ratio = cam_height/c->height;

  double cx = -c->width + left + width/2.0f;
  cx *= width_ratio;
  double cy = c->height - top - height/2.0f;
  cy *= height_ratio;

  Vec3 ccenter = vec3_add(c->object.position, vec3_mul(camdir, c->near));
  Vec3 center = vec3_add(ccenter, vec3_mul(camright, cx));
  center = vec3_add(center, vec3_mul(camup, cy));
  
  Vec3 dir = vec3_sub(center, c->object.position);
  dir = vec3_normalized(dir);
  Quat qq = quat_between_vec(ccenter, center);
  qq = quat_mul(c->object.orientation, qq);

  frustum_set(
        f,
        c->near, 
        c->far,
        c->object.position, 
        quat_rotate_vec3(c->object.orientation, vec3(0,0,-1)),
        quat_rotate_vec3(c->object.orientation, vec3(0,1,0)),
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

  Evas_Coord x, y, w, h;
  evas_object_geometry_get (v->glview, &x, &y, &w, &h);
  if (sx + yepx > x + w) yepx = x + w - sx;
  if (sy + yepy > y + h) yepy = y + h - sy;

  if (sx < x) {
    yepx = yepx - (x - sx);
    sx = x;
  }
  if (sy < y) {
    yepy = yepy - (y - sy);
    sy = y;
  }

  evas_object_move(rect, sx, sy);
  evas_object_resize(rect, yepx, yepy);


  ViewCamera* c = v->camera;
  //Frustum f;
  //frustum_from_rect(&f, c, sx, sy, yepx, yepy);

  sx = sx - x;
  sy = sy - y;

  Plane planes[6];
  camera_get_frustum_planes_rect(c, planes, sx, sy, yepx, yepy );

  Render* r = v->render;

  Eina_List* newlist = NULL;
  Eina_List *l;
  RenderObject *ro;
  EINA_LIST_FOREACH(r->render_objects, l, ro) {
    Object* o = ro->object;
    if (planes_is_in_object(planes, 6, o)) {
      newlist = eina_list_append(newlist, o);
    }
  }

  context_objects_set(v->context, newlist);
  /*
  bool b = frustum_is_in(&f, o->position);
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
  camera_world_to_screen(v->camera, object_world_position_get(o));
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

static void
_object_duplicate(void* data, Evas_Object* o, void* event_info)
{
  View* v = data;
  Eina_List* objects = context_objects_get(v->context);
  Scene* s = context_scene_get(v->context);


  Eina_List* newobj = NULL;
  Eina_List* l;
  Object* obj;
  EINA_LIST_FOREACH(objects, l, obj) {
    Object* copy = object_copy(obj);
    object_post_read(copy);
    newobj = eina_list_append(newobj, copy);
  }

  control_objects_add(v->control, s, newobj);
  context_objects_set(v->context, newobj);
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
    elm_menu_item_add(menu, NULL, NULL, "Duplicate", _object_duplicate, v);
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

  Evas_Coord x, y, w, h;
  evas_object_geometry_get (v->glview, &x, &y, &w, &h);
  int cx = ev->canvas.x - x;
  int cy = ev->canvas.y - y;

  Ray r = ray_from_screen(v->camera, cx, cy, 1000);
  printf("clickgl pos : %d, %d \n", cx, cy);

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
      double diff = vec3_length2(vec3_sub(ir.position, v->camera->object->position));
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
  o->name = "repere";
  Component* comp = create_component(&line_desc);
  object_add_component(o,comp);
  Line* l = comp->data;
  l->camera = camera;
  line_add_color(l, vec3(0,0,0), vec3(u,0,0), RED);
  line_add_color(l, vec3(0,0,0), vec3(0,u,0), GREEN);
  line_add_color(l, vec3(0,0,0), vec3(0,0,u), BLUE);
  line_set_use_depth(l, false);
  return o;
}

static Object* 
_create_grid(Camera* camera)
{
  Object* grid = create_object();
  grid->name = "grid";
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

    Vec3 cp = v->camera->object->position;
    Vec3 direction = quat_rotate_vec3(v->camera->object->orientation, vec3(0,0,-1));
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
  yep->name = eina_stringshare_add("new_empty");
  View* v = (View*) data;

  Vec3 cp = v->camera->object->position;
  Vec3 direction = quat_rotate_vec3(v->camera->object->orientation, vec3(0,0,-1));
  cp = vec3_add(
        cp,
        vec3_mul(direction, 30));

  object_set_position(yep, cp);
  control_object_add(v->control, v->context->scene, yep);

  context_objects_clean(v->context);
  context_object_add(v->context, yep);

}

#include "ui/resource_view.h"
static Scene* gamescene_ = NULL;
static void
_gameview_closed(void *data, Evas_Object *obj, void *event_info)
{
  printf("gameview closed\n");
  if (s_view_destroyed) return;

  View* v = data;
  resource_view_scene_del(v->rv, gamescene_);

  Scene* cs = context_scene_get(v->context);
  if (cs == gamescene_) {
    //TODO select another scene... the first scene? or last selected scene before gameview
    void* s;
    Eina_Iterator *it = eina_hash_iterator_data_new(resource_scenes_get(s_rm));
    while (eina_iterator_next(it, &s))
     {
      view_scene_set(v, s);
      //const char *number = data;
      //printf("%s\n", number);
      break;
     }
  }

  scene_del(gamescene_);
  gamescene_ = NULL;

  //Context* context = v->context;

  //scene_del(context->scene);
  //Scene* s = scene_read("scenecur.eet");
  //scene_post_read(s);

  //context_objects_clean(context);
  //context->scene = s;
}


Evas_Object* gameview_;

static void
_play(void *data,
      Evas_Object *obj,
      void *event_info)
{
  View* v = data;

  if (!gameview_) {
    //scene_write(v->context->scene, "scenecur.eet");

    Scene* s = scene_copy(v->context->scene, "gameviewcopy");
    printf("scene to copy is name is %s\n", v->context->scene->name);
    scene_post_read(s);
    gamescene_ = s;

    resource_view_scene_add(v->rv, s);

    //gameview_ = create_gameview_window(v->context->scene, &gameview_, v->control );
    gameview_ = create_gameview_window(s, &gameview_, v->control );
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
  scene_del(v->context->scene);
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
    Component* c = create_component(cd);
    control_component_add(v->control, o, c);
  }
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
    elm_menu_item_add(menu, NULL, NULL, c->name, _addcomp, c);
  }

  return menu;
}

static void
_addcomponent(void *data,
      Evas_Object *obj,
      void *event_info)
{
  View* v = data;

  Evas_Object* win = evas_object_top_get (evas_object_evas_get(obj));
  Evas_Object* menu = _create_component_menu(win, s_component_manager->components);
  evas_object_data_set(menu, "view", v);
  evas_object_show(menu);

  Evas_Coord x,y,w,h;
  evas_object_geometry_get(obj, &x, &y, &w, &h);
  elm_menu_move(menu, x, y);
}

static void
_dragger_global_local_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
  View* v = data;
  v->control->dragger_is_local = !v->control->dragger_is_local;
}

static void
_add_buttons(View* v, Evas_Object* win)
{
  Evas_Object* fs_bt, *ic, *bt;
  int r,g,b,a;

  ic = elm_icon_add(win);
  elm_icon_standard_set(ic, "file");
  evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);

  Evas_Object* box = elm_box_add(win);
  elm_box_horizontal_set(box, EINA_TRUE);
  //evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_show(box);
  //elm_box_pack_end(v->box, box);
  elm_table_pack(v->table, box, 0, 0, 1, 1);
  //evas_object_color_set(box, 150,150,150,150);
  //elm_box_homogeneous_set(box, EINA_TRUE);

  /*
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

  evas_object_color_get(fs_bt, &r,&g,&b,&a);
  a = 150;
  evas_object_color_set(fs_bt, r,g,b,a);
  evas_object_color_set(ic, r,g,b,a);
  evas_object_resize(fs_bt, 100, 25);
  evas_object_move(fs_bt, 15, 15);

  evas_object_smart_callback_add(fs_bt, "file,chosen", _file_chosen, v);
  */


  bt = elm_button_add(win);
  elm_object_focus_allow_set(bt, 0);

  elm_object_text_set(bt, "Add empty");
  evas_object_show(bt);
  elm_box_pack_end(box, bt);

  evas_object_color_get(bt, &r,&g,&b,&a);
  a = 150;

  evas_object_color_set(bt, r,g,b,a);
  evas_object_resize(bt, 100, 25);
  evas_object_move(bt, 15, 15);
  evas_object_data_set(bt, "view", v);
  evas_object_smart_callback_add(bt, "clicked", _new_empty, v);
  //view->addObjectToHide(bt);
  //view->addObjectToHide(fs_bt);


  bt = elm_button_add(win);
  elm_object_focus_allow_set(bt, 0);

  elm_object_text_set(bt, "play");
  elm_box_pack_end(box, bt);
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
  elm_box_pack_end(box, bt);
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
  elm_box_pack_end(box, bt);
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
  elm_box_pack_end(box, bt);
  evas_object_show(bt);

  evas_object_color_set(bt, r,g,b,a);
  evas_object_resize(bt, 50, 25);
  evas_object_move(bt, 405, 15);
  evas_object_data_set(bt, "view", v);
  evas_object_smart_callback_add(bt, "clicked", _reload, v);


  Evas_Object* tg = elm_check_add(win);
  evas_object_color_set(tg, r,g,b,a);
  //evas_object_move(tg, 15, 45);
  //evas_object_resize(tg, 200, 25);
  elm_object_style_set(tg, "toggle");
  elm_object_text_set(tg, "Dragger");
  elm_object_part_text_set(tg, "on", "Local");
  elm_object_part_text_set(tg, "off", "Global");
  //elm_box_pack_end(bx, tg);
  elm_box_pack_end(box, tg);
  evas_object_show(tg);
  evas_object_smart_callback_add(tg, "changed", _dragger_global_local_changed_cb, v);

}

static Eina_List*
_view_draggers_create(dragger_create_fn dfn, bool create_plane, Quat q1, Quat q2, Quat q3 )
{
  Eina_List* draggers = NULL;

  Vec4 red = RED;
  Vec4 green = GREEN;
  Vec4 blue = BLUE;

  Object* dragger;

  dragger = dfn(
        vec3(1,0,0),
        red,
        false);
  //must be the component
  Dragger* dc = object_component_get(dragger, "dragger");
  dc->ori = q1;
  draggers = eina_list_append(draggers, dragger);

  dragger = dfn(
        vec3(0,1,0),
        green,
        false);
  dc = object_component_get(dragger, "dragger");
  dc->ori = q2;
  draggers = eina_list_append(draggers, dragger);

  dragger = dfn(
        vec3(0,0,1),
        blue,
        false);
  dc = object_component_get(dragger, "dragger");
  dc->ori = quat_identity();
  draggers = eina_list_append(draggers, dragger);

  if (!create_plane) return draggers;

  red.w = 0.1f;
  green.w = 0.1f;
  blue.w = 0.1f;

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
  dc = object_component_get(dragger, "dragger");
  dc->ori = quat_yaw_pitch_roll_deg(-90, 0,0);
  draggers = eina_list_append(draggers, dragger);

  dragger = dfn(
        vec3(1,0,1),
        green,
        true);
  dc = object_component_get(dragger, "dragger");
  dc->ori = quat_yaw_pitch_roll_deg(0, 0,90);
  draggers = eina_list_append(draggers, dragger);
  return draggers;
}

static void
_create_view_objects(View* v)
{
  v->camera = view_camera_new();
  v->camera->object->orientation_type = ORIENTATION_EULER;
  Vec3 p = {50,20,50};
  //v->camera->origin = p;
  //v->camera->object.position = p;
  camera_pan(v->camera, p);
  Vec3 at = {0,0,0};
  camera_lookat(v->camera, at);

  v->repere = _create_repere(1, v->camera->camera_component);
  Line* l = object_component_get(v->repere, "line");
  if (l) line_set_size_fixed(l, true);

  Quat q1 = quat_yaw_pitch_roll_deg(-90,0,0);
  Quat q2 = quat_yaw_pitch_roll_deg(0, 90,0);
  Quat qi = quat_identity();

  v->dragger_translate = _view_draggers_create(_dragger_translate_create, true, q1,q2,qi);
  v->dragger_scale = _view_draggers_create(_dragger_scale_create, true, q1,q2,qi);

  Quat q3 = quat_yaw_pitch_roll_deg(90,0,0);
  Quat q4 = quat_yaw_pitch_roll_deg(0, -90,0);
  v->dragger_rotate = _view_draggers_create(_dragger_rotate_create, false, q3, q4, qi);

  v->draggers = v->dragger_translate;

  v->camera_repere = _create_repere(40, v->camera->camera_component);
  v->camera_repere->position = vec3(10,10, -10);
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
view_new(Evas_Object *win)
{
  View *view = calloc(1,sizeof *view);

  view->context = calloc(1,sizeof *view->context);
  view->control = create_control(view);
  
  view->table = elm_table_add(win);
  evas_object_size_hint_weight_set(view->table, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_show(view->table);
  elm_table_homogeneous_set(view->table, EINA_TRUE);

  //Evas_Object* toolbar = _create_toolbar(win);
  //elm_box_pack_end(view->box, toolbar);

  view->glview = _create_glview(win);
  elm_table_pack(view->table, view->glview, 0, 0, 1, 10);
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
    //*
    printf("I write the scene before I destroy the scene\n");
    scene_print(v->context->scene);
    scene_write(v->context->scene, "scene/scenewrite.eet");
    //*/
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

    Mesh* m = mesh_component_mesh_get(mc);
    if (!m) {
      _render_object_add(v, o, root);
      continue;
    }

    OBox b;
    Vec3 pos = object_world_position_get(o);
    Quat ori = object_world_orientation_get(o);
    Vec3 sca = object_world_scale_get(o);
    aabox_to_obox(m->box, b, pos, ori, sca);

    if (planes_is_box_in_allow_false_positives(planes, 6, b)) {
      //if (planes_is_in(planes, 6, o->position)) {
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
}

#include "resource.h"

Render*
create_render()
{
  Render* r = calloc(1, sizeof *r);
  r->fbo_selected = create_fbo();
  r->fbo_all = create_fbo();

  r->quad_outline = create_object();
  r->quad_outline->name = "outline";
  Component* comp = create_component(&mesh_desc);
  MeshComponent* mc = comp->data;
  mesh_component_mesh_set_by_name(mc, "quad");

  r->quad_outline->mesh = mesh_component_mesh_get(mc);
  object_add_component(r->quad_outline, comp);
  Vec3 t3 = {0,0,-100};
  object_set_position(r->quad_outline, t3);
  r->quad_outline->name = eina_stringshare_add("quad");

  Texture* tsel = texture_new();
  Texture* tall = texture_new();
  texture_fbo_link(tsel, &r->fbo_selected->texture_depth_stencil_id);
  texture_fbo_link(tall, &r->fbo_all->texture_depth_stencil_id);

  Shader* s = create_shader("stencil", "shader/stencil.vert", "shader/stencil.frag");
  shader_attribute_add(s, "vertex", 3, GL_FLOAT);
  shader_uniform_add(s, "matrix");
  shader_uniform_add(s, "resolution");
  shader_uniform_type_add(s, "texture", UNIFORM_TEXTURE, false);
  shader_uniform_type_add(s, "texture_all", UNIFORM_TEXTURE, false);

  mesh_component_shader_set(mc, s);

  TextureHandle* th = texture_handle_new();
  th->name = "fbo_sel";
  th->texture = tsel;
  shader_instance_texture_data_set(mc->shader_instance, "texture", th);
  th = texture_handle_new();
  th->name = "fbo_all";
  th->texture = tall;
  shader_instance_texture_data_set(mc->shader_instance, "texture_all", th);


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
_object_camera_face(Quat qo, Object* o, ViewCamera* c)
{
  Dragger* d = object_component_get(o, "dragger");

  Vec3 diff = vec3_sub(o->position, c->object->position);
  double dotx = vec3_dot(diff, quat_rotate_vec3(qo, vec3(1,0,0)));
  double doty = vec3_dot(diff, quat_rotate_vec3(qo, vec3(0,1,0)));
  double dotz = vec3_dot(diff, quat_rotate_vec3(qo, vec3(0,0,1)));
  float angle = 0;
  o->angles.x = 0;
  o->angles.y = 0;
  o->angles.z = 0;
  /*
  Vec3 camx = quat_rotate_vec3(c->object->orientation, vec3(1,0,0));
  printf("camx : %f, %f ,%f\n", camx.x, camx.y, camx.z);
  Vec3 obx = quat_rotate_vec3(qo, vec3(1,0,0));
  double dot = vec3_dot(obx, camx);
  */

  if ( vec3_equal(d->constraint, vec3(0,0,1))) {
    if (dotx >0) {
      if (doty >0)
      angle = -180;
      else
      angle = -90;
    }
    else if (doty >0)
      angle = 90;
  }

  if ( vec3_equal(d->constraint, vec3(0,1,0))) {
    if (dotx >0) {
      if (dotz >0)
      angle = -180;
      else
      angle = -90;
    }
    else if (dotz >0)
      angle = 90;
  }

  if ( vec3_equal(d->constraint, vec3(1,0,0))) {
    if (doty >0) {
      if (dotz >0)
      angle = 180;
      else
      angle = 90;
    }
    else if (dotz >0)
      angle = -90;
  }


  Quat q = quat_yaw_pitch_roll_deg(0,0, angle);

  //o->orientation = quat_mul(q, d->ori);
  o->orientation = quat_mul(d->ori,q);
  o->orientation = quat_mul(qo, o->orientation);
  o->orientation_type = ORIENTATION_QUAT;

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
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT) ;
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
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT) ;
  //glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
  //glClearStencil(0);
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 0x1, 0x1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  /*
  EINA_LIST_FOREACH(r->objects, l, o) {
    object_draw_edit_component(o, cam_mat_inv, cc->projection , id4, "mesh");
  }
  */

  EINA_LIST_FOREACH(r->render_objects, l, ro) {
    object_draw_edit_component2(ro->object, cam_mat_inv, cc->projection, ro->world, "mesh");
  }

  //glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

  //was a test, can be removed
  /*
  int w = c->width;
  int h = c->height;
  printf(" w , h : %d, %d \n", w, h);
  GLuint mypixels[w*h];
  glReadPixels(
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
  glClear(GL_DEPTH_BUFFER_BIT);
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
    //bool b = frustum_is_in(&f, o->position);
    //if (!b) continue;
    object_draw_edit(o, cam_mat_inv, cc->projection, id4);
  }
  */

  EINA_LIST_FOREACH(r->render_objects, l, ro) {
    object_draw_edit2(ro->object, cam_mat_inv, cc->projection, ro->world);
  }

  //TODO avoid compute matrix 2 times

  //Render lines only selected
  glClear(GL_DEPTH_BUFFER_BIT);
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
    glClear(GL_DEPTH_BUFFER_BIT);
    v->repere->position = repere_position;
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
    glClear(GL_DEPTH_BUFFER_BIT);
    EINA_LIST_FOREACH(v->draggers, l, dragger){
      dragger->position = repere_position;
      Dragger* d = object_component_get(dragger, "dragger");
      if (d) {
        dragger->orientation_type = ORIENTATION_QUAT;
        if (d->type == DRAGGER_SCALE || v->control->dragger_is_local) {
          dragger->orientation = quat_mul(repere_ori, d->ori);
        }
        else {
          dragger->orientation = d->ori;
        }

        if (d->type == DRAGGER_ROTATE) {
          if (v->control->dragger_is_local)
          _object_camera_face(repere_ori, dragger, c);
          else
          _object_camera_face(quat_identity(), dragger, c);
        }
      }
      object_draw_edit(dragger, cam_mat_inv, cc->projection, id4);
    }
  }


  //Render camera repere
  glClear(GL_DEPTH_BUFFER_BIT);
  float m = 40;
  Matrix4 id;
  mat4_set_identity(id);
  mat4_inverse(id, cam_mat_inv);
  v->camera_repere->position = vec3(-cc->width/2.0 +m, -cc->height/2.0 + m, -10);
  v->camera_repere->orientation = quat_inverse(co->orientation);
  object_compute_matrix_with_quat(v->camera_repere, mo);
  Line* line = object_component_get(v->camera_repere, "line");
  if (line) line->id_texture = r->fbo_all->texture_depth_stencil_id;

  object_draw_edit(v->camera_repere, id4, cc->orthographic, id4);
 
}


void
view_scene_set(View* v, Scene* s)
{
  context_scene_set(v->context, s);
  tree_scene_set(v->tree, s);
  //char yep[256];
  //sprintf(yep, "<b>Scene: </b>%s", s->name);
  //elm_object_text_set(v->scene_entry, yep);
 
  property_scene_show(v->property, s);
}
