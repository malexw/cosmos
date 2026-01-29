#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#include "AudioManager.hpp"

// Simple WAV file loader - returns true on success
static bool loadWAV(const char* filename, ALenum* format, ALvoid** data, ALsizei* size, ALsizei* freq) {
  FILE* file = fopen(filename, "rb");
  if (!file) {
    std::cout << "AudioManager: Failed to open " << filename << std::endl;
    return false;
  }

  // Read and verify RIFF header
  char riff[4];
  if (fread(riff, 1, 4, file) != 4) { fclose(file); return false; }
  if (strncmp(riff, "RIFF", 4) != 0) {
    std::cout << "AudioManager: Not a valid WAV file (no RIFF header)" << std::endl;
    fclose(file);
    return false;
  }

  fseek(file, 4, SEEK_CUR); // Skip file size

  char wave[4];
  if (fread(wave, 1, 4, file) != 4) { fclose(file); return false; }
  if (strncmp(wave, "WAVE", 4) != 0) {
    std::cout << "AudioManager: Not a valid WAV file (no WAVE format)" << std::endl;
    fclose(file);
    return false;
  }

  // Find fmt chunk
  char chunkId[4];
  unsigned int chunkSize;
  while (fread(chunkId, 1, 4, file) == 4) {
    if (fread(&chunkSize, 4, 1, file) != 1) break;
    if (strncmp(chunkId, "fmt ", 4) == 0) {
      unsigned short audioFormat, numChannels, blockAlign, bitsPerSample;
      unsigned int sampleRate, byteRate;

      if (fread(&audioFormat, 2, 1, file) != 1) break;
      if (fread(&numChannels, 2, 1, file) != 1) break;
      if (fread(&sampleRate, 4, 1, file) != 1) break;
      if (fread(&byteRate, 4, 1, file) != 1) break;
      if (fread(&blockAlign, 2, 1, file) != 1) break;
      if (fread(&bitsPerSample, 2, 1, file) != 1) break;
      (void)byteRate; // unused but part of WAV format

      // Skip any extra fmt bytes
      if (chunkSize > 16) {
        fseek(file, chunkSize - 16, SEEK_CUR);
      }

      *freq = sampleRate;

      // Determine OpenAL format
      if (numChannels == 1) {
        *format = (bitsPerSample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
      } else {
        *format = (bitsPerSample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
      }
    } else if (strncmp(chunkId, "data", 4) == 0) {
      *size = chunkSize;
      *data = malloc(chunkSize);
      if (fread(*data, 1, chunkSize, file) != chunkSize) {
        free(*data);
        fclose(file);
        return false;
      }
      fclose(file);
      return true;
    } else {
      // Skip unknown chunk
      fseek(file, chunkSize, SEEK_CUR);
    }
  }

  fclose(file);
  std::cout << "AudioManager: WAV file missing data chunk" << std::endl;
  return false;
}

AudioManager::AudioManager()
  : loaded_(false) {
	init();
}

void AudioManager::init() {
  // Initialize OpenAL directly (no ALUT)
  ALCdevice* device = alcOpenDevice(NULL);
  if (!device) {
    std::cout << "AudioManager: Failed to open audio device" << std::endl;
    return;
  }
  ALCcontext* context = alcCreateContext(device, NULL);
  if (!context) {
    std::cout << "AudioManager: Failed to create audio context" << std::endl;
    alcCloseDevice(device);
    return;
  }
  alcMakeContextCurrent(context);

  sound_names_.push_back(std::string("res/sounds/starshipmono.wav"));
  load_sounds();
}

AudioManager& AudioManager::get() {
  static AudioManager instance;
  return instance;
}

void AudioManager::load_sounds() {
  if (loaded_) {
    std::cout << "SoundManager: Error - sounds already loaded" << std::endl;
    return;
  }

  int sound_count = sound_names_.size();
  std::vector<unsigned int> source_indicies(sound_count);
  std::vector<unsigned int> buffer_indicies(sound_count);
  alGenBuffers(sound_count, buffer_indicies.data());
  alGenSources(sound_count, source_indicies.data());

  for (int j = 0; j < sound_count; ++j) {
    ALenum format = AL_FORMAT_MONO16;
    ALsizei size = 0;
    ALsizei freq = 44100;
    ALvoid* data = nullptr;

    if (loadWAV(sound_names_[j].c_str(), &format, &data, &size, &freq)) {
      alBufferData(buffer_indicies[j], format, data, size, freq);
      free(data);
      Sound::ShPtr s(new Sound(sound_names_[j], source_indicies[j], buffer_indicies[j]));
      sounds_.push_back(s);
    } else {
      std::cout << "AudioManager: Failed to load " << sound_names_[j] << std::endl;
    }
  }

  loaded_ = true;
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
	for (const Sound::ShPtr& sound : sounds_) {
		if (sound->is_name(name)) {
			return sound;
		}
	}
	
  std::cout << "Error: sound <" << name << "> not found" << std::endl;
	return Sound::ShPtr();
}
