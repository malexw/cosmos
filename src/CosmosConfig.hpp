#ifndef COSMOS_CosmosConfig_H_
#define COSMOS_CosmosConfig_H_

#include <string>

#include <boost/shared_ptr.hpp>

#include "Texture.hpp"
#include "util.hpp"

class CosmosConfig {
public:
	typedef boost::shared_ptr<CosmosConfig> ShPtr;

	CosmosConfig();

  static CosmosConfig& get();
  
  bool is_valid() const { return valid_; }
  void invalidate() { valid_ = false; }
  void set_valid() { valid_ = true; }

  void set_hdr(bool value) { hdr_ = value; invalidate(); }
  bool is_hdr() const { return hdr_; }
  
  void set_textures(bool value) { textures_ = value; invalidate(); }
  bool is_textures() const { return textures_; }
  
  void set_bump_mapping(bool value) { bump_ = value; }
  bool is_bump_mapping() const { return bump_; }
  
  void set_decals(bool value) { decals_ = value; }
  bool is_decals() const { return decals_; }
  
  void set_particles(bool value) { particles_ = value; }
  bool is_particles() const { return particles_; }
  
  void set_collidables(bool value) { collidables_ = value; }
  bool is_collidables() const { return collidables_; }

private:
  bool valid_;
  bool hdr_;
  bool textures_;
  bool bump_;
  bool decals_;
  bool particles_;
  bool collidables_;

	DISALLOW_COPY_AND_ASSIGN(CosmosConfig);
};

#endif
