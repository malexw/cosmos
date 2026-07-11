#ifndef COSMOS_CosmosConfig_H_
#define COSMOS_CosmosConfig_H_

#include <string>

#include <memory>

#include "Texture.hpp"
#include "util.hpp"

class CosmosConfig {
public:
	using ShPtr = std::shared_ptr<CosmosConfig>;

	CosmosConfig();

  static CosmosConfig& get();
  
  bool is_valid() const { return valid_; }
  void invalidate() { valid_ = false; }
  void set_valid() { valid_ = true; }

  void set_hdr(bool value) { hdr_ = value; invalidate(); }
  bool is_hdr() const { return hdr_; }
  
  void set_skybox(bool value) { skybox_ = value; }
  bool is_skybox() const { return skybox_; }
  
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
  
  void set_collisions(bool value) { collisions_ = value; }
  bool is_collisions() const { return collisions_; }
  
  void set_shadows(bool value) { shadows_ = value; invalidate(); }
  bool is_shadows() const { return shadows_; }
  
  void set_sounds(bool value) { sounds_ = value; invalidate(); }
  bool is_sounds() const { return sounds_; }
  
  void set_quit(bool value) { quit_ = value; invalidate(); }
  bool is_quit() const { return quit_; }

  void set_shadow_debug(bool value) { shadow_debug_ = value; }
  bool is_shadow_debug() const { return shadow_debug_; }

  void set_pcf_mode(int value) { pcf_mode_ = value; }
  int pcf_mode() const { return pcf_mode_; }

  void set_gl_debug(bool value) { gl_debug_ = value; }
  bool is_gl_debug() const { return gl_debug_; }

  void set_show_axes(bool value) { show_axes_ = value; }
  bool is_show_axes() const { return show_axes_; }

  void set_exposure(float value) { exposure_ = value; }
  float exposure() const { return exposure_; }

  void set_ssao(bool value) { ssao_ = value; }
  bool is_ssao() const { return ssao_; }

  void set_ssao_radius(float value) { ssao_radius_ = value; }
  float ssao_radius() const { return ssao_radius_; }

  void set_ssao_bias(float value) { ssao_bias_ = value; }
  float ssao_bias() const { return ssao_bias_; }

  void set_ssao_power(float value) { ssao_power_ = value; }
  float ssao_power() const { return ssao_power_; }

  void set_shadow_distance(float d) { shadow_distance_ = d; }
  float shadow_distance() const { return shadow_distance_; }

  void set_shadow_cascades(int n) { shadow_cascades_ = n; }
  int shadow_cascades() const { return shadow_cascades_; }

  void set_shadow_bias(float b) { shadow_bias_ = b; }
  float shadow_bias() const { return shadow_bias_; }

private:
  bool valid_;
  bool hdr_;
  bool textures_;
  bool bump_;
  bool decals_;
  bool particles_;
  bool collidables_;
  bool collisions_;
  bool skybox_;
  bool shadows_;
  bool sounds_;
  bool quit_;
  bool shadow_debug_;
  int pcf_mode_;
  bool gl_debug_;
  bool show_axes_;
  float exposure_;
  bool ssao_;
  float ssao_radius_;
  float ssao_bias_;
  float ssao_power_;
  float shadow_distance_;
  int shadow_cascades_;
  float shadow_bias_;

	CosmosConfig(const CosmosConfig&) = delete;
	CosmosConfig& operator=(const CosmosConfig&) = delete;
};

#endif
