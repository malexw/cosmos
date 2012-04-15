#include <iostream>

#include "AudioManager.hpp"

AudioManager::AudioManager()
  : loaded_(false) {
  init();
}

/*
 * This initialization function is just to make it easier to manually edit the 'to-be-loaded' font list.
 * In the future, FontMan should read from some kind of resource file so we don't need to specify these by
 * hand
 */
void AudioManager::init() {
  alutInit(0, NULL);
  sound_names_.push_back(std::string("res/sounds/starshipmono.wav"));
  load_sounds();
}

/*
 *
 */
void AudioManager::load_sounds() {
  if (loaded_) {
    std::cout << "SoundManager: Error - sounds already loaded" << std::endl;
    return;
  }

  // For now, assuming one sound (buffer) per source
  int sound_count = sound_names_.size();
  unsigned int source_indicies[sound_count];
  unsigned int buffer_indicies[sound_count];
  alGenBuffers(sound_count, buffer_indicies);
  alGenSources(sound_count, source_indicies);

  ALenum     format;
  ALsizei    size;
  ALsizei    freq;
  ALboolean  loop;
  ALvoid*    data;

  for (int j = 0; j < sound_count; ++j) {
    // TODO Evil, evil casting here
    alutLoadWAVFile(reinterpret_cast<ALbyte*>(const_cast<char*>(sound_names_[j].c_str())), &format, &data, &size, &freq, &loop);
    alBufferData(buffer_indicies[j], format, data, size, freq);
    alutUnloadWAV(format, data, size, freq);
    Sound::ShPtr s(new Sound(sound_names_[j], source_indicies[j], buffer_indicies[j]));
    sounds_.push_back(s);
  }
}

void AudioManager::set_listener_transform(Transform::ShPtr transform) {
  Vector3f up = transform->get_rotation() * Vector3f::UNIT_Y;
  Vector3f normal = transform->get_rotation() * Vector3f::NEGATIVE_Z;
  Vector3f pos = transform->get_position();
  alListener3f(AL_POSITION, pos.x(), pos.y(), pos.z());
  ALfloat orientation[6] = {normal.x(), normal.y(), normal.z(), up.x(), up.y(), up.z()};
  alListenerfv(AL_ORIENTATION, orientation);
}

/*
 * Uses a dumb linear search to find a font with the same name. Optimizations welcome!
 */
Sound::ShPtr AudioManager::get_sound(std::string name) {
  foreach (Sound::ShPtr sound, sounds_) {
    if (sound->is_name(name)) {
      return sound;
    }
  }

  std::cout << "Error: sound <" << name << "> not found" << std::endl;
  return Sound::ShPtr();
}
