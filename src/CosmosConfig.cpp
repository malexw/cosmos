#include "CosmosConfig.hpp"

CosmosConfig::CosmosConfig()
 : valid_(false), hdr_(false), textures_(true), bump_(true), decals_(true), particles_(true),
   collidables_(false), collisions_(true), skybox_(true), shadows_(true), sounds_(true), quit_(false),
   shadow_debug_(false), pcf_mode_(0), gl_debug_(false), show_axes_(false), exposure_(1.0f),
   ssao_(true), ssao_radius_(0.5f), ssao_bias_(0.15f), ssao_power_(1.0f),
   shadow_distance_(60.0f), shadow_cascades_(3), shadow_bias_(0.05f),
   shadow_cube_debug_(false), shadow_cube_face_(0) { }

/*
 * Singleton pattern
 */
CosmosConfig& CosmosConfig::get() {
  static CosmosConfig instance;
  return instance;
}
