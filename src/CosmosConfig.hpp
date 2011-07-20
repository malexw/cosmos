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

private:
  bool valid_;
  bool hdr_;

	DISALLOW_COPY_AND_ASSIGN(CosmosConfig);
};

#endif
