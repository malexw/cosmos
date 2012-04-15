#include "CosmosConfig.hpp"

CosmosConfig::CosmosConfig()
 : valid_(false), hdr_(false), textures_(true), bump_(true), decals_(true), particles_(true),
   collidables_(false), collisions_(true), skybox_(true), shadows_(true), sounds_(true), quit_(false) { }

/*
 * Singleton pattern
 */
CosmosConfig& CosmosConfig::get() {
  static CosmosConfig instance;
  return instance;
}
