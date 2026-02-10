#ifndef COSMOS_VERTEXSHADER_H_
#define COSMOS_VERTEXSHADER_H_

#include <string>

#include <memory>

#include "util.hpp"

class VertexShader {
public:
	using ShPtr = std::shared_ptr<VertexShader>;

	VertexShader(std::string name, int id): name_(name), shader_id_(id) {}
	
	// Returns the name of the VertexShader
	std::string get_name() const;
	
	// Compares the name of this VertexShader with another name. Returns true if they're equal, false otherwise.
	bool is_name(const std::string& rhs) const;

  int get_id() const { return shader_id_; }

private:
	std::string name_;
	int shader_id_;

	VertexShader(const VertexShader&) = delete;
	VertexShader& operator=(const VertexShader&) = delete;
};

#endif
