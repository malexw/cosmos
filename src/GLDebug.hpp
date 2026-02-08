#ifndef COSMOS_GLDEBUG_HPP_
#define COSMOS_GLDEBUG_HPP_

#include <iostream>

#include <SDL2/SDL_opengl.h>

#include "CosmosConfig.hpp"

inline const char* glErrorString(GLenum err) {
    switch (err) {
        case GL_NO_ERROR:          return "GL_NO_ERROR";
        case GL_INVALID_ENUM:      return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:     return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
        case GL_OUT_OF_MEMORY:     return "GL_OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
        default:                   return "Unknown GL error";
    }
}

inline void glCheckError_(const char* file, int line, const char* expr) {
    if (!CosmosConfig::get().is_gl_debug()) return;

    static int error_count = 0;
    static const int max_errors = 10;

    GLenum err = glGetError();
    while (err != GL_NO_ERROR) {
        if (error_count < max_errors) {
            std::cout << "GL Error [" << file << ":" << line << "] "
                      << expr << " -> " << glErrorString(err) << std::endl;
            ++error_count;
            if (error_count == max_errors) {
                std::cout << "GL Error limit reached, suppressing further errors" << std::endl;
                CosmosConfig::get().set_gl_debug(false);
            }
        }
        err = glGetError();
    }
}

#define GL_CHECK(expr) do { expr; glCheckError_(__FILE__, __LINE__, #expr); } while(0)
#define GL_CHECK_ERRORS() glCheckError_(__FILE__, __LINE__, "check")

#endif
