#ifndef COSMOS_SOUND_H_
#define COSMOS_SOUND_H_

#include <string>
#include <string_view>

#include <memory>

#include "util.hpp"
#include "Vector3f.hpp"

class Sound {
public:
	using ShPtr = std::shared_ptr<Sound>;

	Sound(std::string name, int voice_index, int clip_index);

	// Returns the name of the sound
	std::string get_name() const;

	// Compare the name of this sound with another name. Returns true if they're equal, false otherwise.
	bool is_name(std::string_view rhs) const;

	// Return the mixer voice index of the sound. This value is meaningless unless the Sound is loaded.
	int get_index() const;

  void play();
  void pause();
  void set_gain(float gain);
  void set_looping(bool loop);
  void set_position(const Vector3f& pos);
  void set_rolloff(float rolloff);

private:
	std::string name_;
	int voice_index_;
	int clip_index_;

	Sound(const Sound&) = delete;
	Sound& operator=(const Sound&) = delete;
};

#endif
