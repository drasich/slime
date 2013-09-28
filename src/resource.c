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
  /*
  Shader* simple = create_shader("shader/simple.shader","shader/simple.vert", "shader/simple.frag");
  simple->has_vertex = true;
  simple->has_normal = true;
  simple->has_texcoord = true;
  simple->has_uniform_normal_matrix = true;

  Shader* red = create_shader("shader/red.shader", "shader/simple.vert", "shader/red.frag");
  red->has_vertex = true;
  red->has_normal = true;
  red->has_texcoord = true;
  red->has_uniform_normal_matrix = true;
  */

  Shader* dragger = create_shader("shader/dragger.shader","shader/dragger.vert", "shader/dragger.frag");
  //Shader* dragger = create_shader("shader/dragger.shader","shader/simple.vert", "shader/simple.frag");
  dragger->has_vertex = true;
  shader_write(dragger);

  Shader* simple = shader_read("shader/simple.shader");
  Shader* red = shader_read("shader/red.shader");

  eina_hash_add(rm->shaders, simple->name, simple);
  eina_hash_add(rm->shaders, red->name, red);
  eina_hash_add(rm->shaders, dragger->name, dragger);
  //shader_write(red);
  //shader_write(simple);
  //Shader* s = shader_read("shader/red.shader");
  Shader* simplea = shader_read("shader/simple.shader");
}

