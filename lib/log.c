#include "log.h"
#include "Eet.h"

void
log_domains_register()
{
  log_shader_dom = eina_log_domain_register("shader", EINA_COLOR_LIGHTBLUE);
  log_mesh_dom = eina_log_domain_register("mesh", EINA_COLOR_LIGHTRED);
  log_scene_dom = eina_log_domain_register("scene", EINA_COLOR_LIGHTCYAN);
  log_object_dom = eina_log_domain_register("object", EINA_COLOR_GREEN);
  log_render_dom = eina_log_domain_register("render", EINA_COLOR_ORANGE);
  log_prefab_dom = eina_log_domain_register("prefab", EINA_COLOR_ORANGE);
  log_armature_dom = eina_log_domain_register("armature", EINA_COLOR_ORANGE);
}
