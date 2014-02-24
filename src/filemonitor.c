#include "filemonitor.h"
#include "Ecore_File.h"
#include "resource.h"

void monitor_model(void *data, Ecore_File_Monitor *em, Ecore_File_Event event, const char *path)
{
  printf("model event : %d, path %s \n", event, path);
}

void monitor_shader(void *data, Ecore_File_Monitor *em, Ecore_File_Event event, const char *path)
{
  printf("shader event : %d, path %s \n", event, path);
  resource_shader_update(data, path);
}


void
filemonitor_init(ResourceManager* rm)
{
  ecore_file_init(); //TODO ecore_file_shutdown
  const char* path = "model";
  Ecore_File_Monitor * efm = ecore_file_monitor_add( path,
        monitor_model,
        NULL
        );

  path = "shader";
  efm = ecore_file_monitor_add( path,
        monitor_shader,
        rm
        );
}
