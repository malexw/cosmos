#ifndef COSMOS_ShaderProgram_H_
#define COSMOS_ShaderProgram_H_

#include <string>

#include <memory>

#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "util.hpp"

class ShaderProgram {
public:
	typedef std::shared_ptr<ShaderProgram> ShPtr;

	ShaderProgram(std::string name, int id): name_(name), shader_id_(id) {}
	
	// Returns the name of the ShaderProgram
	std::string get_name() const;
	
	// Compares the name of this ShaderProgram with another name. Returns true if they're equal, false otherwise.
	bool is_name(const std::string& rhs) const;

  int get_id() const { return shader_id_; }
  
  void run() const { glUseProgram(shader_id_); }
  void setf(std::string varname, float value);
  void seti(std::string varname, int value);

  // Set matrix uniforms — program must be currently active (call run() first)
  void setMat4(const char* name, const glm::mat4& mat) const;
  void setMat3(const char* name, const glm::mat3& mat) const;

private:
	std::string name_;
	int shader_id_;

	DISALLOW_COPY_AND_ASSIGN(ShaderProgram);
};

#endif
