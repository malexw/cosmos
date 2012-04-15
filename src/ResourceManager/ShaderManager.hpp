#ifndef COSMOS_SHADERMANAGER_H_
#define COSMOS_SHADERMANAGER_H_

#include <vector>

#include <boost/shared_ptr.hpp>

#include "SDL/SDL_opengl.h"

#include "Shaders/VertexShader.hpp"
#include "Shaders/FragmentShader.hpp"
#include "Shaders/ShaderProgram.hpp"
#include "util.hpp"

class ShaderManager {
 public:
	typedef boost::shared_ptr<ShaderManager> ShPtr;

	ShaderManager();
	
  static ShaderManager& get();
	
	// Iterates through the list of loaded shader programs searching for a shader with the same name as "name".
	// Returns the first shader found with a matching name. Returns an empty pointer if no matching shader
	// is found.
	const ShaderProgram::ShPtr get_shader_program(const std::string& name) const;

 private:
	bool loaded_;
	std::vector<std::string> shader_names_;
	std::vector<ShaderProgram::ShPtr> programs_;
	std::vector<VertexShader::ShPtr> vshaders_;
  std::vector<FragmentShader::ShPtr> fshaders_;
  
	void init();
  
  // Iterates through the list of shaders that need to be loaded and loads them. First checks to see if
	// shaders have been loaded to prevent duplicate loadings
	void load_shaders();

  char* load_shader(char *fn);
	void print_shader_log(int id);
  void print_program_log(int id);
  
	DISALLOW_COPY_AND_ASSIGN(ShaderManager);
};

#endif
