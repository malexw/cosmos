#include "CosmosConfig.hpp"

CosmosConfig::CosmosConfig()
 : valid_(false), hdr_(false) { }

/*
 * Singleton pattern
 */
CosmosConfig& CosmosConfig::get() {
  static CosmosConfig instance;
  return instance;
}
