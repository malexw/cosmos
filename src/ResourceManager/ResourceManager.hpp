#ifndef COSMOS_RESOURCEMANAGER_H_
#define COSMOS_RESOURCEMANAGER_H_

#include <vector>

#include <memory>

#include "util.hpp"

class ResourceManager {
 public:
	typedef std::shared_ptr<ResourceManager> ShPtr;

	ResourceManager();
	
  static ResourceManager& get();

 private:
	
	void init();
	
	DISALLOW_COPY_AND_ASSIGN(ResourceManager);
};

#endif
