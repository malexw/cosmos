#ifndef COSMOS_SOUND_H_
#define COSMOS_SOUND_H_

#include <string>

#include <memory>

#ifdef __APPLE__
#include <OpenAL/al.h>
#else
#include <AL/al.h>
#endif

#include "util.hpp"
#include "Vector3f.hpp"

class Sound {
public:
	typedef std::shared_ptr<Sound> ShPtr;

	Sound(std::string name, unsigned int sound_index, unsigned int buffer_index);
	
	// Returns the name of the sound
	std::string get_name() const;

	// Compare the name of this sound with another name. Returns true if they're equal, false otherwise.
	bool is_name(const std::string& rhs) const;

	// Return the source index of the sound. This value is meaningless unless the Sound is loaded.
	int get_index() const;
  
  void play();
  void pause();
  void set_gain(float gain);
  void set_looping(bool loop);
  void set_position(const Vector3f& pos);
  void set_rolloff(float rolloff);

private:
	std::string name_;
	unsigned int sound_index_;
	unsigned int buffer_index_;

	DISALLOW_COPY_AND_ASSIGN(Sound);
};

#endif
