#include <iostream>

#include "FileBlob.hpp"
#include "ShaderManager.hpp"

ShaderManager::ShaderManager() {
  init();
}

/*
 * This initialization function is just to make it easier to manually edit the 'to-be-loaded' font list.
 * In the future, FontMan should read from some kind of resource file so we don't need to specify these by
 * hand
 */
void ShaderManager::init() {

  // Add the default shader
  ShaderProgram::ShPtr default_program(new ShaderProgram("default"));

  int default_vname = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(default_vname, 1, default_vert, 0);
  glCompileShader(default_vname);

  int default_fname = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(default_fname, 1, default_frag, 0);
  glCompileShader(default_fname);

  glAttachShader(default_program->get_id(), default_vname);
  glAttachShader(default_program->get_id(), default_fname);
  glLinkProgram(default_program->get_id());

  programs_.insert(ShaderTable::value_type(default_program->get_name(), default_program));

  shader_names_.push_back(std::string("res/shaders/bumpdec.vert"));
  shader_names_.push_back(std::string("res/shaders/bumpdec.frag"));
  create_program("bumpdec", shader_names_);
  shader_names_.clear();
  
  shader_names_.push_back(std::string("res/shaders/shadow.vert"));
  shader_names_.push_back(std::string("res/shaders/shadow.frag"));
  create_program("shadow", shader_names_);
  shader_names_.clear();
  
  // Configuration for the shadow program
  ShaderProgram::ShPtr shadow = get_program("shadow");
  shadow->run();
  GLint texSampler = glGetUniformLocation(shadow->get_id(), "tex");
  GLint shadowSampler = glGetUniformLocation(shadow->get_id(), "shadowMap");
  glUniform1i(texSampler, 0);
  glUniform1i(shadowSampler, 3);
  
  shader_names_.push_back(std::string("res/shaders/hdr.vert"));
  shader_names_.push_back(std::string("res/shaders/hdr.frag"));
  create_program("hdr", shader_names_);
  shader_names_.clear();
  
  shader_names_.push_back(std::string("res/shaders/bump.vert"));
  shader_names_.push_back(std::string("res/shaders/bump.frag"));
  create_program("bump", shader_names_);
  shader_names_.clear();
  //load_shaders();
}

/*
 *
 */
/*void ShaderManager::load_shaders() {

  // The rest of the shaders
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
  unsigned int p = glCreateProgram();
  unsigned int v = vshaders_[0]->get_id();
  unsigned int f = fshaders_[0]->get_id();

  glAttachShader(p,v);
  glAttachShader(p,f);

  glLinkProgram(p);
  //print_program_log(p);
  ShaderProgram::ShPtr program(new ShaderProgram("bumpdec", p));
  programs_.insert(ShaderTable::value_type(program->get_name(), program));
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
  //print_program_log(p);
  ShaderProgram::ShPtr shadow(new ShaderProgram("shadow", p));
  programs_.insert(ShaderTable::value_type(shadow->get_name(), shadow));
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
  //print_program_log(p);
  ShaderProgram::ShPtr hdr(new ShaderProgram("hdr", p));
  programs_.insert(ShaderTable::value_type(hdr->get_name(), hdr));
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
  //print_program_log(p);
  ShaderProgram::ShPtr bump(new ShaderProgram("bump", p));
  programs_.insert(ShaderTable::value_type(bump->get_name(), bump));
  glUseProgram(p);

  texSampler = glGetUniformLocation(p, "tex");
  bumpSampler = glGetUniformLocation(p, "bump");
  glUniform1i(texSampler, 0);
  glUniform1i(bumpSampler, 1);

  glUseProgram(0);
}*/

/*
 * Returns a shader program with the name "name" if it's been loaded. Otherwise returns the default program.
 */
ShaderProgram::ShPtr ShaderManager::get_program(const std::string& name) {
  ShaderProgram::ShPtr ret;
  ShaderTable::iterator iter = programs_.find(name);

  if (iter != programs_.end()) {
    ret = iter->second;
  } else {
    // Attempt to load the program if not found
    // if it can't be loaded, return the default
    std::cout << "Error: shader program <" << name << "> not found" << std::endl;
    ret = programs_.find("default")->second;
  }

  return ret;
}

const bool ShaderManager::create_program(const std::string& program_name, const std::vector<std::string>& paths) {

  ShaderProgram::ShPtr program(new ShaderProgram(program_name));
  unsigned int shader_count = paths.size();

  for (int i = 0; i < shader_count; ++i) {

    FileBlob::ShPtr file(new FileBlob(paths[i]));

    if (file->extension() == "vert") {

      VertexShader::ShPtr vshader(new VertexShader(file));
      if (vshader->compile()) {
        program->attach_shader(vshader);
      } else {
        std::cout << "Failed to compile vertex shader <" << paths[i] << ">" << std::endl;
      }

    } else if (file->extension() == "frag") {

      FragmentShader::ShPtr fshader(new FragmentShader(file));
      if (fshader->compile()) {
        program->attach_shader(fshader);
      } else {
        std::cout << "Failed to compile fragment shader <" << paths[i] << ">" << std::endl;
      }

    } else {
      std::cout << "ShaderManager error: shader type not recognized" << std::endl;
    }
  }

  if (program->link()) {
    programs_.insert(ShaderTable::value_type(program->get_name(), program));
    return true;
  } else {
    std::cout << "Failed to link shader program <" << program_name << ">" << std::endl;
    return false;
  }
}

const char* ShaderManager::default_frag[] = {
  "void main() { "
    "gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); "
  "}"
};
const char* ShaderManager::default_vert[] = {
  "layout (location = 0) in vec3 pos; "
  "void main() { "
    "gl_Position = gl_ModelViewProjectionMatrix * vec4(pos, 1.0); "
  "}"
};
