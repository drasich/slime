#include "resource.h"
//#include "Ecore.h"

Mesh*
resource_mesh_get(ResourceManager* rm, const char* name)
{
  Mesh* m = eina_hash_find(rm->meshes, name);
  if (!m)
  printf("Cannot find mesh %s \n", name);

  return m;
}

Shader*
resource_shader_get(ResourceManager* rm, const char* name)
{
  Shader* s = eina_hash_find(rm->shaders, name);
  if (!s)
  printf("Cannot find shader %s \n", name);

  return s;
}

Texture*
resource_texture_get(ResourceManager* rm, const char* name)
{
  Texture* t = eina_hash_find(rm->textures, name);
  if (!t)
  printf("Cannot find texture %s \n", name);

  return t;
}


Eina_Hash*
resource_meshes_get(ResourceManager* rm)
{
  return rm->meshes;
}

Eina_Hash*
resource_shaders_get(ResourceManager* rm)
{
  return rm->shaders;
}

Eina_Hash*
resource_textures_get(ResourceManager* rm)
{
  return rm->textures;
}

Eina_Hash*
resource_scenes_get(ResourceManager* rm)
{
  return rm->scenes;
}



static void
_resource_mesh_add_cb(const char *name, const char *path, void *data)
{
  ResourceManager* rm = data;
  if (eina_str_has_extension(name,"mesh")) {
    printf("mesh %s in %s\n", name, path);
    rm->meshes_to_load = eina_list_append(rm->meshes_to_load, eina_stringshare_add(name));
  }
}

static void
_resource_scene_add_cb(const char *name, const char *path, void *data)
{
  ResourceManager* rm = data;
  if (eina_str_has_extension(name,"scene")) {
    int len = strlen(name) - strlen(".scene");
    char shortname[len+1];
    memcpy(shortname, name, len);
    shortname[len] = '\0';
    rm->scenes_to_load = eina_list_append(rm->scenes_to_load, eina_stringshare_add(shortname));
  }
}


void
resource_read_path(ResourceManager* rm)
{
  const char* dir = "model";


  /* with ecore
  Eina_List* files = ecore_file_ls(dir);

  Eina_List *l;
  const char *file;
  EINA_LIST_FOREACH(files, l, file) {
    if (eina_str_has_extension(file,"mesh"))
    printf("file : %s\n", file);
  }
  */

  eina_file_dir_list("model", EINA_FALSE, _resource_mesh_add_cb, rm);
  eina_file_dir_list("scene", EINA_FALSE, _resource_scene_add_cb, rm);
  //resource_load(rm);
}

static void 
_scene_free_cb(void *scene)
{
  printf("scene free cb \n");
  scene_del(scene);
}

ResourceManager*
resource_manager_create()
{
  ResourceManager* rm = calloc(1, sizeof *rm);
  rm->meshes = eina_hash_string_superfast_new(NULL);
  rm->shaders = eina_hash_string_superfast_new(NULL);
  rm->meshes_to_load = NULL;
  rm->textures = eina_hash_string_superfast_new(NULL);
  rm->scenes = eina_hash_string_superfast_new(_scene_free_cb);
  rm->scenes_to_load = NULL;
  return rm;

}

void
resource_scenes_clean()
{
  //eina_hash_free(s_rm->scenes);
  //s_rm->scenes = NULL;
  //s_rm->scenes = eina_hash_string_superfast_new(_scene_free_cb);

  printf("RRRR resources scenes_clean\n");
  eina_hash_free_buckets(s_rm->scenes);
}

void
resource_scenes_load()
{
  ResourceManager* rm = s_rm;
  Eina_List *l;
  const char *name;

  EINA_LIST_FOREACH(rm->scenes_to_load, l, name) {
    int l = strlen(name) + strlen("scene/");
    int l2 = l + strlen(".scene");
    char filepath[l2 + 1];
    eina_str_join(filepath, l + 1, '/', "scene" , name);
    eina_str_join(filepath, l2 + 1, '.', filepath, "scene" );
    filepath[l2] = '\0';

    Scene* s = scene_read(filepath);
    s->name = eina_stringshare_add(name);
    eina_hash_add(rm->scenes, s->name, s);
    scene_post_read(s);
  }

}

void resource_load(ResourceManager* rm)
{
  Eina_List *l;
  const char *name;
  const char *path = "model";
  EINA_LIST_FOREACH(rm->meshes_to_load, l, name) {
    int l = strlen(name) + strlen(path) + 2;
    char filepath[l];
    eina_str_join(filepath, l, '/', path , name);

    Mesh* m = mesh_new();
    mesh_file_set(m, filepath);
    eina_hash_add(rm->meshes, filepath, m);
  }

  resource_scenes_load();
}


bool
resource_scene_add(ResourceManager* rm, Scene* s)
{
  //if (eina_hash_find
  return eina_hash_add(rm->scenes, s->name, s);
}

void
resource_simple_mesh_create(ResourceManager* rm)
{
  Mesh* m = mesh_new();
  create_mesh_quad(m,1,1);
  m->name = "quad";
  eina_hash_add(rm->meshes, m->name, m);
}

void 
resource_shader_create(ResourceManager* rm)
{
  shader_descriptor_init();
  Shader* simple = create_shader("shader/simple.shader","shader/simple.vert", "shader/simple.frag");
  shader_attribute_add(simple, "vertex", 3, GL_FLOAT);
  shader_attribute_add(simple, "normal", 3, GL_FLOAT);
  shader_attribute_add(simple, "texcoord", 2, GL_FLOAT);
  shader_uniform_add(simple, "matrix", false);
  shader_uniform_add(simple, "normal_matrix", false);
  shader_uniform_type_add(simple, "texture", UNIFORM_TEXTURE, true);
  shader_uniform_type_add(simple, "testfloat", UNIFORM_FLOAT, true );
  shader_uniform_type_add(simple, "light", UNIFORM_VEC3, true);

  Shader* diffuse = create_shader("shader/diffuse.shader","shader/diffuse.vert", "shader/diffuse.frag");
  shader_attribute_add(diffuse, "vertex", 3, GL_FLOAT);
  shader_attribute_add(diffuse, "texcoord", 2, GL_FLOAT);
  shader_uniform_add(diffuse, "matrix", false);
  shader_uniform_type_add(diffuse, "texture", UNIFORM_TEXTURE, true);

  Shader* red = create_shader("shader/red.shader", "shader/simple.vert", "shader/red.frag");
  shader_attribute_add(red, "vertex", 3, GL_FLOAT);
  shader_attribute_add(red, "normal", 3, GL_FLOAT);
  shader_attribute_add(red, "texcoord", 2, GL_FLOAT);
  shader_uniform_add(red, "matrix", false);
  shader_uniform_add(red, "normal_matrix", false);
  shader_uniform_type_add(red, "texture", UNIFORM_TEXTURE, true);

  Shader* dragger = create_shader("shader/dragger.shader","shader/dragger.vert", "shader/dragger.frag");
  shader_attribute_add(dragger, "vertex", 3, GL_FLOAT);
  shader_uniform_add(dragger, "matrix", false);
  shader_uniform_type_add(dragger, "color", UNIFORM_VEC4, false);
  shader_write(dragger);

  //Shader* simple = shader_read("shader/simple.shader");
  //Shader* red = shader_read("shader/red.shader");


  Shader* line = create_shader("line", "shader/line.vert", "shader/line.frag");
  shader_attribute_add(line, "vertex", 3, GL_FLOAT);
  shader_attribute_add(line, "color", 4, GL_FLOAT);
  shader_uniform_add(line, "matrix", false);
  //shader_uniform_type_add(line, "texture", UNIFORM_TEXTURE, false); //TODO uncomment
  shader_uniform_type_add(line, "resolution", UNIFORM_VEC2, false);
  shader_uniform_type_add(line, "use_depth", UNIFORM_INT, true);
  shader_uniform_type_add(line, "size_fixed", UNIFORM_INT, true);

  eina_hash_add(rm->shaders, line->name, line);
  eina_hash_add(rm->shaders, simple->name, simple);
  eina_hash_add(rm->shaders, diffuse->name, diffuse);
  eina_hash_add(rm->shaders, red->name, red);
  eina_hash_add(rm->shaders, dragger->name, dragger);


  //shader_write(red);
  //shader_write(simple);
  //Shader* s = shader_read("shader/red.shader");
  //Shader* simplea = shader_read("shader/simple.shader");
}


void
resource_texture_create(ResourceManager* rm)
{
  const char* filetex = "model/ceil.png";
  //rm->textures_to_load = eina_list_append(rm->textures_to_load, eina_stringshare_add(filetex));
  Texture* tex = texture_new();
  tex->filename = filetex;
  texture_png_read(tex);
  eina_hash_add(rm->textures, filetex, tex);

  tex = texture_new();
  tex->filename = "model/test.png";
  texture_png_read(tex);
  eina_hash_add(rm->textures, tex->filename, tex);

  tex = texture_new();
  tex->filename = "model/red64.png";
  texture_png_read(tex);
  eina_hash_add(rm->textures, tex->filename, tex);

  tex = texture_new();
  tex->filename = "model/base_skeleton_col.png";
  texture_png_read(tex);
  eina_hash_add(rm->textures, tex->filename, tex);
}

/*
void
resource_handle_change(ResourceManager* rm, ResourceHandle* rh, const char* name)
{
  Resource* r = resource_get(rm, name);
  if (!r) return;

  if (r->is_loaded) {
    rh->resource = r;
    return;
  }
  else if (!rh->r) {
    //TODO
    //rh->r = resource_default_(rm);
  }

  ResourceWaiting rw = { rh, r};

  rm->waiting = eina_list_append(rm->waiting, rw);

}
*/

TextureHandle* texture_handle_new()
{
  TextureHandle* th = calloc(1, sizeof *th);
  return th;
}

TextureHandle* resource_texture_handle_new(ResourceManager* rm, const char* name)
{
  TextureHandle* th = calloc(1, sizeof *th);
  resource_texture_handle_set(rm, th, name);
  return th;
}

void
resource_texture_handle_set(ResourceManager* rm, TextureHandle* th, const char* name)
{
  th->name = name;
  Texture* t = resource_texture_get(rm, name);
  if (t)
  th->texture = t;
  else {
    //TODO waiting list
    /*
    ResourceWaiting rw = { rh, r};
    rm->waiting = eina_list_append(rm->waiting, rw);
    */
  }
}

/////////
MeshHandle* resource_mesh_handle_new(ResourceManager* rm, const char* name)
{
  MeshHandle* mh = calloc(1, sizeof *mh);
  resource_mesh_handle_set(rm, mh, name);
  return mh;
}

void
resource_mesh_handle_set(ResourceManager* rm, MeshHandle* mh, const char* name)
{
  mh->name = name;
  Mesh* m = resource_mesh_get(rm, name);
  if (m)
  mh->mesh = m;
  else {
    //TODO waiting list
    /*
    ResourceWaiting rw = { rh, r};
    rm->waiting = eina_list_append(rm->waiting, rw);
    */
  }
}

void
resource_shader_handle_set(ResourceManager* rm, ShaderHandle* sh, const char* name)
{
  //TODO choose between this state or the callback or both
  sh->state = RESOURCE_STATE_CHANGED;
  printf("resource shader handle set\n");
  if (sh->cb) sh->cb(sh->name, name, sh->data);

  sh->name = name;
  Shader* s = resource_shader_get(rm, name);
  if (s)
  sh->shader = s;
  else {
    //TODO waiting list
    /*
    ResourceWaiting rw = { rh, r};
    rm->waiting = eina_list_append(rm->waiting, rw);
    */
  }

}

Scene*
resource_scene_get(ResourceManager* rm, const char* name)
{
  Scene* s = eina_hash_find(rm->scenes, name);
  if (!s)
  printf("Cannot find scene %s \n", name);

  return s;

}

void
resource_scene_del(ResourceManager* rm, Scene* s)
{
  eina_hash_del_by_key(rm->scenes, s->name);
  scene_del(s);
  s = NULL;
}

void
resource_scene_save(const Scene* s)
{
  int l = strlen(s->name) + strlen("scene/") + strlen(".scene") + 1;
  char yep[l];
  char copy[l];
  eina_strlcpy(yep, "scene/", strlen("scene/") + 1);
  eina_strlcat(yep, s->name, l);
  eina_strlcpy(copy, yep, strlen(yep) + 1);
  eina_strlcat(yep, ".scene", l);
  eina_strlcat(copy, ".saved", l);
  printf("scene name : %s, yep %s, length %d, copy %s \n", s->name, yep, l, copy);
  eina_file_copy(
        yep,
        copy, 
        EINA_FILE_COPY_DATA | EINA_FILE_COPY_PERMISSION | EINA_FILE_COPY_XATTR,
        NULL,
        NULL);
  scene_write(s, yep);
}

void
resource_scenes_save()
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
      resource_scene_save(s);
    }
    eina_iterator_free(it);
  }
}

