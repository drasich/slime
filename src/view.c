#include <Elementary.h>
#include "view.h"
#include "mesh.h"
#include "object.h"
#include "scene.h"
#include "gl.h"
#include "context.h"
#include "control.h"
#include "ui/property.h"
#include "intersect.h"
#define __UNUSED__

static void
_key_down(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info)
{
  Evas_Event_Key_Down *ev = (Evas_Event_Key_Down*)event_info;
  printf("KEY: down, keyname: %s , key %s \n", ev->keyname, ev->key);

  View* v = evas_object_data_get(o, "view");
  Control* cl = v->control;
  control_key_down(cl, ev);
}

static void
_mouse_move(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o, void *event_info)
{
  //elm_object_focus_set(o, EINA_TRUE);
  Evas_Event_Mouse_Move *ev = (Evas_Event_Mouse_Move*) event_info;

  View* v = evas_object_data_get(o, "view");
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
  v->context->object = o;

  //TODO tell properties to change, through control?
  //or emit a signal to say object selected has changed and catch this signal in properties, and other possible widgets
  property_update(v->property, o);
  tree_update(v->tree, o);
}

static void
_mouse_down(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o, void *event_info)
{
  Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down*) event_info;
  //elm_object_focus_set(o, EINA_TRUE);

  Scene* s = evas_object_data_get(o, "scene");
  View* v = evas_object_data_get(o, "view");
  Control* cl = v->control;
  control_mouse_down(cl, ev);
  Ray r = ray_from_screen(v->camera, ev->canvas.x, ev->canvas.y, 1000);

  bool found = false;
  double d;
  Object* clicked = NULL;

  Eina_List *list;
  Object *ob;
  EINA_LIST_FOREACH(s->objects, list, ob) {
    if (!ob->mesh) continue;
    IntersectionRay ir = intersection_ray_box(r, ob->mesh->box, ob->Position, ob->Orientation);
    if (ir.hit)
      ir = intersection_ray_object(r, ob);
    
    if (ir.hit) {
      double diff = vec3_length2(vec3_sub(ir.position, v->camera->object.Position));
      if ( (found && diff < d) || !found) {
        found = true;
        d = diff;
        clicked = ob;
      }
    }
  }

  if (clicked != NULL) {
    _view_select_object(v, clicked);

    //TODO compute the z if we don't want the outline to display with depth
    //s->quad_outline->Position.Z = -970.0f;
    //printf("test :  %f\n", test);
    //printf("selected position z :  %f\n", selected->Position.Z);
  }

}

static void
_mouse_up(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info)
{
  Evas_Event_Mouse_Up *ev = (Evas_Event_Mouse_Up*)event_info;
  //if (ev->button != 1) return;
  //printf("MOUSE: up   @ %4i %4i\n", ev->canvas.x, ev->canvas.y);
  //evas_object_hide(indicator[0]);
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

Object* _create_repere(float u)
{
  Object* o = create_object();
  o->line = create_line();
  line_add_color(o->line, vec3(0,0,0), vec3(u,0,0), vec4(1,0,0,1));
  line_add_color(o->line, vec3(0,0,0), vec3(0,u,0), vec4(0,1,0,1));
  line_add_color(o->line, vec3(0,0,0), vec3(0,0,u), vec4(0,0,1,1));
  line_set_use_depth(o->line, false);
  return o;
}

Object* _create_grid()
{
  Object* grid = create_object();
  grid->line = create_line();
  line_add_grid(grid->line, 100, 10);
  return grid;
}


// Callbacks
static void
_init_gl(Evas_Object *obj)
{
  View* v = evas_object_data_get(obj, "view");
  
  /*
  v->repere = _create_repere(1);
  line_set_size_fixed(v->repere->line, true);
  v->camera_repere = _create_repere(40);
  v->camera_repere->Position = vec3(10,10, -10);
  line_set_use_perspective(v->camera_repere->line, false);
  v->grid = _create_grid();
  v->camera = create_camera();
  v->camera->object.name = "camera";
  Vec3 p = {20,5,20};
  //v->camera->origin = p;
  //v->camera->object.Position = p;
  camera_pan(v->camera, p);
  Vec3 at = {0,0,0};
  camera_lookat(v->camera, at);
  */

  v->render = create_render();

  gl->glEnable(GL_DEPTH_TEST);
  gl->glEnable(GL_STENCIL_TEST);
  gl->glDepthFunc(GL_LEQUAL);
  gl->glClearDepthf(1.0f);
  gl->glClearStencil(0);
}

static void
_del_gl(Evas_Object *obj)
{
  printf("glview delete gl\n");
  Scene* s = evas_object_data_get(obj, "scene");
  scene_destroy(s);
}

static void
_resize_gl(Evas_Object *obj)
{
   int w, h;
   elm_glview_size_get(obj, &w, &h);
   //printf("resize gl %d, %d \n", w, h);

   // GL Viewport stuff. you can avoid doing this if viewport is all the
   // same as last frame if you want
   gl->glViewport(0, 0, w, h);

   Scene* s = evas_object_data_get(obj, "scene");
   View* v = evas_object_data_get(obj, "view");
   camera_set_resolution(v->camera, w, h);
   quad_resize(v->render->quad_outline->mesh, w, h);
   quad_resize(v->render->quad_color->mesh, w, h);

   //TODO
   fbo_resize(v->render->fbo_all, w, h);
   fbo_resize(v->render->fbo_selected, w, h);
}

static void
_draw_gl(Evas_Object *obj)
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
   Scene* s = evas_object_data_get(obj, "scene");
   scene_update(s);

   View* v = evas_object_data_get(obj, "view");
   view_update(v,0);
   view_draw(v);

   gl->glFinish();
}

static Eina_Bool
_anim(void *data)
{
   elm_glview_changed_set(data);
   return EINA_TRUE;
}

static void
_on_done(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   evas_object_del((Evas_Object*)data);
}


static void
_del(void *data __UNUSED__, Evas *evas __UNUSED__, Evas_Object *obj, void *event_info __UNUSED__)
{
  printf("del ani\n");
   Ecore_Animator *ani = evas_object_data_get(obj, "ani");
   ecore_animator_del(ani);
}

static Evas_Object*
_create_glview(View* view, Evas_Object* win)
{
  Evas_Object *bx, *glview;
  Ecore_Animator *ani;

  view->box = elm_box_add(win);
  bx = view->box;
  evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  //elm_win_resize_object_add(win, bx);
  evas_object_show(bx);

  glview = elm_glview_add(win);
  gl = elm_glview_gl_api_get(glview);
  evas_object_size_hint_align_set(glview, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_size_hint_weight_set(glview, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_glview_mode_set(glview, ELM_GLVIEW_ALPHA | ELM_GLVIEW_DEPTH);
  elm_glview_resize_policy_set(glview, ELM_GLVIEW_RESIZE_POLICY_RECREATE);
  elm_glview_render_policy_set(glview, ELM_GLVIEW_RENDER_POLICY_ON_DEMAND);
  elm_glview_init_func_set(glview, _init_gl);
  elm_glview_del_func_set(glview, _del_gl);
  elm_glview_resize_func_set(glview, _resize_gl);
  elm_glview_render_func_set(glview, _draw_gl);
  elm_box_pack_end(bx, glview);
  evas_object_show(glview);

  elm_object_focus_set(glview, EINA_TRUE);

  ani = ecore_animator_add(_anim, glview);
  evas_object_data_set(glview, "ani", ani);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_DEL, _del, glview);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_KEY_DOWN, _key_down, NULL);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_MOUSE_MOVE, _mouse_move, NULL);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_MOUSE_DOWN, _mouse_down, NULL);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_MOUSE_UP, _mouse_up, NULL);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_MOUSE_WHEEL, _mouse_wheel, NULL);
  evas_object_event_callback_add(glview, EVAS_CALLBACK_MOUSE_IN, _mouse_in, NULL);

  return glview;
}

static void
_new_object(void            *data,
             Evas_Object *obj,
             void            *event_info)
{
  return;
  View* v = (View*) data;
  printf("new object\n");
  tree_add_object(v->tree, NULL);

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
  elm_fileselector_button_path_set(fs_bt, "/home/chris");
  elm_object_text_set(fs_bt, "Select a file");
  elm_object_part_content_set(fs_bt, "icon", ic);

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

  //evas_object_smart_callback_add(fs_bt, "file,chosen", _file_chosen, actors);


  bt = elm_button_add(win);
  elm_object_focus_allow_set(bt, 0);

  elm_object_text_set(bt, "New object");
  evas_object_show(bt);

  evas_object_color_set(bt, r,g,b,a);
  evas_object_resize(bt, 100, 25);
  evas_object_move(bt, 15, 45);
  evas_object_data_set(bt, "view", v);
  evas_object_smart_callback_add(bt, "clicked", _new_object, v);
  //view->addObjectToHide(bt);
  //view->addObjectToHide(fs_bt);


}

static void
_create_view_objects(View* v)
{
  v->repere = _create_repere(1);
  line_set_size_fixed(v->repere->line, true);
  v->camera_repere = _create_repere(40);
  v->camera_repere->Position = vec3(10,10, -10);
  line_set_use_perspective(v->camera_repere->line, false);
  v->grid = _create_grid();
  v->camera = create_camera();
  v->camera->object.name = "camera";
  Vec3 p = {20,5,20};
  //v->camera->origin = p;
  //v->camera->object.Position = p;
  camera_pan(v->camera, p);
  Vec3 at = {0,0,0};
  camera_lookat(v->camera, at);
}

View*
create_view(Evas_Object *win)
{
  View *view = calloc(1,sizeof *view);

  view->context = calloc(1,sizeof *view->context);
  view->control = create_control(view);
  view->glview = _create_glview(view, win);

  _add_buttons(view, win);

  //view->property = property_create(win);
  view->property = create_property(win, view->context);
  view->tree = create_widget_tree(win, view->context);
  evas_object_data_set(view->glview, "view", view);

  _create_view_objects(view);

  return view;
}

void
view_destroy(View* v)
{
  printf("destroy view\n");
  //TODO free camera
  //TODO free scene here?
  free(v->context);
  free(v->control);
  free(v);
}

void
view_update(View* v, double dt)
{
  object_update((Object*)v->camera);
}

Render*
create_render()
{
  Render* r = calloc(1, sizeof *r);
  r->fbo_selected = create_fbo();
  r->fbo_all = create_fbo();

  r->quad_outline = create_object();
  r->quad_outline->mesh = create_mesh_quad(100,100);
  Vec3 t3 = {0,0,-100};
  object_set_position(r->quad_outline, t3);
  r->quad_outline->name = "quad";

  r->quad_outline->mesh->shader = create_shader("shader/stencil.vert", "shader/stencil.frag");
  shader_use(r->quad_outline->mesh->shader);
  shader_init_attribute(r->quad_outline->mesh->shader, "vertex", &r->quad_outline->mesh->attribute_vertex);
  shader_init_uniform(r->quad_outline->mesh->shader, "matrix", &r->quad_outline->mesh->uniform_matrix);
  shader_init_uniform(r->quad_outline->mesh->shader, "resolution", &r->quad_outline->mesh->uniform_resolution);

  r->quad_color = create_object();
  r->quad_color->mesh = create_mesh_quad(100,100);
  object_set_position(r->quad_color, t3);
  r->quad_color->name = "quad";

  r->quad_color->mesh->shader = create_shader("shader/stencil.vert", "shader/quad.frag");
  shader_use(r->quad_color->mesh->shader);
  shader_init_attribute(r->quad_color->mesh->shader, "vertex", &r->quad_color->mesh->attribute_vertex);
  shader_init_uniform(r->quad_color->mesh->shader, "matrix", &r->quad_color->mesh->uniform_matrix);
  shader_init_uniform(r->quad_color->mesh->shader, "resolution", &r->quad_color->mesh->uniform_resolution);

  return r;

}

void
view_draw(View* v)
{
  Camera* c = v->camera;
  Context* cx = v->context;
  Render* r = v->render;
  Scene* s = cx->scene;

  Matrix4 cam_mat_inv, mo;

  mat4_inverse(((Object*)c)->matrix, cam_mat_inv);
  Matrix4* projection = &c->projection;
  Matrix4* ortho = &c->orthographic;

  //Render just selected to fbo
  if (cx->object != NULL) {
    fbo_use(r->fbo_selected);
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT) ;
    object_compute_matrix(cx->object, mo);
    mat4_multiply(cam_mat_inv, mo, mo);
    object_draw(cx->object, mo, *projection);
    fbo_use_end();
  }

  //Render all objects to fbo to get depth for the lines.
  fbo_use(r->fbo_all);
  gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT) ;
  //gl->glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
  //gl->glClearStencil(0);
  gl->glEnable(GL_STENCIL_TEST);
  gl->glStencilFunc(GL_ALWAYS, 0x1, 0x1);
  gl->glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(s->objects, l, o) {
    object_compute_matrix(o, mo);
    mat4_multiply(cam_mat_inv, mo, mo);
    //mat4_multiply(cam_mat_inv, o->matrix, mo);
    object_draw(o, mo, *projection);
  }
  //gl->glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

  //TODO : test, can be removed
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
  v->grid->line->id_texture = r->fbo_all->texture_depth_stencil_id;
  mat4_multiply(cam_mat_inv, mo, mo);
  object_draw_lines_camera(v->grid, mo, c);


  //Render objects
  EINA_LIST_FOREACH(s->objects, l, o) {
    object_compute_matrix(o, mo);
    mat4_multiply(cam_mat_inv, mo, mo);
    //mat4_multiply(cam_mat_inv, o->matrix, mo);
    object_draw(o, mo, *projection);
  }

  //TODO avoid compute matrix 2 times
  //Render lines
  /*
  gl->glClear(GL_DEPTH_BUFFER_BIT);
  EINA_LIST_FOREACH(s->objects, l, o) {
    object_compute_matrix(o, mo);
    mat4_multiply(cam_mat_inv, mo, mo);
    //mat4_multiply(cam_mat_inv, o->matrix, mo);
    //TODO Fix how to use depth texture for lines
    if (o->line != NULL) { o->line->id_texture = r->fbo_all->texture_depth_stencil_id;
    }
    //object_draw_lines(o, mo, *projection);
    object_draw_lines_camera(o, mo, c);
  }
  */
  //Render lines only selected
  gl->glClear(GL_DEPTH_BUFFER_BIT);
  if (cx->object != NULL) {
    o = cx->object;
    object_compute_matrix(o, mo);
    mat4_multiply(cam_mat_inv, mo, mo);
    //mat4_multiply(cam_mat_inv, o->matrix, mo);
    //TODO Fix how to use depth texture for lines
    if (o->line != NULL) { o->line->id_texture = r->fbo_all->texture_depth_stencil_id;
    }
    //object_draw_lines(o, mo, *projection);
    object_draw_lines_camera(o, mo, c);
  }

  //repere
  if (cx->object != NULL) {
    gl->glClear(GL_DEPTH_BUFFER_BIT);
    v->repere->Position = cx->object->Position;
    v->repere->angles = cx->object->angles;
    object_compute_matrix(v->repere, mo);
    v->repere->line->id_texture = r->fbo_all->texture_depth_stencil_id;
    mat4_multiply(cam_mat_inv, mo, mo);
    object_draw_lines_camera(v->repere, mo, c);
  }


  //Render objects with quad
  //object_compute_matrix(s->quad_color, mo);
  //if (s->quad_color->mesh != NULL) 
  //s->quad_color->mesh->id_texture = s->fbo_all->texture_color;
  //object_draw(s->quad_color, mo, *ortho);


  //Render outline with quad
  if (cx->object != NULL) {
    object_compute_matrix(r->quad_outline, mo);
    if (r->quad_outline->mesh != NULL) 
    r->quad_outline->mesh->id_texture = r->fbo_selected->texture_depth_stencil_id;
    object_draw(r->quad_outline, mo, *ortho);
  }

  //gl->glClear(GL_DEPTH_BUFFER_BIT);
  //EINA_LIST_FOREACH(s->ortho, l, o) {
    //object_compute_matrix(o, mo);
    //if (o->mesh != NULL) o->mesh->id_texture = s->fbo_selected->texture_depth_stencil_id;
    //object_draw(o, mo, *ortho);
    ////if (o->mesh != NULL) o->mesh->id_texture = s->fbo_all->texture_color;
    ////object_draw(o, mo, *ortho);
  //}

  gl->glClear(GL_DEPTH_BUFFER_BIT);
  float m = 40;
  Matrix4 id;
  mat4_set_identity(id);
  mat4_inverse(id, cam_mat_inv);
  v->camera_repere->Position = vec3(-v->camera->width/2.0 +m, -v->camera->height/2.0 + m, -10);
  v->camera_repere->Orientation = quat_inverse(v->camera->object.Orientation);
  object_compute_matrix_with_quat(v->camera_repere, mo);
  v->camera_repere->line->id_texture = r->fbo_all->texture_depth_stencil_id;
  mat4_multiply(cam_mat_inv, mo, mo);
  object_draw_lines_camera(v->camera_repere, mo, c);
 
}

