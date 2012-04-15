#ifndef COSMOS_VERTEXSHADER_H_
#define COSMOS_VERTEXSHADER_H_

#include <string>

#include <boost/shared_ptr.hpp>

#include "util.hpp"

class VertexShader {
public:
	typedef boost::shared_ptr<VertexShader> ShPtr;

	VertexShader(std::string name, int id): name_(name), shader_id_(id) {}
	
	// Returns the name of the VertexShader
	const std::string get_name() const;
	
	// Compares the name of this VertexShader with another name. Returns true if they're equal, false otherwise.
	const bool is_name(const std::string& rhs) const;
  
  const int get_id() const { return shader_id_; }

private:
	std::string name_;
	int shader_id_;

	DISALLOW_COPY_AND_ASSIGN(VertexShader);
};

#endif
