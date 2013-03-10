#include "fbo.h"

Fbo* 
create_fbo()
{
  Fbo* f = calloc(1, sizeof *f);

  gl->glGenTextures(1, &f->texture_depth_stencil_id);
  gl->glBindTexture(GL_TEXTURE_2D, f->texture_depth_stencil_id);
  gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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

  gl->glGenTextures(1, &f->texture_color);
  gl->glBindTexture(GL_TEXTURE_2D, f->texture_color);
  gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  gl->glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        width,
        height,
        0,
        GL_RGB, //GL_DEPTH_STENCIL_OES,
        GL_UNSIGNED_SHORT_5_6_5, //GL_UNSIGNED_INT_24_8_OES,
        NULL);

	gl->glBindTexture(GL_TEXTURE_2D, 0);


  gl->glGenRenderbuffers(1, &f->rb);
  gl->glBindRenderbuffer(GL_RENDERBUFFER, f->rb);
  gl->glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);
  //gl->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, width, height);
  gl->glBindRenderbuffer(GL_RENDERBUFFER, 0);

  gl->glGenFramebuffers(1, &f->fbo);
  gl->glBindFramebuffer(GL_FRAMEBUFFER, f->fbo);

  gl->glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        f->texture_depth_stencil_id,
        0);

  gl->glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_STENCIL_ATTACHMENT,
        GL_TEXTURE_2D,
        f->texture_depth_stencil_id,
        0);

  gl->glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        f->texture_color,
        0);

  /*
  gl->glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, 
        GL_COLOR_ATTACHMENT0,
        GL_RENDERBUFFER,
        f->rb);
        */


  GLenum e = gl->glCheckFramebufferStatus(GL_FRAMEBUFFER);

  if (e == GL_FRAMEBUFFER_COMPLETE) {
    printf("---->>>>>buffer complete \n");
  }
  else
    printf("---->>>>>buffer NOOOOOOOOOOOOOOOT complete \n");

  gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);


  return f;
}

void 
fbo_use(Fbo* f)
{
  gl->glBindTexture(GL_TEXTURE_2D, 0);
  gl->glBindFramebuffer(GL_FRAMEBUFFER, f->fbo);

}

void
fbo_use_end()
{
  gl->glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void
fbo_resize(Fbo* f, int w, int h)
{
	gl->glBindTexture(GL_TEXTURE_2D, f->texture_depth_stencil_id);

  gl->glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_STENCIL_OES,
        w,
        h,
        0,
        GL_DEPTH_STENCIL_OES,
        GL_UNSIGNED_INT_24_8_OES,
        NULL);

	gl->glBindTexture(GL_TEXTURE_2D, 0);

	gl->glBindTexture(GL_TEXTURE_2D, f->texture_color);
  gl->glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        w,
        h,
        0,
        GL_RGB,
        GL_UNSIGNED_SHORT_5_6_5,
        NULL);

	gl->glBindTexture(GL_TEXTURE_2D, 0);

  gl->glBindRenderbuffer(GL_RENDERBUFFER, f->rb);
  gl->glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, w, h);
  //gl->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, w, h);
  gl->glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void
fbo_destroy(Fbo* s)
{
  //TODO

}

