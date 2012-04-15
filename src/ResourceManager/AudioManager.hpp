#ifndef COSMOS_AUDIOMANAGER_H_
#define COSMOS_AUDIOMANAGER_H_

#include <vector>

#include <boost/shared_ptr.hpp>

#include "AL/al.h"
#include "AL/alut.h"

#include "Sound.hpp"
#include "Transform.hpp"
#include "util.hpp"

class AudioManager {
 public:
  typedef boost::shared_ptr<AudioManager> ShPtr;

  AudioManager();

  // Iterates through the list of loaded textures searching for a texture with the same name as "name".
  // Returns the first texture found with a matching name. Returns an empty pointer if no matching texture
  // is found.
  Sound::ShPtr get_sound(const std::string name);

  void set_listener_transform(Transform::ShPtr transform);

 private:
  bool loaded_;
  std::vector<std::string> sound_names_;
  std::vector<Sound::ShPtr> sounds_;

  void init();

  // Iterates through the list of textures that need to be loaded and loads them. First checks to see if
  // textures have been loaded to prevent duplicate loadings
  void load_sounds();

  DISALLOW_COPY_AND_ASSIGN(AudioManager);
};

#endif
