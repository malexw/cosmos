#include "Sound.hpp"

#include "ResourceManager/AudioManager.hpp"

Sound::Sound(std::string name, int voice_index, int clip_index)
  : name_(name),
    voice_index_(voice_index),
    clip_index_(clip_index) {
}

std::string Sound::get_name() const {
	return name_;
}

bool Sound::is_name(std::string_view rhs) const {
	return name_ == rhs;
}

int Sound::get_index() const {
	return voice_index_;
}

void Sound::play() {
  AudioManager::get().mixer().play(voice_index_);
}

void Sound::pause() {
  AudioManager::get().mixer().pause(voice_index_);
}

void Sound::set_gain(float gain) {
  AudioManager::get().mixer().set_gain(voice_index_, gain);
}

void Sound::set_looping(bool loop) {
  AudioManager::get().mixer().set_looping(voice_index_, loop);
}

void Sound::set_position(const Vector3f& pos) {
  AudioManager::get().mixer().set_position(voice_index_, pos);
}

void Sound::set_rolloff(float rolloff) {
  AudioManager::get().mixer().set_rolloff(voice_index_, rolloff);
}
