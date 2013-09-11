#include "resource.h"
#include "Ecore.h"

Mesh*
resource_mesh_get(ResourceManager* rm, const char* name)
{
  Mesh* m = eina_hash_find(rm->meshes, name);
  if (m)
  printf("found mesh %s\n", m->name);
  else
  printf("not found mesh \n");

  return m;
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

