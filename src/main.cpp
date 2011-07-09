#include <iostream>

#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"

#include "ResourceManager/ResourceManager.hpp"
#include "Timer.hpp"

//Screen attributes
const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 600;
const int SCREEN_BPP = 32;

//The frame rate
const int FRAMES_PER_SECOND = 60;

int main(int argc, char* argv[]) {
  ResourceManager::get();

  if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
  {
      //error
  }

  SDL_Surface* sdlSurface_ = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_OPENGL );
  /*if( SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_OPENGL ) == NULL )
  {
      //error
  }*/

  glShadeModel(GL_SMOOTH);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(2.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_COLOR_MATERIAL);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glAlphaFunc(GL_GREATER, 0.1);
  glEnable(GL_ALPHA_TEST);

  /*glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);*/
  //GLfloat ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (GLfloat)SCREEN_WIDTH/(GLfloat)SCREEN_HEIGHT, 1.0f, 100.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  if( glGetError() != GL_NO_ERROR )
  {
      //error
  }
  /*if( init_gl() == false )
  {
      //error
  }*/

  SDL_WM_SetCaption( "Cosmos", NULL );

  Timer::ShPtr fps_(new Timer());

  while(1) {
    fps_->start();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    //glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY_EXT);
    
    glTranslatef(0.0f, 0.0f, -10.0f);
    glColor3f(1.0f,0.0f,0.0f);
    GLUquadricObj* quadratic_ = gluNewQuadric();
    gluQuadricNormals(quadratic_, GLU_SMOOTH);
    //gluQuadricTexture(quadratic_, GL_TRUE);
    gluSphere(quadratic_,0.5f,32,32);

    SDL_GL_SwapBuffers();

    //Cap the frame rate
    if( fps_->get_ticks() < 1000 / FRAMES_PER_SECOND )
    {
      SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps_->get_ticks() );
    }
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
  }
  
  
  return 0;
}
