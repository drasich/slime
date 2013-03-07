#include "scene.h"
#include "gl.h"

Scene*
create_scene()
{
  Scene* s = calloc(1, sizeof(Scene));
  s->objects = NULL;
  s->ortho = NULL;
  eina_init();
  s->camera = create_camera();
  s->camera->object.name = "camera";
  //Vec3 v = {10,10,10};
  Vec3 v = {0,0,20};
  //Vec3 axis = {0,1,0};
  //Quat q = quat_angle_axis(3.14f/4.0f, axis);
  s->camera->object.Position = v;
  Vec3 at = {0,0,0};
  Vec3 up = {0,1,0};
  s->camera->object.Orientation = quat_lookat(v, at, up);

  gl->glGenTextures(1, &s->texture_depth_stencil_id);
	gl->glBindTexture(GL_TEXTURE_2D, s->texture_depth_stencil_id);
  gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


  //TODO texture resolution
  int width = 1200;
  int height = 400;
  /*
  gl->glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT,
        width,
        height,
        0,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        NULL);
        */
  gl->glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_STENCIL_OES,
        width,
        height,
        0,
        GL_DEPTH_STENCIL_OES,
        GL_UNSIGNED_INT_24_8_OES,
        NULL);

	gl->glBindTexture(GL_TEXTURE_2D, 0);

  gl->glGenRenderbuffers(1, &s->rb);
  gl->glBindRenderbuffer(GL_RENDERBUFFER, s->rb);
  gl->glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);
  gl->glBindRenderbuffer(GL_RENDERBUFFER, 0);
  
  gl->glGenFramebuffers(1, &s->fbo);
  gl->glBindFramebuffer(GL_FRAMEBUFFER, s->fbo);

  gl->glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        s->texture_depth_stencil_id,
        0);

  gl->glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_STENCIL_ATTACHMENT,
        GL_TEXTURE_2D,
        s->texture_depth_stencil_id,
        0);

  gl->glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, 
        GL_COLOR_ATTACHMENT0,
        GL_RENDERBUFFER,
        s->rb);

  GLenum e = gl->glCheckFramebufferStatus(GL_FRAMEBUFFER);

  if (e == GL_FRAMEBUFFER_COMPLETE) {
    printf("---->>>>>buffer complete \n");
  }

  gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return s;
}

void
scene_add_object(Scene* s, Object* o)
{
  s->objects = eina_list_append(s->objects, o);
  o->scene = s;
}

void
scene_add_object_ortho(Scene* s, Object* o)
{
  s->ortho = eina_list_append(s->ortho, o);
  o->scene = s;
}



void
scene_destroy(Scene* s)
{
  Eina_List *l;
  Object *o;

  EINA_LIST_FREE(s->objects, o) {
    object_destroy(o);
  }

  EINA_LIST_FREE(s->ortho, o) {
    object_destroy(o);
  }


  //TODO destroy camera

  free(s);
  eina_shutdown();
}

void
scene_draw(Scene* s)
{
  Matrix4 cam_mat_inv, mo;

  //TODO save the inverse camera and compute only if there was a change.
  mat4_inverse(((Object*)(s->camera))->matrix, cam_mat_inv);
  Matrix4* projection = &s->camera->projection;
  Matrix4* ortho = &s->camera->orthographic;

  gl->glBindTexture(GL_TEXTURE_2D, 0);
  gl->glBindFramebuffer(GL_FRAMEBUFFER, s->fbo);

  gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT) ;
  //gl->glClearStencil(0);
  gl->glEnable(GL_STENCIL_TEST);
  gl->glStencilFunc(GL_ALWAYS, 0x1, 0x1);
  gl->glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(s->objects, l, o) {
    object_compute_matrix(o, mo);
    mat4_multiply(cam_mat_inv, mo, mo);
    object_draw(o, mo, *projection);
  }

  //TODO : test, can be removed
  //GLuint mypixels[w*h];
  //gl->glReadPixels(0, 0, w, h, GL_DEPTH_STENCIL_OES, GL_UNSIGNED_INT_24_8_OES, mypixels);
  //save_png(mypixels);


  gl->glBindFramebuffer(GL_FRAMEBUFFER,0);

  EINA_LIST_FOREACH(s->objects, l, o) {
    object_compute_matrix(o, mo);
    mat4_multiply(cam_mat_inv, mo, mo);
    object_draw(o, mo, *projection);
  }

  //TODO avoid compute matrix 2 times
  gl->glClear(GL_DEPTH_BUFFER_BIT);
  EINA_LIST_FOREACH(s->objects, l, o) {
    object_compute_matrix(o, mo);
    mat4_multiply(cam_mat_inv, mo, mo);
    //TODO Fix how to use depth texture for lines
    //if (o->line != NULL) o->line->id_texture = s->texture_depth_stencil_id;
    //object_draw_lines(o, mo, *projection);
  }

  //* TODO ortho
  gl->glClear(GL_DEPTH_BUFFER_BIT);
  EINA_LIST_FOREACH(s->ortho, l, o) {
    object_compute_matrix(o, mo);
    //mat4_multiply(cam_mat_inv, mo, mo);
    object_draw(o, mo, *ortho);
  }
  //*/

}

void
scene_update(Scene* s)
{
  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(s->objects, l, o)
    object_update(o);

  //TODO only update camera if there is a movement
  object_update((Object*)s->camera);
}

