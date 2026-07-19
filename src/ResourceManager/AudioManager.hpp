#ifndef COSMOS_AUDIOMANAGER_H_
#define COSMOS_AUDIOMANAGER_H_

#include <vector>

#include <memory>

#include "AudioMixer.hpp"
#include "Sound.hpp"
#include "Transform.hpp"
#include "util.hpp"

class AudioManager {
 public:
	using ShPtr = std::shared_ptr<AudioManager>;

	AudioManager();

  static AudioManager& get();

	// Iterates through the list of loaded sounds searching for a sound with the same name as "name".
	// Returns the first sound found with a matching name. Returns an empty pointer if no matching sound
	// is found.
	Sound::ShPtr get_sound(const std::string name);

  // Loads a sound from the given path if it hasn't been loaded yet.
  // Returns the existing sound if already loaded.
  Sound::ShPtr load_sound(const std::string& path);

  void set_listener_transform(Transform::ShPtr transform);

  AudioMixer& mixer() { return mixer_; }

  // Tears down the mixer (audio device and Steam Audio objects). Idempotent;
  // must run before SDL_Quit() since the singleton outlives it.
  void shutdown();

 private:
	AudioMixer mixer_;
	std::vector<Sound::ShPtr> sounds_;

	AudioManager(const AudioManager&) = delete;
	AudioManager& operator=(const AudioManager&) = delete;
};

#endif
