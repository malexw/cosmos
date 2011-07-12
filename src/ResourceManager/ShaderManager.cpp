#include <iostream>

#include "FileBlob.hpp"
#include "ShaderManager.hpp"

ShaderManager::ShaderManager() 
  : loaded_(false) {
	init();
}

/*
 * This initialization function is just to make it easier to manually edit the 'to-be-loaded' font list.
 * In the future, FontMan should read from some kind of resource file so we don't need to specify these by
 * hand
 */
void ShaderManager::init() {
	shader_names_.push_back(std::string("res/shaders/simple.vert"));
  shader_names_.push_back(std::string("res/shaders/simple.frag"));
  load_shaders();
}

/*
 * Singleton pattern
 */
ShaderManager& ShaderManager::get() {
  static ShaderManager instance;
  return instance;
}

/*
 * 
 */
void ShaderManager::load_shaders() {
  if (loaded_) {
		std::cout << "ShaderManager error: shaders already loaded" << std::endl;
		return;
	}
    
  int shader_count = shader_names_.size();
    
  for (int j = 0; j < shader_count; ++j) {
    std::cout << "Processing " << shader_names_[j] << std::endl;
    FileBlob::ShPtr file(new FileBlob(shader_names_[j]));
    if (file->extension() == "vert") {
      int vname = glCreateShader(GL_VERTEX_SHADER);
      VertexShader::ShPtr vshader(new VertexShader(shader_names_[j], vname));
      vshaders_.push_back(vshader);
      const char* src = file->get_bytes();
      glShaderSource(vname, 1, &src, 0);
      glCompileShader(vname);
      print_shader_log(vname);
    } else if (file->extension() == "frag") {
      int fname = glCreateShader(GL_FRAGMENT_SHADER);
      FragmentShader::ShPtr fshader(new FragmentShader(shader_names_[j], fname));
      fshaders_.push_back(fshader);
      const char* src = file->get_bytes();
      glShaderSource(fname, 1, &src, 0);
      glCompileShader(fname);
      print_shader_log(fname);
    } else {
      std::cout << "ShaderManager error: shader type not recognized" << std::endl;
    }
	}
  // Sooo lazy
  int p = glCreateProgram();
  int v = vshaders_.back()->get_id();
  int f = fshaders_.back()->get_id();
  
  glAttachShader(p,v);
  glAttachShader(p,f);
  glLinkProgram(p);
  print_program_log(p);
  glUseProgram(p);
}

/*
 * Uses a dumb linear search to find a font with the same name. Optimizations welcome!
 */
const ShaderProgram::ShPtr ShaderManager::get_shader_program(const std::string& name) const {
	foreach (ShaderProgram::ShPtr shaderp, programs_) {
		if (shaderp->is_name(name)) {
			return shaderp;
		}
	}
	
	return ShaderProgram::ShPtr();
}

void ShaderManager::print_shader_log(int id) {

  int length = 0;
  int count = 0;

  glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

  if (length > 0) {
    char data[length];
    glGetShaderInfoLog(id, length, &count, data);
    std::cout << data;
  }
}

void ShaderManager::print_program_log(int id) {

  int length = 0;
  int count = 0;

  glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);

  if (length > 0) {
    char data[length];
    glGetProgramInfoLog(id, length, &count, data);
    std::cout << data;
  }
}
