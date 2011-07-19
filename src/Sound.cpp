#include "Sound.hpp"

Sound::Sound(std::string name, unsigned int sound_index, unsigned int buffer_index)
  : sound_index_(sound_index), buffer_index_(buffer_index),
    name_(name) {
  alSourcei(sound_index_, AL_BUFFER, buffer_index_);
  alSourcef(sound_index_, AL_PITCH, 1.0f);
  alSourcef(sound_index_, AL_GAIN, 0.0f);
}

const std::string Sound::get_name() const {
	return name_;
}

const bool Sound::is_name(const std::string& rhs) const {
	return name_.compare(rhs) == 0;
}

const int Sound::get_index() const {
	return sound_index_;
}

void Sound::play() {
  alSourcePlay(sound_index_);
}

void Sound::set_gain(float gain) {
  alSourcef(sound_index_, AL_GAIN, gain);
}

void Sound::set_looping(bool loop) {
  alSourcei(sound_index_, AL_LOOPING, loop);
}

void Sound::set_position(const Vector3f& pos) {
  alSource3f(sound_index_, AL_POSITION, pos.x(), pos.y(), pos.z());
}

void Sound::set_rolloff(float rolloff) {
  alSourcef(sound_index_, AL_ROLLOFF_FACTOR, rolloff);
}
