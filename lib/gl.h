#ifndef __gl__
#define __gl__

#if defined EVAS_GL

#include "Evas_GL.h"
Evas_GL_API* gl;

#define glGetAttribLocation gl->glGetAttribLocation
#define glGetUniformLocation gl->glGetUniformLocation
#define glCreateShader gl->glCreateShader
#define glShaderSource gl->glShaderSource
#define glCompileShader gl->glCompileShader
#define glGetShaderiv gl->glGetShaderiv
#define glGetShaderInfoLog gl->glGetShaderInfoLog
#define glCreateProgram gl->glCreateProgram
#define glAttachShader gl->glAttachShader
#define glLinkProgram gl->glLinkProgram
#define glGetProgramiv gl->glGetProgramiv
#define glGetProgramInfoLog gl->glGetProgramInfoLog
#define glUseProgram gl->glUseProgram
#define glDeleteShader gl->glDeleteShader
#define glDeleteProgram gl->glDeleteProgram
#define glUniform1i gl->glUniform1i
#define glActiveTexture gl->glActiveTexture
#define glBindTexture gl->glBindTexture
#define glUniform1f gl->glUniform1f
#define glUniform3f gl->glUniform3f
#define glUniform4f gl->glUniform4f
#define glBindBuffer gl->glBindBuffer
#define glEnableVertexAttribArray gl->glEnableVertexAttribArray
#define glVertexAttribPointer gl->glVertexAttribPointer
#define glDrawElements gl->glDrawElements
#define glDrawArrays gl->glDrawArrays
#define glDisableVertexAttribArray gl->glDisableVertexAttribArray
#define glUniformMatrix4fv gl->glUniformMatrix4fv
#define glUniformMatrix3fv gl->glUniformMatrix3fv
#define glGenTextures gl->glGenTextures
#define glTexParameterf gl->glTexParameterf
#define glTexImage2D gl->glTexImage2D
#define glGenBuffers gl->glGenBuffers
#define glBufferData gl->glBufferData
#define glBufferSubData gl->glBufferSubData
#define glUniform2f gl->glUniform2f
#define glDeleteBuffers gl->glDeleteBuffers
#define glEnable gl->glEnable
#define glDepthFunc gl->glDepthFunc
#define glClearDepthf gl->glClearDepthf
#define glClearStencil gl->glClearStencil
#define glViewport gl->glViewport
#define glClearColor gl->glClearColor
#define glClear gl->glClear
#define glBlendFunc gl->glBlendFunc
#define glFinish gl->glFinish
#define glStencilFunc gl->glStencilFunc
#define glStencilOp gl->glStencilOp
#define glGenRenderbuffers gl->glGenRenderbuffers
#define glBindRenderbuffer gl->glBindRenderbuffer
#define glRenderbufferStorage gl->glRenderbufferStorage
#define glBindRenderbuffer gl->glBindRenderbuffer
#define glGenFramebuffers gl->glGenFramebuffers
#define glBindFramebuffer gl->glBindFramebuffer
#define glFramebufferTexture2D gl->glFramebufferTexture2D
#define glCheckFramebufferStatus gl->glCheckFramebufferStatus

#else

#include "GLES2/gl2.h"

#endif

#endif

