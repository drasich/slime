#include "log.h"
#include "Eet.h"

void
log_domains_register()
{
  log_shader_dom = eina_log_domain_register("shader", EINA_COLOR_LIGHTBLUE);
  log_mesh_dom = eina_log_domain_register("mesh", EINA_COLOR_LIGHTRED);
}
