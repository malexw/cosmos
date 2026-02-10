#ifndef COSMOS_FRAGMENTSHADER_H_
#define COSMOS_FRAGMENTSHADER_H_

#include <string>

#include <memory>

#include "util.hpp"

class FragmentShader {
public:
	using ShPtr = std::shared_ptr<FragmentShader>;

	FragmentShader(std::string name, int id): name_(name), shader_id_(id) {}
	
	// Returns the name of the FragmentShader
	std::string get_name() const;
	
	// Compares the name of this FragmentShader with another name. Returns true if they're equal, false otherwise.
	bool is_name(const std::string& rhs) const;

  int get_id() const { return shader_id_; }

private:
	std::string name_;
	int shader_id_;

	FragmentShader(const FragmentShader&) = delete;
	FragmentShader& operator=(const FragmentShader&) = delete;
};

#endif
