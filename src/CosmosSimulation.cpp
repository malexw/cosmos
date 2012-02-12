#include <iostream>

#include <boost/pointer_cast.hpp>

#include "PlayerInputHandler.hpp"
#include "InputManager.hpp"
#include "Camera.hpp"
#include "CosmosConfig.hpp"
#include "CosmosSimulation.hpp"
#include "GameObject.hpp"
#include "GameObjectManager.hpp"
#include "CollidableObject.hpp"
#include "Material.hpp"
#include "Message.hpp"
#include "ParticleEmitter.hpp"
#include "Quaternion.hpp"
#include "Renderable.hpp"
#include "Sound.hpp"
#include "Timer.hpp"
#include "Transform.hpp"
#include "Vector3f.hpp"
#include "World.hpp"

// Screen size
const int CosmosSimulation::SCREEN_WIDTH = 960;
const int CosmosSimulation::SCREEN_HEIGHT = 600;
const int CosmosSimulation::SCREEN_BPP = 32;

// Frame rate
const int CosmosSimulation::FRAMES_PER_SECOND = 70;
const float CosmosSimulation::US_PER_FRAME = 1000000/FRAMES_PER_SECOND;
const float CosmosSimulation::TEN_FPS = 100000;

CosmosSimulation::CosmosSimulation() {
  init_sdl();
  init_gl();
  init_resource_managers();
}

CosmosSimulation::~CosmosSimulation() {
  // Shut down the managers
  // (no managers need explicit shutdown yet)

  SDL_Quit();
}

void CosmosSimulation::run() {
  // ---------- CAMERA ------------------------------
  camera_.reset(new Camera(gob_manager_));

  {
    unsigned int camera_id = camera_->get_id();
    Message::ShPtr mts(new Message(Message::TRANSFORM_SET));
    // use default scale and quaternion value
    mts->add_arg("translation_x", 2.0f).add_arg("translation_y", 1.0f).add_arg("translation_z", -2.0f);
    gob_manager_->message_transform(camera_id, mts);
    Message::ShPtr mtl(new Message(Message::TRANSFORM_LOOKAT));
    mtl->add_arg("direction_x", 0.0f).add_arg("direction_y", 0.0f).add_arg("direction_z", -1.0f);
    gob_manager_->message_transform(camera_id, mtl);
    Message::ShPtr mcs(new Message(Message::COLLIDABLE_SCALE));
    mcs->add_arg("scale_x", 3.0f).add_arg("scale_y", 3.0f).add_arg("scale_z", 3.0f);
    gob_manager_->message_collidable(camera_id, mcs);
  }
  
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
  fps_.reset(new Timer());

  // -------------- SHADOWS --------------------------------------------
  GLuint shadowBuffer;
  glGenFramebuffersEXT(1, &shadowBuffer);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, shadowBuffer);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, texture_manager_->get_texture("shadow_map")->get_index(), 0);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  // -------------- HDR? -----------------------------------------------
  GLuint hdrFrameBuffer;
  glGenFramebuffersEXT(1, &hdrFrameBuffer);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, hdrFrameBuffer);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texture_manager_->get_texture("hdr target")->get_index(), 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  // ----------------- WORLD -------------------------------------------
  // TODO A world should be terrain + a skybox and the renderer should take this into account
  World::ShPtr world(new World(*this, std::string("res/worlds/bigbrother.obj")));

  // -------------- OBJECTS --------------------------------------------

  unsigned int cube_id;
  // The cube
  {
    cube_id = gob_manager_->spawn(GameObjectManager::COMPONENT_TRANSFORM | GameObjectManager::COMPONENT_RENDERABLE | GameObjectManager::COMPONENT_COLLIDABLE);
    Message::ShPtr mts(new Message(Message::TRANSFORM_SET));
    // use default scale and quaternion value
    mts->add_arg("translation_x", 2.0f).add_arg("translation_y", 1.0f).add_arg("translation_z", -12.0f);
    gob_manager_->message_transform(cube_id, mts);
    Message::ShPtr mrs(new Message(Message::RENDERABLE_SET));
    mrs->add_arg("material", "res/materials/default.mtl").add_arg("mesh", "res/meshes/cube.obj");
    gob_manager_->message_renderable(cube_id, mrs);
    Message::ShPtr mcs(new Message(Message::COLLIDABLE_SCALE));
    mcs->add_arg("scale_x", 1.732f).add_arg("scale_y", 1.732f).add_arg("scale_z", 10.0f);
    gob_manager_->message_collidable(cube_id, mcs);
  }
  // Pull these out so we have direct control while testing
  Renderable::ShPtr cube_renderable = gob_manager_->get_renderable(cube_id);
  Transform::ShPtr cube_transform = gob_manager_->get_transform(cube_id);
  
  // The particle emitter
  ParticleEmitter::ShPtr emitter;
  {
    unsigned int pe_id = gob_manager_->spawn(GameObjectManager::COMPONENT_TRANSFORM | GameObjectManager::COMPONENT_RENDERABLE);
    Message::ShPtr mts(new Message(Message::TRANSFORM_SET));
    // use default scale and quaternion value
    mts->add_arg("translation_x", 7.0f).add_arg("translation_y", 1.0f).add_arg("translation_z", -20.0f);
    gob_manager_->message_transform(pe_id, mts);
    Message::ShPtr mrs(new Message(Message::RENDERABLE_SET));
    mrs->add_arg("material", "res/materials/ion.mtl").add_arg("mesh", "res/meshes/face-center-quad.obj");
    gob_manager_->message_renderable(pe_id, mrs);
    
    emitter.reset(new ParticleEmitter(gob_manager_->get_transform(pe_id), gob_manager_->get_renderable(pe_id), 3.0f, 2.0f, 20, 30));
  }

  Sound::ShPtr part_sound = audio_manager_->get_sound("res/sounds/starshipmono.wav");
  part_sound->set_position(Vector3f(7, 1, -20));
  part_sound->set_gain(1.0f);
  part_sound->set_looping(true);
  part_sound->set_rolloff(0.5f);
  part_sound->play();

  // ----------------- INPUT -------------------------------------------
  // TODO Don't subclass InputHandlers, create generic ones and assign a callback
  InputManager im;
  PlayerInputHandler::ShPtr pih(new PlayerInputHandler(gob_manager_, camera_->get_id()));
  InputHandler::ShPtr ih(boost::static_pointer_cast<InputHandler>(pih));
  im.pushHandler(ih);

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
      // Turning this off for the Gob rearch
      /*if (config.is_hdr()) {
        skybox_renderable->set_mesh(mesh_manager_->get_mesh("res/meshes/hdrbox.obj")).set_material(material_manager_->get_material("res/materials/hdrbox.mtl"));
      } else {
        skybox_renderable->set_mesh(mesh_manager_->get_mesh("res/meshes/skybox.obj")).set_material(material_manager_->get_material("res/materials/skybox.mtl"));
      }*/
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
    float update_delta = fps_->frame_delta();
    gob_manager_->update_collidables(update_delta);
    //camera_collidable->update(updateDelta);    
    emitter->update(update_delta);
    Message::ShPtr tum(new Message(Message::TRANSFORM_UPDATE));
    tum->add_arg("yaw", 2).add_arg("pitch", 1);
    gob_manager_->message_transform(cube_id, tum);

    // Collisions
    if (config.is_collisions()) {
      // TODO Fix the collision checker
      //gob_manager_->check_collisions();
      //camera_collidable->check(cube_collidable);
    }

    // Reupdate

    // With final positions, we can update the sound
    audio_manager_->set_listener_transform(camera_->get_transform());

    //-------------- First pass for shadows
    if (config.is_shadows()) {
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, shadowBuffer); //Rendering offscreen
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
      // Generate the matrix for seeing the world from the light source
      glMultMatrixf(Camera::matrixFromPositionDirection(Vector3f(5, 15, 5), Vector3f(5, 0, -30)-Vector3f(5, 15, 5)).to_array());

      // Draw all the things that should cast shadows
      world->draw_geometry();

      glPushMatrix();
      cube_transform->apply();
      glMatrixMode(GL_TEXTURE);
      glPushMatrix();
      cube_transform->apply();
      shader_manager_->get_program("default")->run();
      cube_renderable->draw_geometry();
      glUseProgram(0);
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
    //glUseProgram(ShaderManager::get().get_program("hdr")->get_id());
    camera_->apply_rotation();
    if (config.is_hdr()) {
      shader_manager_->get_program("hdr")->run();
    }
    glPushMatrix();
    glRotatef(180, 0.0, 1.0f, 0.0);
    world->draw_skybox();
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
        glBindTexture(GL_TEXTURE_2D, texture_manager_->get_texture("hdr target")->get_index());
      }
      mesh_manager_->get_mesh("res/meshes/face-center-quad.obj")->draw();
      glDrawArrays(GL_TRIANGLES, 0, 6);
      glEnable(GL_LIGHTING);
      glPopMatrix();
    }

    glClear(GL_DEPTH_BUFFER_BIT);

    if (config.is_shadows()) {
      shader_manager_->get_program("shadow")->run();
      glActiveTexture(GL_TEXTURE3);
      glBindTexture(GL_TEXTURE_2D, texture_manager_->get_texture("shadow_map")->get_index());
      glActiveTexture(GL_TEXTURE0);
    }

    camera_->apply_inverse();
    //glPushMatrix();
    //camera_->apply();
    //camera_collidable->render_collision();
    //glPopMatrix();
    // Remember kids, always apply your lights *after* the camera transform
    glLightfv(GL_LIGHT0, GL_POSITION, lightVals);

    world->draw();

    glPushMatrix();
    //if (config.is_collidables()) {
      //cube_collidable->render_collision();
    //}
    cube_transform->apply();
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    cube_transform->apply();
    shader_manager_->get_program("bump")->run();
    cube_renderable->render();
    glUseProgram(0);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glUseProgram(0);
    if (config.is_particles()) {
      glDisable(GL_LIGHTING);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE);
      glDepthMask(GL_FALSE);
      emitter->render(camera_->get_transform());
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
}

void CosmosSimulation::init_resource_managers() {
  //resource_manager_.reset(new ResourceManager());
  texture_manager_.reset(new TextureManager());
  shader_manager_.reset(new ShaderManager());
  font_manager_.reset(new FontManager(texture_manager_));
  material_manager_.reset(new MaterialManager(shader_manager_, texture_manager_));
  mesh_manager_.reset(new MeshManager());
  audio_manager_.reset(new AudioManager());

  gob_manager_.reset(new GameObjectManager(material_manager_, mesh_manager_));

  std::cout << "Resources initialized" << std::endl;
}

void CosmosSimulation::init_sdl() {
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
     std::cout << "SDL_Init failed";
     return;
  }

  surface_ = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_OPENGL | SDL_HWSURFACE /*| SDL_FULLSCREEN*/);
  if (surface_ == 0) {
    std::cout << "SDL_SetVideoMode failed";
    return;
  }

  SDL_WM_SetCaption("Cosmos", 0);

  std::cout << "SDL initialized" << std::endl;
}

/*void CosmosSimulation::init_gl() {
  const char* vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
  const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
  const char* glsl_version = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
  std::cout << vendor << " GL version:" << version << " GLSL version:" << glsl_version << std::endl;
  glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  if( glGetError() != GL_NO_ERROR )
  {
    std::cout << "OpenGL Error" << std::endl;
  }
  std::cout << "OpenGL initialized" << std::endl;
}*/

void CosmosSimulation::init_gl() {
  const char* vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
  const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
  const char* glsl_version = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
  std::cout << vendor << " GL version:" << version << " GLSL version:" << glsl_version << std::endl;
  //const char* extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
  //std::cout << extensions << std::endl;
  
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

  std::cout << "OpenGL initialized" << std::endl;
}
