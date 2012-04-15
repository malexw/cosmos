#ifndef COSMOS_SOUND_H_
#define COSMOS_SOUND_H_

#include <string>

#include <boost/shared_ptr.hpp>

#include "AL/al.h"
#include "AL/alut.h"

#include "util.hpp"
#include "Vector3f.hpp"

class Sound {
public:
	typedef boost::shared_ptr<Sound> ShPtr;

	Sound(std::string name, unsigned int sound_index, unsigned int buffer_index);
	
	// Returns the name of the sound
	const std::string get_name() const;
	
	// Compare the name of this sound with another name. Returns true if they're equal, false otherwise.
	const bool is_name(const std::string& rhs) const;
	
	// Return the source index of the sound. This value is meaningless unless the Sound is loaded.
	const int get_index() const;
  
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
