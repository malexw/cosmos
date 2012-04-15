#ifndef COSMOS_RENDERER_H_
#define COSMOS_RENDERER_H_

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif

#include <iostream>

#include "SDL/SDL.h"
#include "GL/gl.h"
#include "GL/glu.h"
#include "GL/glx.h"
#include "GL/glext.h"
//#include "GL/glxext.h"

#define glXGetProcAddress(x) (*glXGetProcAddress)((const GLubyte*)x)

struct UniformLocations {
  static const unsigned int MATRIX_BINDING = 0;
  static const unsigned int SHADOW_MATRIX_BINDING = 1;

  static const unsigned int DIFFUSE_TEXTURE_UNIT = 0;
  static const unsigned int BUMP_TEXTURE_UNIT = 1;
  static const unsigned int SHADOW_TEXTURE_UNIT = 3;
};

class GlRenderer {

 public:

  GlRenderer() {};

  static GLuint glGetUniformBlockIndex(GLuint program, const GLchar* block_name) {
    return ((PFNGLGETUNIFORMBLOCKINDEXPROC)(SDL_GL_GetProcAddress("glGetUniformBlockIndex")))(program, block_name);
  }

  static void glUniformBlockBinding(GLuint program, GLuint block_index, GLuint block_binding) {
    return ((PFNGLUNIFORMBLOCKBINDINGPROC)(SDL_GL_GetProcAddress("glUniformBlockBinding")))(program, block_index, block_binding);
  }

};

#endif
