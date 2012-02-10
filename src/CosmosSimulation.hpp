#ifndef COSMOS_COSMOSSIMULATION_H_
#define COSMOS_COSMOSSIMULATION_H_

#include <boost/shared_ptr.hpp>

#include "SDL/SDL.h"
//#include "SDL/SDL_opengl.h"
#define GL_GLEXT_PROTOTYPES
#include "GL/gl.h"
#include "GL/glu.h"
#include "GL/glext.h"

#include "ResourceManager/AudioManager.hpp"
#include "ResourceManager/FontManager.hpp"
#include "ResourceManager/MaterialManager.hpp"
#include "ResourceManager/MeshManager.hpp"
//#include "ResourceManager/ResourceManager.hpp"
#include "ResourceManager/ShaderManager.hpp"
#include "ResourceManager/TextureManager.hpp"

#include "Camera.hpp"
#include "GameObjectManager.hpp"
#include "Timer.hpp"

class CosmosSimulation {
 public:
  typedef boost::shared_ptr<CosmosSimulation> ShPtr;

  CosmosSimulation();
  ~CosmosSimulation();

  void run();

  AudioManager::ShPtr audio_manager_;
  FontManager::ShPtr font_manager_;
  MaterialManager::ShPtr material_manager_;
  MeshManager::ShPtr mesh_manager_;
  //ResourceManager::ShPtr resource_manager_;
  ShaderManager::ShPtr shader_manager_;
  TextureManager::ShPtr texture_manager_;
  GameObjectManager::ShPtr gob_manager_;

  // Screen size
  static const int SCREEN_WIDTH;
  static const int SCREEN_HEIGHT;
  static const int SCREEN_BPP;

  // Frame rate
  static const int FRAMES_PER_SECOND;
  static const float US_PER_FRAME;
  static const float TEN_FPS;

 private:
  void init_resource_managers();
  void init_sdl();
  void init_gl();

  SDL_Surface* surface_;

  Timer::ShPtr fps_;

  // TODO Camera should likely be moved out to a "Renderer" class along with all the GL-specific code
  Camera::ShPtr camera_;

  DISALLOW_COPY_AND_ASSIGN(CosmosSimulation);
};

#endif
