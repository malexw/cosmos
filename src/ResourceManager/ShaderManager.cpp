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
  ShaderProgram::ShPtr default_program(new ShaderProgram("error_program"));

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

  //shader_names_.push_back(std::string("res/shaders/bumpdec.vert"));
  //shader_names_.push_back(std::string("res/shaders/bumpdec.frag"));
  //create_program("bumpdec", shader_names_);
  //shader_names_.clear();

  // Configuration for the bump/decal program
  //ShaderProgram::ShPtr bumpdec = get_program("bumpdec");
  //bumpdec->seti("tex", 0);
  //bumpdec->seti("bump", 1);
  //bumpdec->seti("decal", 2);

  //shader_names_.push_back(std::string("res/shaders/shadow.vert"));
  //shader_names_.push_back(std::string("res/shaders/shadow.frag"));
  //create_program("shadow", shader_names_);
  //shader_names_.clear();

  // Configuration for the shadow program
  //ShaderProgram::ShPtr shadow = get_program("shadow");
  //shadow->seti("tex", 0);
  //shadow->seti("shadowMap", 3);

  /*shader_names_.push_back(std::string("res/shaders/hdr.vert"));
  shader_names_.push_back(std::string("res/shaders/hdr.frag"));
  create_program("hdr", shader_names_);
  shader_names_.clear();

  // HDR program config
  ShaderProgram::ShPtr hdr = get_program("hdr");
  hdr->setf("exposure", 1.0f);*/
  
  //shader_names_.push_back(std::string("res/shaders/point_sprite.vert"));
  //shader_names_.push_back(std::string("res/shaders/point_sprite.geom"));
  //shader_names_.push_back(std::string("res/shaders/point_sprite.frag"));
  //create_program("point-sprite", shader_names_);
  //shader_names_.clear();

  //ShaderProgram::ShPtr sprite = get_program("point-sprite");
  //sprite->seti("tex", 0);

  //shader_names_.push_back(std::string("res/shaders/bump.vert"));
  //shader_names_.push_back(std::string("res/shaders/bump.frag"));
  //create_program("bump", shader_names_);
  //shader_names_.clear();

  //ShaderProgram::ShPtr bump = get_program("bump");
  //bump->seti("tex", 0);
  //bump->seti("bump", 1);
}

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
    if (!file->is_valid()) {
      std::cout << "Failed to compile shader: source file <" << paths[i] << "> not found." << std::endl;
      continue;
    }

    if (file->extension() == "vert") {

      VertexShader::ShPtr vshader(new VertexShader(file));
      if (vshader->compile()) {
        program->attach_shader(vshader);
      } else {
        std::cout << "Failed to compile vertex shader <" << paths[i] << ">" << std::endl;
      }

    } else if (file->extension() == "geom") {
      
      GeometryShader::ShPtr gshader(new GeometryShader(file));
      if (gshader->compile()) {
        program->attach_shader(gshader);
      } else {
        std::cout << "Failed to compile geometry shader <" << paths[i] << ">" << std::endl;
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
