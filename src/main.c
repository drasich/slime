#include <Elementary.h>
#include "view.h"
#include "gameview.h"
#include "ui/tree.h"
#include "scene.h"
#include "control.h"
#include "resource.h"
#include "texture.h"
#include "component/meshcomponent.h"
#include "ui/resource_view.h"
#define __UNUSED__

//TODO put these in application
static View* view;

static void
create_workspace(Evas_Object* parent)
{

}

static Evas_Object *create_my_group(Evas *canvas, const char *text)
{
  Evas_Object *edje;

  edje = edje_object_add(canvas);
  if (!edje)
   {
    EINA_LOG_CRIT("could not create edje object!");
    return NULL;
   }

  if (!edje_object_file_set(edje, "edc/test00.edj", "my_group"))
   {
    int err = edje_object_load_error_get(edje);
    const char *errmsg = edje_load_error_str(err);
    EINA_LOG_ERR("could not load 'my_group' from .edj file : %s",
          errmsg);

    evas_object_del(edje);
    return NULL;
   }

  if (text)
   {
    if (!edje_object_part_text_set(edje, "text", text))
     {
      EINA_LOG_WARN("could not set the text. "
            "Maybe part 'text' does not exist?");
     }
   }

  evas_object_move(edje, 0, 0);
  evas_object_resize(edje, 320, 240);
  evas_object_show(edje);
  return edje;
}

Evas_Object*
create_panes(Evas_Object* win, Eina_Bool hor)
{
  Evas_Object* panes = elm_panes_add(win);
  elm_panes_horizontal_set(panes, hor);
  evas_object_size_hint_weight_set(panes, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(panes, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_show(panes);

  return panes;
}

static void
win_del(void *data, Evas_Object *obj, void *event_info)
{
  printf("win delete \n");
  view_destroy(view);
  elm_exit();
}

/*
static void
_change_scene(void *data,
      Evas_Object *obj,
      void *event_info)
{
  const char *name = data;
  printf("change scene : %s \n", name);
  view_scene_set(view, resource_scene_get(s_rm, name));
}

static Evas_Object*
_create_scene_menu(Evas_Object* win)
{
  Evas_Object* menu;

  menu = elm_menu_add(win);

  Eina_Iterator* it;
  Eina_Hash* hash = resource_scenes_get(s_rm);
  void *data;

  if (!hash) return NULL;

  it = eina_hash_iterator_tuple_new(hash);

  while (eina_iterator_next(it, &data)) {
    Eina_Hash_Tuple *t = data;
    const char* name = t->key;
    //const Scene* s = t->data;
    //printf("key, scene name : %s, %s\n", name, s->name);
    elm_menu_item_add(menu, NULL, NULL, name, _change_scene, name);
  }
  eina_iterator_free(it);

  return menu;
}


static void
_entry_clicked_cb(void *data, Evas_Object *obj, void *event)
{
  Evas_Object* win = evas_object_top_get(evas_object_evas_get(obj));
  Evas_Object* menu = _create_scene_menu(win);
  
  if (!menu) return;
  evas_object_show(menu);

  Evas_Coord x,y,w,h;
  evas_object_geometry_get(obj, &x, &y, &w, &h);
  elm_menu_move(menu, x, y);
  
}
*/



static void
create_window()
{
  Evas_Object *win;
  win = elm_win_util_standard_add("slime", "slime");
  elm_win_autodel_set(win, EINA_TRUE);
  evas_object_smart_callback_add(win, "delete,request", win_del, NULL);

  /*
  Evas_Object* box = elm_box_add(win);
  evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

  elm_win_resize_object_add(win, box);
  evas_object_show(box);

  Evas_Object* scene_entry = elm_entry_add(win);
  elm_object_text_set(scene_entry, "<b>Scene: </b>my test");
  elm_entry_single_line_set(scene_entry, EINA_TRUE);
  elm_entry_editable_set(scene_entry, EINA_FALSE);
  elm_entry_scrollable_set(scene_entry, EINA_TRUE);
  evas_object_smart_callback_add(scene_entry, "clicked", _entry_clicked_cb, NULL);

  evas_object_size_hint_align_set(scene_entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
  //evas_object_size_hint_weight_set(scene_entry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_show(scene_entry);
  elm_box_pack_end(box, scene_entry);
  */

  Evas_Object* panes = create_panes(win, false);
  //elm_box_pack_end(box, panes);
  elm_win_resize_object_add(win, panes);
  Evas_Object* hpanes = create_panes(win, false);
  Evas_Object* treepanes = create_panes(win, false);
  view = view_new(win);
  //view->scene_entry = scene_entry;
  Evas_Object* glview = view->glview;
  Evas_Object* property = view->property->root;
  Evas_Object* tree = view->tree->root;

  ResourceView* rv = resource_view_new(win, view);
  
  resource_view_update(rv);

  /*
   {
    Eina_Iterator* it;
    Eina_Hash* hash = resource_scenes_get(s_rm);

    if (hash) {

      it = eina_hash_iterator_tuple_new(hash);
      void *data;

      while (eina_iterator_next(it, &data)) {
        Eina_Hash_Tuple *t = data;
        //const char* name = t->key;
        const Scene* s = t->data;
        //printf("key, scene name : %s, %s\n", name, s->name);
        //elm_menu_item_add(menu, NULL, NULL, name, _change_scene, name);
        resource_view_scene_add(rv, s);
      }
      eina_iterator_free(it);
    }
   }
   */






  elm_object_part_content_set(treepanes, "left", rv->gl);
  elm_object_part_content_set(treepanes, "right", tree);


  elm_object_part_content_set(hpanes, "left", view->table);
  elm_object_part_content_set(hpanes, "right", property);

  //elm_object_part_content_set(panes, "left", tree);
  evas_object_hide(tree);
  elm_object_part_content_set(panes, "left", treepanes);
  elm_object_part_content_set(panes, "right", hpanes);

  elm_panes_content_left_size_set(panes, 0.20f);
  elm_panes_content_right_size_set(hpanes, 0.35f);

  //evas_object_resize(win, 800/3, 400/3);
  //evas_object_resize(win, 800, 200);
  evas_object_resize(win, 1600, 600);
  evas_object_show(win);


  //Evas* evas = evas_object_evas_get(win);
  //Evas_Object* edje = create_my_group(evas, "danceoff");
}

/*
static void
create_window_panels()
{
  Evas_Object *win;
  win = elm_win_util_standard_add("slime", "slime");
  elm_win_autodel_set(win, EINA_TRUE);
  evas_object_smart_callback_add(win, "delete,request", win_del, NULL);

  Evas_Object* box = elm_box_add(win);
  elm_box_horizontal_set(box, EINA_TRUE);
  evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_win_resize_object_add(win, box);
  evas_object_show(box);

  //view = create_view(win);
  view = create_view(box);
  Evas_Object* glview = view->glview;
  Evas_Object* property = view->property->root;
  Evas_Object* tree = view->tree->root;
  elm_win_resize_object_add(win, view->box);
  //elm_box_pack_end(box, view->box);

  Evas_Object* panel = elm_panel_add(box);
  elm_panel_orient_set(panel, ELM_PANEL_ORIENT_LEFT);
  evas_object_size_hint_weight_set(panel, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(panel, EVAS_HINT_FILL, EVAS_HINT_FILL);

  elm_object_content_set(panel, tree);
  elm_box_pack_end(box, panel);
  evas_object_show(panel);

  elm_box_pack_end(box, view->box);

  panel = elm_panel_add(box);
  elm_panel_orient_set(panel, ELM_PANEL_ORIENT_RIGHT);
  evas_object_size_hint_weight_set(panel, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(panel, EVAS_HINT_FILL, EVAS_HINT_FILL);

  elm_object_content_set(panel, property);
  elm_box_pack_end(box, panel);
  evas_object_show(panel);


  //evas_object_resize(win, 800/3, 400/3);
  //evas_object_resize(win, 800, 200);
  evas_object_resize(win, 1200, 400);
  evas_object_show(win);


  //Evas* evas = evas_object_evas_get(win);
  //Evas_Object* edje = create_my_group(evas, "danceoff");
}
*/


static Object*
_object_mesh_create(const char* file)
{
  Object* o = create_object();
  o->name = file;
  Component* meshcomp = create_component(&mesh_desc);
  MeshComponent* mc = meshcomp->data;
  mesh_component_shader_set_by_name(mc,"shader/simple.shader");

  TextureHandle* t = resource_texture_handle_new(s_rm, "model/ceil.png");
  shader_instance_texture_data_set(mc->shader_instance, "texture", t);

  UniformValue* uv = calloc(1, sizeof *uv);
  uv->type = UNIFORM_FLOAT;
  uv->value.f = 1.0f;
  shader_instance_uniform_data_set(mc->shader_instance, "testfloat", uv);

  uv = calloc(1, sizeof *uv);
  uv->type = UNIFORM_VEC3;
  uv->value.vec3 = vec3(0.2,-0.5, -1);
  shader_instance_uniform_data_set(mc->shader_instance, "light", uv);

  mesh_component_mesh_set_by_name(mc, file);

  object_add_component(o, meshcomp);

  return o;
}

static void
populate_scene(Control* c, Scene* s)
{
  Object* parent;
   {
    Object* o = _object_mesh_create("model/Cube.mesh");
    o->name = eina_stringshare_add("cubel");

    Vec3 t = {0,0,0};
    object_set_position(o, t);
    control_object_add(c, s, o);
    parent = o;
   }

   {
    Object* yep = _object_mesh_create("model/smallchar.mesh");
    yep->name = eina_stringshare_add("smallchar");
    //animation_play(yep, "walkquat", LOOP);

    Vec3 t2 = {-10,0,0};
    object_set_position(yep, t2);
    control_object_add(c,s,yep);
   }

   {
    Object* cam = create_object();
    cam->name = eina_stringshare_add("cameratest");
    Vec3 campos = {0,0,20};
    cam->position = campos;
    Component* compcam = create_component(&camera_desc);
    object_add_component(cam, compcam);

    control_object_add(c,s,cam);
    scene_camera_set(s,cam);
   }

   {
    Object* empty = create_object();
    empty->name = eina_stringshare_add("child");

    Component* meshcomp = create_component(&mesh_desc);
    MeshComponent* mc = meshcomp->data;
    mesh_component_shader_set_by_name(mc,"shader/simple.shader");

    mesh_component_mesh_set_by_name(mc, "model/smallchar.mesh");
    object_add_component(empty, meshcomp);
    empty->position = vec3(6,0,0);

    UniformValue* uv = calloc(1, sizeof *uv);
    uv->type = UNIFORM_FLOAT;
    uv->value.f = 0.5f;
    shader_instance_uniform_data_set(mc->shader_instance, "testfloat", uv);

    //control_object_add(c,s,empty);
    object_child_add(parent, empty);

    /* testing code, can be removed
    Component* cline = create_component(&line_desc);
    Line *line = cline->data; 
    line->camera = c->view->camera->camera_component;
    line_set_use_depth(line, true);
    line_add_box(line, mc->mesh->box, vec4(0,1,0,0.2));
    line->fixed  = true;
    line->boxtest = mc->mesh->box;
    object_add_component(empty, cline);
    */
   }


  //GLint bits;
  //gl->glGetIntegerv(GL_DEPTH_BITS, &bits);
  //printf("depth buffer %d\n\n", bits);
}


static void 
build_scene()
{
  Scene* s = scene_new();
  populate_scene(view->control, s);
  tree_scene_set(view->tree, s);

  //printf("scene ORIGINAL\n");
  //scene_print(s);
}

static void
gameviewtest()
{
  Evas_Object *win;
  win = elm_win_util_standard_add("slime", "slime");
  evas_object_resize(win, 800, 200);
  evas_object_show(win);
  elm_win_autodel_set(win, EINA_TRUE);
  evas_object_smart_callback_add(win, "delete,request", win_del, NULL);
  create_gameview(win);

}

EAPI_MAIN int
elm_main(int argc, char **argv)
{
  eina_init();
  eet_init();
  property_holder_init();

  s_component_manager = create_component_manager(); //TODO
  component_manager_load(s_component_manager);

  scene_descriptor_init();

  s_rm = resource_manager_create();
  resource_read_path(s_rm);

  resource_simple_mesh_create(s_rm);
  resource_shader_create(s_rm);
  resource_texture_create(s_rm);
  resource_load(s_rm);

  elm_config_preferred_engine_set("opengl_x11");
  //elm_config_focus_highlight_animate_set(EINA_TRUE);
  //elm_config_focus_highlight_enabled_set(EINA_TRUE);
  create_window();
  //create_window_panels();

  view_scene_set(view, resource_scene_get(s_rm, "base"));
  //build_scene();
  //gameviewtest();

  elm_run();
  elm_shutdown();
  resource_scenes_save();

  elm_config_preferred_engine_set(NULL);

  return 0;
}
ELM_MAIN()

