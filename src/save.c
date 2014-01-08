#include "save.h"
#include "Eet.h"

static Eet_Data_Descriptor *_save_descriptor = NULL;
static const char SAVE_FILE_ENTRY[] = "save";

static void
_save_descriptor_init(void)
{
  Eet_Data_Descriptor_Class eddc;

  EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Save);
  _save_descriptor = eet_data_descriptor_stream_new(&eddc);

#define ADD_BASIC(member, eet_type) \
  EET_DATA_DESCRIPTOR_ADD_BASIC             \
  (_save_descriptor, Save, # member, member, eet_type)

  ADD_BASIC(scene, EET_T_STRING);

#undef ADD_BASIC
}

static const char* SAVE_FILE = "save.eet";

Eina_Bool
save_write(const Save* s)
{
  if (!_save_descriptor) _save_descriptor_init();

  Eina_Bool ret;
  Eet_File *ef = eet_open(SAVE_FILE, EET_FILE_MODE_WRITE);
  if (!ef) {
    fprintf(stderr, "error reading file %s \n", SAVE_FILE);
    return EINA_FALSE;
  }

  ret = eet_data_write(ef, _save_descriptor, SAVE_FILE_ENTRY, s, EINA_TRUE);
  eet_close(ef);
  if (ret) {
    printf("return value for save looks ok \n");
  }
  else
    printf("return value for save NOT OK \n");

  return ret;
}

static void 
_output(void *data, const char *string)
{
  printf("%s", string);
}


Save*
save_read()
{
  if (!_save_descriptor) _save_descriptor_init();
  Save* s;

  Eet_File *ef = eet_open(SAVE_FILE, EET_FILE_MODE_READ);
  if (!ef) {
    fprintf(stderr, "error reading file %s \n", SAVE_FILE);
    return NULL;
  }

  s = eet_data_read(ef, _save_descriptor, SAVE_FILE_ENTRY);
  printf("save read data dump\n");
  eet_data_dump(ef, SAVE_FILE_ENTRY, _output, NULL);
  printf("save read data dump end\n");
  eet_close(ef);

  if (s) {
    printf("Save scene: %s \n", s->scene);
  }
  else
  printf("s is null\n");
 
  return s;  
}

