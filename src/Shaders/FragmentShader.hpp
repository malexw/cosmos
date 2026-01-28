#ifndef COSMOS_FRAGMENTSHADER_H_
#define COSMOS_FRAGMENTSHADER_H_

#include <string>

#include <memory>

#include "util.hpp"

class FragmentShader {
public:
	typedef std::shared_ptr<FragmentShader> ShPtr;

	FragmentShader(std::string name, int id): name_(name), shader_id_(id) {}
	
	// Returns the name of the FragmentShader
	const std::string get_name() const;
	
	// Compares the name of this FragmentShader with another name. Returns true if they're equal, false otherwise.
	const bool is_name(const std::string& rhs) const;
  
  const int get_id() const { return shader_id_; }

private:
	std::string name_;
	int shader_id_;

	DISALLOW_COPY_AND_ASSIGN(FragmentShader);
};

#endif
