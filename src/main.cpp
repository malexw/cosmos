#include <iostream>
#include <stdlib.h>

#include <boost/pointer_cast.hpp>

#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"

#include "PlayerInputHandler.hpp"
#include "InputManager.hpp"
#include "Camera.hpp"
#include "CosmosConfig.hpp"
#include "GameObject.hpp"
#include "GameObjectManager.hpp"
#include "CollidableObject.hpp"
#include "Material.hpp"
#include "ParticleEmitter.hpp"
#include "Quaternion.hpp"
#include "Renderable.hpp"
#include "Sound.hpp"
#include "ResourceManager/AudioManager.hpp"
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
const float TEN_FPS = 100000;

int main(int argc, char* argv[]) {

  srand(31337);

  if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
  {
     std:: cout << "SDL_Init failed";
     return 0;
  }

  SDL_Surface* sdlSurface_ = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_OPENGL | SDL_HWSURFACE /*| SDL_FULLSCREEN*/);
  if (sdlSurface_ == NULL) {
    std::cout << "SDL_SetVideoMode failed";
    return 0;
  }
  
  SDL_WM_SetCaption( "Cosmos", NULL );

  glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_CULL_FACE);
  
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
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
  //Camera::ShPtr cam(new Camera());
  //cam->set_direction(Vector3f(0.0f, 0.0f, -1.0f));
  //cam->set_translate(Vector3f(5.0f, 3.0f, -3.0f));
  //cam->set_translate(Vector3f(6.732f, 1.0f, -12.0f));

  //cam->set_direction(Vector3f(2, 0, -10)-Vector3f(32, 20, 0));
  //cam->set_translate(Vector3f(32, 20, 0));

  // -------------- SHADOWS --------------------------------------------
  GLuint shadowBuffer;
  glGenFramebuffersEXT(1, &shadowBuffer);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, shadowBuffer);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, TextureManager::get().get_texture("shadow_map")->get_index(), 0);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  // -------------- HDR? -----------------------------------------------
  GLuint hdrFrameBuffer;
  glGenFramebuffersEXT(1, &hdrFrameBuffer);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, hdrFrameBuffer);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, TextureManager::get().get_texture("hdr target")->get_index(), 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  // -------------- OBJECTS --------------------------------------------
  Mesh::ShPtr c = MeshManager::get().get_mesh("res/meshes/cube.obj");
  Mesh::ShPtr q = MeshManager::get().get_mesh("res/meshes/face-center-quad.obj");
  Material::ShPtr m = MaterialManager::get().get_material("res/materials/default.mtl");
  float r = 0.0f;
  
  GameObject::ShPtr skybox(new GameObject());
  GameObjectManager::get().add_object(skybox);
  Transform::ShPtr skybox_transform(new Transform(skybox->id()));
  skybox->set_transform(skybox_transform);
  Renderable::ShPtr skybox_renderable(new Renderable(skybox->id()));
  skybox->set_renderable(skybox_renderable);
  skybox_renderable->set_mesh(MeshManager::get().get_mesh("res/meshes/skybox.obj")).set_material(MaterialManager::get().get_material("res/materials/skybox.mtl"));
  //skybox_renderable->set_mesh(MeshManager::get().get_mesh("res/meshes/hdrbox.obj")).set_material(MaterialManager::get().get_material("res/materials/hdrbox.mtl"));
  
  GameObject::ShPtr camera(new GameObject());
  GameObjectManager::get().add_object(camera);
  Transform::ShPtr camera_transform(new Transform(camera->id()));
  camera->set_transform(camera_transform);
  CollidableObject::ShPtr camera_collidable(new CollidableObject(camera->id(), CollidableObject::TYPE_SPHERE));
  camera->set_collidable(camera_collidable);
  camera_collidable->set_scale(Vector3f(3.0f, 3.0f, 3.0f));
  camera_transform->set_direction(Vector3f(0.0f, 0.0f, -1.0f));
  camera_transform->set_translate(Vector3f(2.0f, 1.0f, -2.0f));
  
  GameObject::ShPtr spinning_cube(new GameObject());
  GameObjectManager::get().add_object(spinning_cube); 
  Transform::ShPtr cube_transform(new Transform(spinning_cube->id()));
  spinning_cube->set_transform(cube_transform);
  Renderable::ShPtr cube_renderable(new Renderable(spinning_cube->id()));
  spinning_cube->set_renderable(cube_renderable);
  CollidableObject::ShPtr cube_collidable(new CollidableObject(spinning_cube->id(), CollidableObject::TYPE_SPHERE));
  spinning_cube->set_collidable(cube_collidable);
  cube_renderable->set_mesh(c).set_material(m);
  cube_transform->set_translate(Vector3f(2.0f, 1.0f, -12.0f));
  //cube_collidable->set_translate(Vector3f(2.0f, 1.0f, -12.0f));
  //cube_collidable->set_scale(Vector3f(1.732, 1.732, 1.732));
  cube_collidable->set_scale(Vector3f(1.732, 1.732, 10));
  
  GameObject::ShPtr part(new GameObject());
  GameObjectManager::get().add_object(part);
  Renderable::ShPtr particle_renderable(new Renderable(part->id()));
  part->set_renderable(particle_renderable);
  particle_renderable->set_mesh(q).set_material(MaterialManager::get().get_material("res/materials/ion.mtl"));
  //std::cout << Quaternion(Vector3f(0.0f, 1.0f, 0.0f), 1.0f)*Quaternion(Vector3f(1.0f, 0.0f, 0.0f), 0.5f) << std::endl;
  Sound::ShPtr part_sound = AudioManager::get().get_sound("res/sounds/starshipmono.wav");
  part_sound->set_position(Vector3f(7, 1, -20));
  part_sound->set_gain(1.0f);
  part_sound->set_looping(true);
  part_sound->set_rolloff(0.5f);
  
  ParticleEmitter::ShPtr emitter(new ParticleEmitter(particle_renderable, Vector3f(7, 1, -20), Vector3f::UNIT_Y, Vector3f::UNIT_X, 3.0f, 2.0f, 20, 30));
  //Particle::ShPtr part(new Particle(ren2));
  
  // ----------------- WORLD -------------------------------------------
  World::ShPtr world(new World(std::string("res/worlds/bigbrother.obj")));
  
  // ----------------- INPUT -------------------------------------------
  InputManager im;
  PlayerInputHandler::ShPtr pih(new PlayerInputHandler(camera->id()));
  InputHandler::ShPtr ih(boost::dynamic_pointer_cast<InputHandler>(pih));
  im.pushHandler(ih);
  
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY_EXT);

  CosmosConfig& config = CosmosConfig::get();

  while(1) {
    fps_->frame_start();
    
    // Input
    im.handleInput();
    if (!config.is_valid()) {
      if (config.is_quit()) {
        break;
      }
      // HDR
      if (config.is_hdr()) {
        skybox_renderable->set_mesh(MeshManager::get().get_mesh("res/meshes/hdrbox.obj")).set_material(MaterialManager::get().get_material("res/materials/hdrbox.mtl"));
      } else {
        skybox_renderable->set_mesh(MeshManager::get().get_mesh("res/meshes/skybox.obj")).set_material(MaterialManager::get().get_material("res/materials/skybox.mtl"));
      }
      if (!config.is_textures()) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      }
      if (!config.is_sounds()) {
        part_sound->pause();
      } else {
        part_sound->play();
      }
      if (!config.is_shadows()) {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, 0);
      }

      config.set_valid();
    }
    
    // Update
    float updateDelta = fps_->frame_delta();
    camera_collidable->update(updateDelta);
    // Keep updating the emitter so when we turn it on, all the particles start up
    //if (config.is_particles()) {
      emitter->update(updateDelta);
    //}
    r += 1.0f;
    
    // Collisions
    if (config.is_collisions()) {
      camera_collidable->check(cube_collidable);
    }
    //camera_collidable->gjk(cube_collidable);
    
    // Reupdate

    // With final positions, we can update the sound
    AudioManager::get().set_listener_transform(camera_transform);

    //-------------- First pass for shadows
    if (config.is_shadows()) {
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, shadowBuffer);	//Rendering offscreen
      glUseProgram(0);
      glViewport(0,0,1024,1024);
      glClear(GL_DEPTH_BUFFER_BIT);
      glCullFace(GL_FRONT);
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluPerspective(45,1,10,40000);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glMultMatrixf(Camera::matrixFromPositionDirection(Vector3f(5, 15, 5), Vector3f(5, 0, -30)-Vector3f(5, 15, 5)).to_array());

      // Draw all the things that should cast shadows
      world->draw_geometry();

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
      cube_renderable->draw_geometry();
      //
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
      
      // Configure the shadow texture
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
    }

    //-------------- Second pass for skybox
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, hdrFrameBuffer);
    glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
    // MUST call glColorMask BEFORE glClear or things get explodey
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    glCullFace(GL_BACK);    
    glActiveTexture(GL_TEXTURE0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45,(static_cast<float>(SCREEN_WIDTH)/static_cast<float>(SCREEN_HEIGHT)),0.1,3);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMultMatrixf(Camera::matrixFromPositionDirection(Vector3f(0, 0, 0.8), Vector3f(0, 0, -1)).to_array());

    glDisable(GL_LIGHTING);
    glFrontFace(GL_CW);
    //glUseProgram(ShaderManager::get().get_shader_program("hdr")->get_id());
    camera_transform->apply_rotation();
    if (config.is_hdr()) {
      ShaderManager::get().get_shader_program("hdr")->run();
    }
    glPushMatrix();
    glRotatef(180, 0.0, 1.0f, 0.0);
    skybox_renderable->render();
    glPopMatrix();
    glUseProgram(0);
    glFrontFace(GL_CCW);
    glEnable(GL_LIGHTING);
      
    //------------------- Third pass to actually draw things
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45,(static_cast<float>(SCREEN_WIDTH)/static_cast<float>(SCREEN_HEIGHT)),1,4000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // "skybox"
    // Skip drawing the background when textures are off since it obscures everything
    if (config.is_textures() && config.is_skybox()) {
      glPushMatrix();
      glDisable(GL_LIGHTING);
      glTranslatef(0.0f, 0.0f, -2.0f);
      glScalef(2.7f, 1.7f, 1.0f);
      if (config.is_textures()) {
        glBindTexture(GL_TEXTURE_2D, TextureManager::get().get_texture("hdr target")->get_index());
      }
      MeshManager::get().get_mesh("res/meshes/face-center-quad.obj")->draw();
      glDrawArrays(GL_TRIANGLES, 0, 6);
      glEnable(GL_LIGHTING);
      glPopMatrix();
    }
    
    glClear(GL_DEPTH_BUFFER_BIT);
    
    if (config.is_shadows()) {
      ShaderManager::get().get_shader_program("shadow")->run();
      glActiveTexture(GL_TEXTURE3);
      glBindTexture(GL_TEXTURE_2D,TextureManager::get().get_texture("shadow_map")->get_index());
      glActiveTexture(GL_TEXTURE0);
    }
    
    camera_transform->apply_inverse();
    //glPushMatrix();
    //camera_transform->apply();
    //camera_collidable->render_collision();
    //glPopMatrix();
    // Remember kids, always apply your lights *after* the camera transform
    glLightfv(GL_LIGHT0, GL_POSITION, lightVals);

    world->draw();
    
    glPushMatrix();
    glTranslatef(2.0f,1.0f,-12.0f);
    if (config.is_collidables()) {
      cube_collidable->render_collision();
    }
    glRotatef(r, 0.0f, 1.0f, 0.0f);
    glRotatef(r/2, 1.0f, 0.0f, 0.0f);
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glTranslatef(2.0f,1.0f,-12.0f);
    glRotatef(r, 0.0f, 1.0f, 0.0f);
    glRotatef(r/2, 1.0f, 0.0f, 0.0f);
    //
    cube_renderable->render();
    //
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    glUseProgram(0);
    if (config.is_particles()) {
      glDisable(GL_LIGHTING);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE);
      glDepthMask(GL_FALSE);
      emitter->render(camera_transform);
      glDepthMask(GL_TRUE);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      glEnable(GL_LIGHTING);
    }
    //////////////////////////////////////////
    SDL_GL_SwapBuffers();
    
    //Cap the frame rate
    fps_->frame_stop();
    if( fps_->frame_length() < US_PER_FRAME )
    {
      //std::cout << "Waiting " << static_cast<int>(US_PER_FRAME - fps_->frame_length()) / 1000 << " ms" << std::endl;
      SDL_Delay(static_cast<int>(US_PER_FRAME - fps_->frame_length()) / 1000);
    }
    /*if( fps_->frame_length() < TEN_FPS )
    {
      SDL_Delay(static_cast<int>(TEN_FPS - fps_->frame_length()) / 1000);
    }*/
  }  
  
  SDL_Quit();
  return 0;
}
