#include "resource.h"
#include "Ecore.h"

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


static void
_print_cb(const char *name, const char *path, void *data)
{
  ResourceManager* rm = data;
  if (eina_str_has_extension(name,"mesh")) {
    printf("file %s in %s\n", name, path);
    rm->meshes_to_load = eina_list_append(rm->meshes_to_load, eina_stringshare_add(name));
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

  eina_file_dir_list(dir, EINA_FALSE, _print_cb, rm);
  resource_load(rm);
}

ResourceManager*
resource_manager_create()
{
  ResourceManager* rm = calloc(1, sizeof *rm);
  rm->meshes = eina_hash_string_superfast_new(NULL);
  rm->shaders = eina_hash_string_superfast_new(NULL);
  rm->meshes_to_load = NULL;
  rm->textures = eina_hash_string_superfast_new(NULL);
  return rm;

}

void resource_load(ResourceManager* rm)
{
  //TODO read meshes_to_load and add meshes to the hash.
  Eina_List *l;
  const char *name;
  const char *path = "model";
  EINA_LIST_FOREACH(rm->meshes_to_load, l, name) {
    printf("load name is %s\n", (char*) name);
    int l = strlen(name) + strlen(path) + 2;
    char filepath[l];
    eina_str_join(filepath, l, '/', path , name);
    printf("l is %d, filepath is %s \n", l, filepath);

    Mesh* m = mesh_create();
    mesh_file_set(m, filepath);
    eina_hash_add(rm->meshes, filepath, m);
  }
}

void
resource_simple_mesh_create(ResourceManager* rm)
{
  Mesh* m = mesh_create();
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
  shader_uniform_add(simple, "matrix");
  shader_uniform_add(simple, "normal_matrix");
  shader_uniform_type_add(simple, "texture", UNIFORM_TEXTURE);
  shader_uniform_type_add(simple, "testfloat", UNIFORM_FLOAT);
  shader_uniform_type_add(simple, "light", UNIFORM_VEC3);

  Shader* red = create_shader("shader/red.shader", "shader/simple.vert", "shader/red.frag");
  shader_attribute_add(red, "vertex", 3, GL_FLOAT);
  shader_attribute_add(red, "normal", 3, GL_FLOAT);
  shader_attribute_add(red, "texcoord", 2, GL_FLOAT);
  shader_uniform_add(red, "matrix");
  shader_uniform_add(red, "normal_matrix");
  shader_uniform_type_add(red, "texture", UNIFORM_TEXTURE);

  Shader* dragger = create_shader("shader/dragger.shader","shader/dragger.vert", "shader/dragger.frag");
  shader_attribute_add(dragger, "vertex", 3, GL_FLOAT);
  shader_uniform_add(dragger, "matrix");
  shader_uniform_type_add(dragger, "color", UNIFORM_VEC4 );
  shader_write(dragger);

  //Shader* simple = shader_read("shader/simple.shader");
  //Shader* red = shader_read("shader/red.shader");

  eina_hash_add(rm->shaders, simple->name, simple);
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

