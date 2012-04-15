#ifndef COSMOS_RESOURCEMANAGER_H_
#define COSMOS_RESOURCEMANAGER_H_

#include <vector>

#include <boost/shared_ptr.hpp>

#include "util.hpp"

class ResourceManager {
 public:
  typedef boost::shared_ptr<ResourceManager> ShPtr;

  ResourceManager();

 private:
  
  void init();
  
  DISALLOW_COPY_AND_ASSIGN(ResourceManager);
};

#endif
