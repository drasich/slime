#include "scene.h"
#include "gl.h"

Scene*
create_scene()
{
  Scene* s = calloc(1, sizeof(Scene));
  s->objects = NULL;
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
  //mat4_set_perspective(s->camera->projection, M_PI/4.0, 1.6 ,1,1000.0f);

  printf(">>>>>>>>>>>>>>next thing to do : how to send the depth texture to line <<<<<<<<<<<<<<<\n");

  gl->glGenTextures(1, &s->id_texture);
	gl->glBindTexture(GL_TEXTURE_2D, s->id_texture);
  gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

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
        s->id_texture,
        0);

  gl->glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_STENCIL_ATTACHMENT,
        GL_TEXTURE_2D,
        s->id_texture,
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
scene_destroy(Scene* s)
{
  Eina_List *l;
  Object *o;

  EINA_LIST_FREE(s->objects, o) {
    object_destroy(o);
  }

  //TODO destroy camera

  free(s);
  eina_shutdown();
}

void
scene_draw(Scene* s, int w, int h)
{
  Matrix4 mt, mr, cam_mat, mo;

  mat4_set_translation(mt, s->camera->object.Position);
  mat4_set_rotation_quat(mr, s->camera->object.Orientation);
  //Quat q = s->camera->object.Orientation;
  //printf("camera rot %f, %f, %f, %f \n", q.X, q.Y, q.Z, q.W);
  mat4_multiply(mt, mr, cam_mat);
  mat4_inverse(cam_mat, cam_mat);

  //TODO get values from camera
  float aspect = (float)w/(float)h;
  Matrix4 projection;
  //mat4_set_frustum(projection, -aspect*1,aspect*1,-1,1,1,1000.0f);
  mat4_set_perspective(projection, M_PI/4.0, aspect ,1.0f,1000.0f);

  gl->glBindTexture(GL_TEXTURE_2D, 0);
  gl->glBindFramebuffer(GL_FRAMEBUFFER, s->fbo);
	//gl->glBindTexture(GL_TEXTURE_2D, s->id_texture);

  //gl->glBindRenderbuffer(GL_RENDERBUFFER, s->rb);
  gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT) ;
  //gl->glClearStencil(0);
  gl->glEnable(GL_STENCIL_TEST);
  gl->glStencilFunc(GL_ALWAYS, 0x1, 0x1);
  gl->glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);


  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(s->objects, l, o) {
    object_compute_matrix(o, mo);
    mat4_multiply(cam_mat, mo, mo);
    object_draw(o, mo, projection);
  }

  //TODO : test, can be removed
  //GLuint mypixels[w*h];
  //gl->glReadPixels(0, 0, w, h, GL_DEPTH_STENCIL_OES, GL_UNSIGNED_INT_24_8_OES, mypixels);
  //save_png(mypixels);


  gl->glBindFramebuffer(GL_FRAMEBUFFER,0);
  //gl->glBindRenderbuffer(GL_RENDERBUFFER,0);
	//gl->glBindTexture(GL_TEXTURE_2D, 0);
  //gl->glDrawBuffer(GL_BACK);
  //gl->glReadBuffer(GL_BACK);

  EINA_LIST_FOREACH(s->objects, l, o) {
    object_compute_matrix(o, mo);
    mat4_multiply(cam_mat, mo, mo);
    object_draw(o, mo, projection);
  }

  //TODO avoid compute matrix 2 times
  gl->glClear(GL_DEPTH_BUFFER_BIT);
  EINA_LIST_FOREACH(s->objects, l, o) {
    object_compute_matrix(o, mo);
    mat4_multiply(cam_mat, mo, mo);
    //TODO Fix this
    o->line->id_texture = s->id_texture;
    object_draw_lines(o, mo, projection);
  }

}

void
scene_update(Scene* s)
{
  Eina_List *l;
  Object *o;
  EINA_LIST_FOREACH(s->objects, l, o)
    object_update(o);
}

