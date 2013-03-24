#include <Elementary.h>
#include "view.h"
#include "mesh.h"
#include "object.h"
#include "scene.h"
#include "gl.h"
#include "context.h"
#include "intersect.h"
#define __UNUSED__

static void
_key_down(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info)
{
  Evas_Event_Key_Down *ev = (Evas_Event_Key_Down*)event_info;
  printf("KEY: down, keyname: %s , key %s \n", ev->keyname, ev->key);
  if (!strcmp(ev->keyname, "Escape")) elm_exit();
}

static void rotate_camera(View* v, float x, float y)
{
  Camera* cam = v->camera;
  Object* c = (Object*) cam;

  cam->yaw += 0.005f*x;
  cam->pitch += 0.005f*y;

  Quat qy = quat_angle_axis(cam->yaw, vec3(0,1,0));
  Quat qp = quat_angle_axis(cam->pitch, vec3(1,0,0));
  Quat result = quat_mul(qy, qp);

  c->Orientation = result;

  Object* o = v->context->object;
  if (o == NULL) return;

  camera_rotate_around(cam, result, o->Position);
}

static void
_mouse_move(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o, void *event_info)
{
  Evas_Event_Mouse_Move *ev = (Evas_Event_Mouse_Move*) event_info;
  //elm_object_focus_set(o, EINA_TRUE);

  if (ev->buttons & 1 == 1){
    View* v = evas_object_data_get(o, "view");
    float x = ev->cur.canvas.x - ev->prev.canvas.x;
    float y = ev->cur.canvas.y - ev->prev.canvas.y;

    const Evas_Modifier * mods = ev->modifiers;
    if (evas_key_modifier_is_set(mods, "Shift")) {
      Vec3 t = {-x*0.05f, y*0.05f, 0};
      camera_pan(v->camera, t);
    } else {
      rotate_camera(v, x, y);
    }
  }
}

Ray
ray_from_click(Camera* c, double x, double y)
{
  double near = c->near;
  Vec3 camz = quat_rotate_vec3(c->object.Orientation, vec3(0,0,-1));
  Vec3 up = quat_rotate_vec3(c->object.Orientation, vec3(0,1,0));
  Vec3 h = vec3_cross(camz, up);
  h = vec3_normalized(h);
  double l = vec3_length(h);
  double vl = tan(c->fovy/2.0) * near;

  // used to do this : elm_glview_size_get(o, &width, &height);
  int width = c->width, height = c->height;
  double aspect = (double)width/ (double)height;
  double vh = vl * aspect;

  up = vec3_mul(up, vl);
  h = vec3_mul(h, vh);

  x -= (double)width / 2.0;
  y -= (double)height / 2.0;

  y /= (double)height / 2.0;
  x /= (double)width / 2.0;

  Vec3 pos = vec3_add(
        c->object.Position, 
        vec3_add(
          vec3_mul(camz,near),
          vec3_add(vec3_mul(h,x), vec3_mul(up,-y))
          )
        );

  Vec3 dir = vec3_sub(pos, c->object.Position);
  dir = vec3_normalized(dir);
  dir = vec3_mul(dir, 100); //TODO length

  Ray r = {pos, dir};
  return r;
}

static void
_mouse_down(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *o, void *event_info)
{
  Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down*) event_info;
  //elm_object_focus_set(o, EINA_TRUE);

  Scene* s = evas_object_data_get(o, "scene");
  View* v = evas_object_data_get(o, "view");
  Ray r = ray_from_click(v->camera, ev->canvas.x, ev->canvas.y);

  bool found = false;
  double d;
  Object* oh = NULL;

  Eina_List *list;
  Object *ob;
  EINA_LIST_FOREACH(s->objects, list, ob) {
    IntersectionRay ir = intersection_ray_box(r, ob->mesh->box, ob->Position, ob->Orientation);
    //if (ir.hit)
    //  ir = intersection_ray_mesh(r, ob->mesh);
    
    if (ir.hit) {


      double diff = vec3_length2(vec3_sub(ir.position, v->camera->object.Position));

      if ( (found && diff < d) || !found) {
        found = true;
        d = diff;
        oh = ob;
        //TODO mesh collision
      }
    }
  }

  if (oh != NULL) {
    //s->selected = oh;
    v->context->object = oh;
    Vec3 yep = quat_rotate_vec3(v->camera->object.Orientation, v->camera->local_offset);
    Vec3 tt = vec3_sub(v->camera->object.Position, yep);
    v->camera->origin = quat_rotate_around(quat_inverse(v->camera->object.Orientation), v->context->object->Position, tt);

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
  printf("MOUSE: up   @ %4i %4i\n", ev->canvas.x, ev->canvas.y);
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
  axis = vec3_mul(axis, 0.5f);
  v->camera->object.Position = vec3_add(v->camera->object.Position, axis);
}

static void
populate_scene(Scene* s)
{
  //Object* o = create_object_file("model/smallchar.bin");
  Object* o = create_object_file("model/cube.bin");
  o->name = "cube";
  //Object* o = create_object_file("model/simpleplane.bin");
  //TODO free shader
  Shader* shader_simple = create_shader("shader/simple.vert", "shader/simple.frag");
  o->mesh->shader = shader_simple;
  mesh_init_attributes(o->mesh);
  mesh_init_uniforms(o->mesh);

  Vec3 t = {-0,-4,-5};
  //Vec3 t = {0,0,0};
  object_set_position(o, t);
  Vec3 axis = {1,0,0};
  Quat q = quat_angle_axis(3.14159f/2.f, axis);
  Vec3 axis2 = {0,0,1};
  Quat q2 = quat_angle_axis(3.14159f/4.f, axis2);
  q = quat_mul(q, q2);
  object_set_orientation(o, q2);
  scene_add_object(s,o);

  //animation_play(o, "walkquat", LOOP);

  Object* yep = create_object_file("model/smallchar.bin");
  yep->mesh->shader = shader_simple;
  mesh_init_attributes(yep->mesh);
  mesh_init_uniforms(yep->mesh);

  yep->name = "2222222";
  Vec3 t2 = {0,-5,-20};
  object_set_position(yep, t2);
  object_set_orientation(yep, q);
  scene_add_object(s,yep);

  //GLint bits;
  //gl->glGetIntegerv(GL_DEPTH_BITS, &bits);
  //printf("depth buffer %d\n\n", bits);
  Matrix4 la;
  Vec3 at = {1,0,-1};
  Vec3 up = {0,1,0};
  mat4_lookat(la, vec3_zero(), at, up);
  Quat myq = mat4_get_quat(la);
  Vec4 toaa = quat_to_axis_angle(myq);
  printf(" toaa : %f, %f, %f, %f \n", toaa.X, toaa.Y, toaa.Z, toaa.W);

  Triangle tri = { 
    vec3(0,0,-10), 
    vec3(10,0,-10), 
    vec3(5,5,-10)
  };
  Ray r = {
    vec3(2,2,0),
    vec3(0,0,-100)
  };

  IntersectionRay ir = intersection_ray_triangle(r,tri,1);
  if (ir.hit)
    printf("!!!!!!!!!!!!!!!there is a collision \n");
  else
    printf("!!!!!!!!!!!!!!!no collision \n");

}

// Callbacks
static void
_init_gl(Evas_Object *obj)
{
  Scene* s = create_scene();
  evas_object_data_set(obj, "scene", s);
  populate_scene(s);
  
  View* v = evas_object_data_get(obj, "view");
  v->context->scene = s;
  v->camera = create_camera();
  v->camera->object.name = "camera";
  Vec3 p = {0,0,20};
  v->camera->origin = p;
  v->camera->object.Position = p;
  Vec3 at = {0,0,0};
  camera_lookat(v->camera, at);

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
  Evas_Object *bx, *bt, *glview;
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

  /*
  bt = elm_button_add(win);
  elm_object_text_set(bt, "OK");
  evas_object_size_hint_align_set(bt, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_size_hint_weight_set(bt, EVAS_HINT_EXPAND, 0.0);
  elm_box_pack_end(bx, bt);
  evas_object_show(bt);
  evas_object_smart_callback_add(bt, "clicked", _on_done, win);
  */


  return glview;

}

View*
create_view(Evas_Object *win)
{
  View *view = calloc(1,sizeof *view);

  view->context = calloc(1,sizeof *view->context);
  view->glview = _create_glview(view, win);
  evas_object_data_set(view->glview, "view", view);

  return view;
}

void
view_destroy(View* v)
{
  printf("destroy view\n");
  //TODO free camera
  //TODO free scene here?
  free(v->context);
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
  shader_init_attribute(r->quad_outline->mesh->shader, "vertex", &r->quad_outline->mesh->attribute_vertex);
  shader_init_uniform(r->quad_outline->mesh->shader, "matrix", &r->quad_outline->mesh->uniform_matrix);
  shader_init_uniform(r->quad_outline->mesh->shader, "resolution", &r->quad_outline->mesh->uniform_resolution);

  r->quad_color = create_object();
  r->quad_color->mesh = create_mesh_quad(100,100);
  object_set_position(r->quad_color, t3);
  r->quad_color->name = "quad";

  r->quad_color->mesh->shader = create_shader("shader/stencil.vert", "shader/quad.frag");
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
    object_draw(o, mo, *projection);
  }
  //gl->glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

  //TODO : test, can be removed
  //
  //int w = c->width;
  //int h = c->height;
  //printf(" w , h : %d, %d \n", w, h);
  //GLuint mypixels[w*h];
  //gl->glReadPixels(
        //0, 
        //0, 
        //w, 
        //h, 
        //GL_DEPTH_STENCIL_OES, 
        //GL_UNSIGNED_INT_24_8_OES, 
        //mypixels);
  //save_png(mypixels, w, h);

  fbo_use_end();


  //Render objects
  EINA_LIST_FOREACH(s->objects, l, o) {
    //object_compute_matrix(o, mo);
    //mat4_multiply(cam_mat_inv, mo, mo);
    mat4_multiply(cam_mat_inv, o->matrix, mo);
    object_draw(o, mo, *projection);
  }

  //TODO avoid compute matrix 2 times
  //Render lines
  //gl->glClear(GL_DEPTH_BUFFER_BIT);
  //EINA_LIST_FOREACH(s->objects, l, o) {
    //object_compute_matrix(o, mo);
    //mat4_multiply(cam_mat_inv, mo, mo);
    ////TODO Fix how to use depth texture for lines
    //if (o->line != NULL) o->line->id_texture = s->fbo_all->texture_depth_stencil_id;
    //object_draw_lines(o, mo, *projection);
  //}


  //Render objects with quad
  //object_compute_matrix(s->quad_color, mo);
  //if (s->quad_color->mesh != NULL) 
  //s->quad_color->mesh->id_texture = s->fbo_all->texture_color;
  //object_draw(s->quad_color, mo, *ortho);


  //Render outline with quad
  object_compute_matrix(r->quad_outline, mo);
  if (r->quad_outline->mesh != NULL) 
  r->quad_outline->mesh->id_texture = r->fbo_selected->texture_depth_stencil_id;
  object_draw(r->quad_outline, mo, *ortho);

  //gl->glClear(GL_DEPTH_BUFFER_BIT);
  //EINA_LIST_FOREACH(s->ortho, l, o) {
    //object_compute_matrix(o, mo);
    //if (o->mesh != NULL) o->mesh->id_texture = s->fbo_selected->texture_depth_stencil_id;
    //object_draw(o, mo, *ortho);
    ////if (o->mesh != NULL) o->mesh->id_texture = s->fbo_all->texture_color;
    ////object_draw(o, mo, *ortho);
  //}
  
}

