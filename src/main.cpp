#include <iostream>

#include <boost/pointer_cast.hpp>

#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"

#include "PlayerInputHandler.hpp"
#include "InputManager.hpp"
#include "Camera.hpp"
#include "GameObject.hpp"
#include "Material.hpp"
#include "Quaternion.hpp"
#include "Renderable.hpp"
#include "ResourceManager/ResourceManager.hpp"
#include "ResourceManager/TextureManager.hpp"
#include "ResourceManager/MaterialManager.hpp"
#include "ResourceManager/MeshManager.hpp"
#include "ResourceManager/ShaderManager.hpp"
#include "Timer.hpp"
#include "Transform.hpp"
#include "Vector3f.hpp"
#include "World.hpp"

//Screen attributes
const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 600;
const int SCREEN_BPP = 32;

//The frame rate
const int FRAMES_PER_SECOND = 60;
const float US_PER_FRAME = 1000000/FRAMES_PER_SECOND;

int main(int argc, char* argv[]) {

  if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
  {
      //error
  }

  SDL_Surface* sdlSurface_ = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_OPENGL );
  /*if( SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_OPENGL ) == NULL )
  {
      //error
  }*/
  SDL_WM_SetCaption( "Cosmos", NULL );

  glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_CULL_FACE);
  
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.1);
  
  glShadeModel(GL_SMOOTH);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(2.0f);

  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  if( glGetError() != GL_NO_ERROR )
  {
      //error
  }

  ResourceManager::get();
  std::cout << "Resources loaded" << std::endl;

  // ----------- LIGHTING ----------------------------------------------
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  //GLfloat lightVals[4] = {0.2f, 0.2f, 0.2f, 1.0f};
  GLfloat lightVals[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightVals);

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightVals);
    
  lightVals[0] = 1.0f; lightVals[1] = 1.0f; lightVals[2] = 1.0f; lightVals[3] = 1.0f;
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightVals);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lightVals);

  //Vector3f(5, 0, -30)-Vector3f(5, 15, 5)
  lightVals[0] = 0.0f; lightVals[1] = -15.0f; lightVals[2] = -30.0f; lightVals[3] = 0.0f;
  //lightVals[0] = 5.0f; lightVals[1] = 15.0f; lightVals[2] = 5.0f; lightVals[3] = 0.0f;
  //glLightfv(GL_LIGHT0, GL_POSITION, lightVals);

  // --------------
  Timer::ShPtr fps_(new Timer());

  // -------------- CAMERA ---------------------------------------------
  Camera::ShPtr cam(new Camera());
  cam->set_direction(Vector3f(0.0f, -1.0f, -1.0f));
  cam->set_translate(Vector3f(0.0f, 10.0f, 20.0f));

  //cam->set_direction(Vector3f(2, 0, -10)-Vector3f(32, 20, 0));
  //cam->set_translate(Vector3f(32, 20, 0));

  // -------------- SHADOWS --------------------------------------------
  GLuint shadowBuffer;
  
  // create a framebuffer object
  glGenFramebuffersEXT(1, &shadowBuffer);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, shadowBuffer);
  
  // Instruct openGL that we won't bind a color texture with the currently binded FBO
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  
  // attach the texture to FBO depth attachment point
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, TextureManager::get().get_texture("shadow_map")->get_index(), 0);
  
  // check FBO status
  /*FBOstatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  if(FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT)
    printf("GL_FRAMEBUFFER_COMPLETE_EXT failed, CANNOT use FBO\n");*/
  
  // switch back to window-system-provided framebuffer
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  // -------------- OBJECTS --------------------------------------------
  Mesh::ShPtr c = MeshManager::get().get_mesh("res/meshes/cube.obj");
  Mesh::ShPtr q = MeshManager::get().get_mesh("res/meshes/face-center-quad.obj");
  Material::ShPtr m = MaterialManager::get().get_material("res/materials/default.mtl");
  float r = 0.0f;
  
  GameObject::ShPtr ob(new GameObject());
  Renderable::ShPtr ren(new Renderable(ob->id()));
  Transform::ShPtr tran(new Transform(ob->id()));
  ren->set_mesh(c).set_material(m);
  tran->set_translate(Vector3f(5.0f, 10.0f, 10.0f));
  
  GameObject::ShPtr ob2(new GameObject());
  Renderable::ShPtr ren2(new Renderable(ob2->id()));
  Transform::ShPtr tran2(new Transform(ob2->id()));
  ren2->set_mesh(c).set_material(m);
  tran2->translate(Vector3f(2.0f, 1.0f, -12.0f));
  //std::cout << Quaternion(Vector3f(0.0f, 1.0f, 0.0f), 1.0f)*Quaternion(Vector3f(1.0f, 0.0f, 0.0f), 0.5f) << std::endl;
  
  // ----------------- WORLD -------------------------------------------
  World::ShPtr w(new World(std::string("res/worlds/bigbrother.obj")));
  
  // ----------------- INPUT -------------------------------------------
  InputManager im;
  PlayerInputHandler::ShPtr pih(new PlayerInputHandler(ob->id()));
  InputHandler::ShPtr ih(boost::dynamic_pointer_cast<InputHandler>(pih));
  im.pushHandler(ih);
  pih->listener(cam);
  
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY_EXT);

  while(1) {
    fps_->frame_start();
    im.handleInput();
    cam->update(fps_->frame_delta());
    r += 1.0f;

    //-------------- First pass for shadows
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,shadowBuffer);	//Rendering offscreen
      glUseProgram(0);
      glViewport(0,0,1024,1024);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glCullFace(GL_FRONT);
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluPerspective(45,1,10,40000);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glMultMatrixf(Camera::matrixFromPositionDirection(Vector3f(5, 15, 5), Vector3f(5, 0, -30)-Vector3f(5, 15, 5)).to_array());

      w->draw_geometry();

      glPushMatrix();
      glTranslatef(2.0f,1.0f,-12.0f);
      glRotatef(r, 0.0f, 1.0f, 0.0f);
      glRotatef(r/2, 1.0f, 0.0f, 0.0f);
      glMatrixMode(GL_TEXTURE);
      glPushMatrix();
      glTranslatef(2.0f,1.0f,-12.0f);
      glRotatef(r, 0.0f, 1.0f, 0.0f);
      glRotatef(r/2, 1.0f, 0.0f, 0.0f);
      //
      ren2->draw_geometry();
      //
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
      ////
      //setTextureMatrix();
        double modelView[16];
        double projection[16];

        const GLdouble bias[16] = {	
        0.5, 0.0, 0.0, 0.0, 
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0};

        glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);

        glMatrixMode(GL_TEXTURE);
        glActiveTexture(GL_TEXTURE3);

        glLoadIdentity();	
        glLoadMatrixd(bias);

        glMultMatrixd(projection);
        glMultMatrixd(modelView);

        glMatrixMode(GL_MODELVIEW);
      
      //------------------- Second pass to actually draw things
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
      glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
      // MUST call glColorMask BEFORE glClear or things get explodey
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
      glCullFace(GL_BACK);

      ShaderManager::get().get_shader_program("shadow")->run();
      glActiveTexture(GL_TEXTURE3);
      glBindTexture(GL_TEXTURE_2D,TextureManager::get().get_texture("shadow_map")->get_index());

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluPerspective(45,(static_cast<float>(SCREEN_WIDTH)/static_cast<float>(SCREEN_HEIGHT)),1,4000);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      cam->apply();
      // Remember kids, always apply your lights *after* the camera transform
      glLightfv(GL_LIGHT0, GL_POSITION, lightVals);

      glActiveTexture(GL_TEXTURE0);
      w->draw();

      glPushMatrix();
      glTranslatef(2.0f,1.0f,-12.0f);
      glRotatef(r, 0.0f, 1.0f, 0.0f);
      glRotatef(r/2, 1.0f, 0.0f, 0.0f);
      glMatrixMode(GL_TEXTURE);
      glPushMatrix();
      glTranslatef(2.0f,1.0f,-12.0f);
      glRotatef(r, 0.0f, 1.0f, 0.0f);
      glRotatef(r/2, 1.0f, 0.0f, 0.0f);
      //
      ren2->render();
      //
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
      ////
    //////////////////////////////////////////
    SDL_GL_SwapBuffers();
    //Cap the frame rate
    fps_->frame_stop();
    if( fps_->frame_length() < US_PER_FRAME )
    {
      //std::cout << "Waiting " << static_cast<int>(US_PER_FRAME - fps_->frame_length()) / 1000 << " ms" << std::endl;
      SDL_Delay(static_cast<int>(US_PER_FRAME - fps_->frame_length()) / 1000);
    }
  }  
  
  return 0;
}
