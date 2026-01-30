#include <iostream>
#include <cmath>
#include <cstdlib>

#include <memory>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
     std:: cout << "SDL_Init failed: " << SDL_GetError();
     return 0;
  }

  // Request OpenGL 3.2 core profile
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  SDL_Window* window = SDL_CreateWindow(
    "Cosmos",
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    SCREEN_WIDTH, SCREEN_HEIGHT,
    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
  );
  if (window == NULL) {
    std::cout << "SDL_CreateWindow failed: " << SDL_GetError();
    SDL_Quit();
    return 0;
  }

  SDL_GLContext glContext = SDL_GL_CreateContext(window);
  if (glContext == NULL) {
    std::cout << "SDL_GL_CreateContext failed: " << SDL_GetError();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
  }

  glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);

  glEnable(GL_CULL_FACE);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  //glBlendFunc(GL_SRC_ALPHA,GL_ONE);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(2.0f);

  if( glGetError() != GL_NO_ERROR )
  {
      //error
  }

  ResourceManager::get();
  std::cout << "Resources loaded" << std::endl;

  // ----------- LIGHTING ----------------------------------------------
  // Light properties (used as shader uniforms)
  glm::vec3 lightDirWorld(0.0f, -15.0f, -30.0f);
  glm::vec4 lightAmbient(0.0f, 0.0f, 0.0f, 1.0f);
  glm::vec4 lightDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
  glm::vec4 lightSpecular(1.0f, 1.0f, 1.0f, 1.0f);
  glm::vec4 ambientGlobal(0.0f, 0.0f, 0.0f, 1.0f);

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
  glGenFramebuffers(1, &shadowBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, TextureManager::get().get_texture("shadow_map")->get_index(), 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // -------------- HDR? -----------------------------------------------
  GLuint hdrFrameBuffer;
  glGenFramebuffers(1, &hdrFrameBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, hdrFrameBuffer);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureManager::get().get_texture("hdr target")->get_index(), 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
  InputHandler::ShPtr ih(std::dynamic_pointer_cast<InputHandler>(pih));
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

    // Compute cube model matrix (used in shadow and main passes)
    glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, -12.0f));
    cubeModel = glm::rotate(cubeModel, glm::radians(r), glm::vec3(0.0f, 1.0f, 0.0f));
    cubeModel = glm::rotate(cubeModel, glm::radians(r/2.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 shadowTexMatrix(1.0f);

    //-------------- First pass for shadows
    if (config.is_shadows()) {
      glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
      glViewport(0,0,1024,1024);
      glClear(GL_DEPTH_BUFFER_BIT);
      glCullFace(GL_FRONT);
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

      glm::mat4 shadowProj = glm::perspective(glm::radians(45.0f), 1.0f, 10.0f, 40000.0f);
      glm::mat4 shadowView = Camera::matrixFromPositionDirection(Vector3f(5, 15, 5), Vector3f(5, 0, -30)-Vector3f(5, 15, 5));

      auto flatProg = ShaderManager::get().get_shader_program("flat");
      flatProg->run();

      // Draw world geometry (identity model)
      flatProg->setMat4("mvp", shadowProj * shadowView);
      world->draw_geometry();

      // Draw cube geometry
      flatProg->setMat4("mvp", shadowProj * shadowView * cubeModel);
      cube_renderable->draw_geometry();

      // Compute shadow texture matrix for main pass
      glm::mat4 biasMatrix(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f
      );
      shadowTexMatrix = biasMatrix * shadowProj * shadowView;
    }

    //-------------- Second pass for skybox
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFrameBuffer);
    glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
    // MUST call glColorMask BEFORE glClear or things get explodey
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);
    glActiveTexture(GL_TEXTURE0);

    {
      glm::mat4 skyProj = glm::perspective(glm::radians(45.0f), (static_cast<float>(SCREEN_WIDTH)/static_cast<float>(SCREEN_HEIGHT)), 0.1f, 3.0f);
      glm::mat4 skyView = Camera::matrixFromPositionDirection(Vector3f(0, 0, 0.8), Vector3f(0, 0, -1));
      glm::mat4 cameraRotInv = camera_transform->get_rotation_matrix();
      glm::mat4 rotY180 = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
      glm::mat4 skyboxMVP = skyProj * skyView * cameraRotInv * rotY180;

      glFrontFace(GL_CW);
      if (config.is_hdr()) {
        auto hdrProg = ShaderManager::get().get_shader_program("hdr");
        hdrProg->run();
        hdrProg->setMat4("mvp", skyboxMVP);
      } else {
        auto unlitProg = ShaderManager::get().get_shader_program("unlit");
        unlitProg->run();
        unlitProg->setMat4("mvp", skyboxMVP);
      }
      skybox_renderable->render();
      glFrontFace(GL_CCW);
    }
      
    //------------------- Third pass to actually draw things
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 mainProj = glm::perspective(glm::radians(45.0f), (static_cast<float>(SCREEN_WIDTH)/static_cast<float>(SCREEN_HEIGHT)), 1.0f, 4000.0f);
    glm::mat4 mainView = camera_transform->get_inverse_matrix();
    glm::mat4 mainPV = mainProj * mainView;

    // "skybox" HUD quad — drawn with identity view (before camera transform)
    if (config.is_textures() && config.is_skybox()) {
      glm::mat4 hudModel = glm::scale(
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f)),
        glm::vec3(2.7f, 1.7f, 1.0f));
      auto unlitProg = ShaderManager::get().get_shader_program("unlit");
      unlitProg->run();
      unlitProg->setMat4("mvp", mainProj * hudModel);
      glBindTexture(GL_TEXTURE_2D, TextureManager::get().get_texture("hdr target")->get_index());
      MeshManager::get().get_mesh("res/meshes/face-center-quad.obj")->draw();
      glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glClear(GL_DEPTH_BUFFER_BIT);

    // Compute light direction in eye space (directional light: just rotate by view)
    glm::vec3 lightPosEye = glm::mat3(mainView) * lightDirWorld;
    glm::mat3 worldNormalMatrix = glm::transpose(glm::inverse(glm::mat3(mainView)));

    // Activate shader for world draw
    if (config.is_shadows()) {
      auto shadowProg = ShaderManager::get().get_shader_program("shadow");
      shadowProg->run();
      shadowProg->setMat4("mvp", mainPV);
      shadowProg->setMat3("normalMatrix", worldNormalMatrix);
      shadowProg->setMat4("shadowMatrix", shadowTexMatrix);
      GLint loc = glGetUniformLocation(shadowProg->get_id(), "lightPosEye");
      if (loc >= 0) glUniform3fv(loc, 1, glm::value_ptr(lightPosEye));
      glActiveTexture(GL_TEXTURE3);
      glBindTexture(GL_TEXTURE_2D, TextureManager::get().get_texture("shadow_map")->get_index());
      glActiveTexture(GL_TEXTURE0);
    } else if (config.is_textures()) {
      // Simple shader with per-vertex lighting (no shadows)
      auto simpleProg = ShaderManager::get().get_shader_program("simple");
      simpleProg->run();
      simpleProg->setMat4("mvp", mainPV);
      simpleProg->setMat3("normalMatrix", worldNormalMatrix);
      GLint loc;
      loc = glGetUniformLocation(simpleProg->get_id(), "lightPosEye");
      if (loc >= 0) glUniform3fv(loc, 1, glm::value_ptr(lightPosEye));
      loc = glGetUniformLocation(simpleProg->get_id(), "lightDiffuse");
      if (loc >= 0) glUniform4fv(loc, 1, glm::value_ptr(lightDiffuse));
      loc = glGetUniformLocation(simpleProg->get_id(), "lightAmbient");
      if (loc >= 0) glUniform4fv(loc, 1, glm::value_ptr(lightAmbient));
      loc = glGetUniformLocation(simpleProg->get_id(), "lightSpecular");
      if (loc >= 0) glUniform4fv(loc, 1, glm::value_ptr(lightSpecular));
      loc = glGetUniformLocation(simpleProg->get_id(), "ambientGlobal");
      if (loc >= 0) glUniform4fv(loc, 1, glm::value_ptr(ambientGlobal));
      loc = glGetUniformLocation(simpleProg->get_id(), "matSpecular");
      if (loc >= 0) glUniform4f(loc, 0.0f, 0.0f, 0.0f, 1.0f);
      loc = glGetUniformLocation(simpleProg->get_id(), "matShininess");
      if (loc >= 0) glUniform1f(loc, 0.0f);
    } else {
      // Wireframe mode — flat shader
      auto flatProg = ShaderManager::get().get_shader_program("flat");
      flatProg->run();
      flatProg->setMat4("mvp", mainPV);
      GLint loc = glGetUniformLocation(flatProg->get_id(), "flatColor");
      if (loc >= 0) glUniform4f(loc, 1.0f, 1.0f, 1.0f, 1.0f);
    }

    world->draw();

    // Collidable wireframe at cube's translated position
    glm::mat4 cubeTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, -12.0f));
    if (config.is_collidables()) {
      cube_collidable->render_collision(mainProj * mainView * cubeTranslate);
    }

    // Full cube model — set shader uniforms
    {
      glm::mat4 cubeMV = mainView * cubeModel;
      glm::mat4 cubeMVP = mainProj * cubeMV;
      glm::mat3 cubeNormalMatrix = glm::transpose(glm::inverse(glm::mat3(cubeMV)));

      if (config.is_textures()) {
        // Pre-set matrix and lighting uniforms on bump and bumpdec shaders
        auto bumpProg = ShaderManager::get().get_shader_program("bump");
        bumpProg->run();
        bumpProg->setMat4("mvp", cubeMVP);
        bumpProg->setMat3("normalMatrix", cubeNormalMatrix);
        glUniform3fv(glGetUniformLocation(bumpProg->get_id(), "lightPosEye"), 1, glm::value_ptr(lightPosEye));

        auto bumpdecProg = ShaderManager::get().get_shader_program("bumpdec");
        bumpdecProg->run();
        bumpdecProg->setMat4("mvp", cubeMVP);
        bumpdecProg->setMat3("normalMatrix", cubeNormalMatrix);
        glUniform3fv(glGetUniformLocation(bumpdecProg->get_id(), "lightPosEye"), 1, glm::value_ptr(lightPosEye));
      } else {
        // Wireframe mode — flat shader for cube
        auto flatProg = ShaderManager::get().get_shader_program("flat");
        flatProg->run();
        flatProg->setMat4("mvp", cubeMVP);
        GLint loc = glGetUniformLocation(flatProg->get_id(), "flatColor");
        if (loc >= 0) glUniform4f(loc, 1.0f, 1.0f, 1.0f, 1.0f);
      }
    }

    cube_renderable->render();

    // Particles
    if (config.is_particles()) {
      glBlendFunc(GL_SRC_ALPHA,GL_ONE);
      glDepthMask(GL_FALSE);
      emitter->render(camera_transform, mainPV);
      glDepthMask(GL_TRUE);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    }
    //////////////////////////////////////////
    SDL_GL_SwapWindow(window);
    
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
  
  SDL_GL_DeleteContext(glContext);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
