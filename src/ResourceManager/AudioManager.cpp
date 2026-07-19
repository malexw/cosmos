#include <filesystem>
#include <iostream>
#include <memory>
#include <vector>

#include "AudioManager.hpp"

#include "FileBlob.hpp"

AudioManager::AudioManager() {
  mixer_.init();
}

AudioManager& AudioManager::get() {
  static AudioManager instance;
  return instance;
}

void AudioManager::shutdown() {
  mixer_.shutdown();
}

Sound::ShPtr AudioManager::load_sound(const std::string& path) {
  // Return existing sound if already loaded
  for (const Sound::ShPtr& sound : sounds_) {
    if (sound->is_name(path)) {
      return sound;
    }
  }

  if (!std::filesystem::exists(path)) {
    std::cout << "AudioManager: Failed to open " << path << std::endl;
    return Sound::ShPtr();
  }

  FileBlob blob(path);
  int clip_index = mixer_.load_clip(blob);
  if (clip_index < 0) {
    std::cout << "AudioManager: Failed to load " << path << std::endl;
    return Sound::ShPtr();
  }
  int voice_index = mixer_.create_voice(clip_index);

  auto s = std::make_shared<Sound>(path, voice_index, clip_index);
  sounds_.push_back(s);
  return s;
}

void AudioManager::set_listener_transform(Transform::ShPtr transform) {
  mixer_.set_listener(transform->get_position(), transform->get_rotation());
}

/*
 * Uses a dumb linear search to find a sound with the same name. Optimizations welcome!
 */
Sound::ShPtr AudioManager::get_sound(std::string name) {
	for (const Sound::ShPtr& sound : sounds_) {
		if (sound->is_name(name)) {
			return sound;
		}
	}

  std::cout << "Error: sound <" << name << "> not found" << std::endl;
	return Sound::ShPtr();
}
