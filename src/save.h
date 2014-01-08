#ifndef __SAVE__
#define __SAVE__
#include  "Eina.h"

typedef struct _Save Save;
struct _Save {
  const char* scene;
};


Save* save_read();
Eina_Bool save_write(const Save* s);
#endif
