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

  ResourceManager::get();

  Timer::ShPtr fps_(new Timer());

  Camera::ShPtr cam(new Camera());
  cam->set_direction(Vector3f(0.0f, -1.0f, -1.0f));
  cam->set_translate(Vector3f(0.0f, 10.0f, 20.0f));

  Mesh::ShPtr c = MeshManager::get().get_mesh("res/meshes/cube.obj");
  Mesh::ShPtr q = MeshManager::get().get_mesh("res/meshes/face-center-quad.obj");
  Material::ShPtr m = MaterialManager::get().get_material("res/materials/default.mtl");
  float r = 0.0f;
  
  GameObject::ShPtr ob(new GameObject());
  Renderable::ShPtr ren(new Renderable(ob->id()));
  Transform::ShPtr tran(new Transform(ob->id()));
  ren->set_mesh(c).set_material(m);
  //tran->print();
  //tran->translate(Vector3f(0.0f, 0.0f, -10.0f));
  tran->set_translate(Vector3f(5.0f, 10.0f, 10.0f));
  //tran->set_quat(Quaternion(Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 1.0f).normalize(), Vector3f(0.0f, -1.0f, 1.0f).normalize()));
  //tran->set_quat(Quaternion(Vector3f(0.0f, 0.0f, 1.0f).cross(Vector3f(1.0f, 1.0f, 1.0f)).normalize(), 45));
  //tran->print();
  //tran->rotate(Quaternion(Vector3f(1.0f, 0.0f, 0.0f).cross(Vector3f(0.0f, 0.0f, 1.0f)), 45));
  //tran->rotate(Quaternion(Vector3f(0.0f, 1.0f, 0.0f), 45.0f));
  
  GameObject::ShPtr ob2(new GameObject());
  Renderable::ShPtr ren2(new Renderable(ob2->id()));
  Transform::ShPtr tran2(new Transform(ob2->id()));
  ren2->set_mesh(c).set_material(m);
  tran2->translate(Vector3f(2.0f, 1.0f, -12.0f));
  std::cout << Quaternion(Vector3f(0.0f, 1.0f, 0.0f), 1.0f)*Quaternion(Vector3f(1.0f, 0.0f, 0.0f), 0.5f) << std::endl;
  
  World::ShPtr w(new World(std::string("res/worlds/bigbrother.obj")));
  
  InputManager im;
  PlayerInputHandler::ShPtr pih(new PlayerInputHandler(ob->id()));
  InputHandler::ShPtr ih(boost::dynamic_pointer_cast<InputHandler>(pih));
  im.pushHandler(ih);
  pih->listener(cam);

  while(1) {
    
    /*SDL_Event event_;
    SDL_WaitEvent( &event_ );
    if ( event_.type == SDL_QUIT ) {
      break;
    }*/
    im.handleInput();
    
    fps_->start();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY_EXT);
    
    r += 1.0f;
    cam->apply();
    //std::cout << Vector3f(0.0f, 0.0f, -10.0f)-Vector3f(0.0f, r/10, 0.0f) << std::endl;
    //cam->set_direction(Vector3f(0.0f, 0.0f, -10.0f)-Vector3f(0.0f, r/10, 0.0f));
    w->draw();
    glPushMatrix();
    //tran->rotate(Quaternion(Vector3f(0.0f, 1.0f, 0.0f), 1.0f)*Quaternion(Vector3f(1.0f, 0.0f, 0.0f), 0.5f));
    //tran->set_quat(Quaternion(Vector3f(0.0f, 1.0f, 0.0f), r));
    //tran->rotate(Quaternion(Vector3f(0.0f, 1.0f, 0.0f), 1.0f));
    //tran->rotateY(1.0f);
    //tran->rotateX(0.5f);
    tran->apply();
    ren->render();
    glPopMatrix();
    
    glPushMatrix();
    /*tran2->load();
    tran2->rotateY(1.0f);
    tran2->rotateX(0.5f);*/
    glTranslatef(2.0f, 1.0f, -12.0f);
    glRotatef(r, 0.0f, 1.0f, 0.0f);
    glRotatef(r/2, 1.0f, 0.0f, 0.0f);
    ren2->render();
    glPopMatrix();
    
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
