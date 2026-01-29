#ifndef COSMOS_TEXTURE_H_
#define COSMOS_TEXTURE_H_

#include <string>

#include <memory>

#include "util.hpp"

class Texture {
public:
	typedef std::shared_ptr<Texture> ShPtr;

	Texture(std::string name);
	
	// Returns the name of the texture
	std::string get_name() const;

	// Compare the name of this texture with another name. Returns true if they're equal, false otherwise.
	bool is_name(const std::string& rhs) const;

	// Return the index, AKA "OpenGL Name" of the texture. This value is meaningless unless the texture is
	// loaded.
	int get_index() const;

	int get_gltype() const;
	int get_bytecount() const;
	
	// Set the index, AKA "OpenGL Name" of the texture - used when loading the texture.
	void set_index(int index);

private:
	std::string name_;
	int texture_index_;
	int gltype_;
	int bytecount_;

	DISALLOW_COPY_AND_ASSIGN(Texture);
};

#endif
