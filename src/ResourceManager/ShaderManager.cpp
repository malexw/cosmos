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
	shader_names_.push_back(std::string("res/shaders/bumpdec.vert"));
  shader_names_.push_back(std::string("res/shaders/bumpdec.frag"));
  shader_names_.push_back(std::string("res/shaders/shadow.vert"));
  shader_names_.push_back(std::string("res/shaders/shadow.frag"));
  shader_names_.push_back(std::string("res/shaders/hdr.vert"));
  shader_names_.push_back(std::string("res/shaders/hdr.frag"));
  shader_names_.push_back(std::string("res/shaders/bump.vert"));
  shader_names_.push_back(std::string("res/shaders/bump.frag"));
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
  // The bumpdec program
  int p = glCreateProgram();
  int v = vshaders_[0]->get_id();
  int f = fshaders_[0]->get_id();
  
  glAttachShader(p,v);
  glAttachShader(p,f);
  
  glLinkProgram(p);
  print_program_log(p);
  ShaderProgram::ShPtr program(new ShaderProgram("bumpdec", p));
  programs_.push_back(program);
  glUseProgram(p);
  
  GLint texSampler = glGetUniformLocation(p, "tex");
  GLint bumpSampler = glGetUniformLocation(p, "bump");
  GLint decalSampler = glGetUniformLocation(p, "decal");
  glUniform1i(texSampler, 0);
  glUniform1i(bumpSampler, 1);
  glUniform1i(decalSampler, 2);

  // The shadow program
  p = glCreateProgram();
  v = vshaders_[1]->get_id();
  f = fshaders_[1]->get_id();
  
  glAttachShader(p, v);
  glAttachShader(p, f);
  glLinkProgram(p);
  print_program_log(p);
  ShaderProgram::ShPtr shadow(new ShaderProgram("shadow", p));
  programs_.push_back(shadow);
  glUseProgram(p);
  texSampler = glGetUniformLocation(p, "tex");
  GLint shadowSampler = glGetUniformLocation(p, "shadowMap");
  glUniform1i(texSampler, 0);
  glUniform1i(shadowSampler, 3);
  
  // HDR program
  p = glCreateProgram();
  v = vshaders_[2]->get_id();
  f = fshaders_[2]->get_id();
  
  glAttachShader(p, v);
  glAttachShader(p, f);
  glLinkProgram(p);
  print_program_log(p);
  ShaderProgram::ShPtr hdr(new ShaderProgram("hdr", p));
  programs_.push_back(hdr);
  glUseProgram(p);
  GLint exp = glGetUniformLocation(p, "exposure");
  //GLint shadowSampler = glGetUniformLocation(p, "shadowMap");
  glUniform1f(exp, 1.0);
  //glUniform1i(shadowSampler, 3);
  
  // The bump program
  p = glCreateProgram();
  v = vshaders_[3]->get_id();
  f = fshaders_[3]->get_id();
  
  glAttachShader(p,v);
  glAttachShader(p,f);
  
  glLinkProgram(p);
  print_program_log(p);
  ShaderProgram::ShPtr bump(new ShaderProgram("bump", p));
  programs_.push_back(bump);
  glUseProgram(p);
  
  texSampler = glGetUniformLocation(p, "tex");
  bumpSampler = glGetUniformLocation(p, "bump");
  glUniform1i(texSampler, 0);
  glUniform1i(bumpSampler, 1);

  glUseProgram(0);
}

/*
 * Uses a dumb linear search to find a font with the same name. Optimizations welcome!
 */
const ShaderProgram::ShPtr ShaderManager::get_shader_program(const std::string& name) const {
	for (const ShaderProgram::ShPtr& shaderp : programs_) {
		if (shaderp->is_name(name)) {
			return shaderp;
		}
	}
	
  std::cout << "Error: shader program <" << name << "> not found" << std::endl;
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
