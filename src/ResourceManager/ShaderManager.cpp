#include <iostream>

#include "FileBlob.hpp"
#include "ShaderManager.hpp"

ShaderManager::ShaderManager() {
  init();
}

void ShaderManager::init() {

  // Add the default shader
  ShaderProgram::ShPtr default_program(new ShaderProgram("__err_program"));

  std::string dv(default_vert);
  insert_includes(dv);
  const char* dv_ptr = dv.c_str();
  const char* vert_sources[1] = { dv_ptr };
  VertexShader::ShPtr vshader(new VertexShader(vert_sources));
  vshader->compile();
  default_program->attach_shader(vshader);

  std::string df(default_frag);
  insert_includes(df);
  const char* df_ptr = df.c_str();
  const char* frag_sources[1] = { df_ptr };
  FragmentShader::ShPtr fshader(new FragmentShader(frag_sources));
  fshader->compile();
  default_program->attach_shader(fshader);

  if (default_program->link()) {
    programs_.insert(ShaderTable::value_type(default_program->get_name(), default_program));
  } else {
    std::cout << "Error linking default shader program" << std::endl;
  }
  // End of code for default shader
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

    std::string source_string(file->get_bytes());
    insert_includes(source_string);
    const char* source_array[1] = { source_string.c_str() };

    if (file->extension() == "vert") {

      VertexShader::ShPtr vshader(new VertexShader(source_array));
      if (vshader->compile()) {
        program->attach_shader(vshader);
      } else {
        std::cout << "Failed to compile vertex shader <" << paths[i] << ">" << std::endl;
      }

    } else if (file->extension() == "geom") {

      GeometryShader::ShPtr gshader(new GeometryShader(source_array));
      if (gshader->compile()) {
        program->attach_shader(gshader);
      } else {
        std::cout << "Failed to compile geometry shader <" << paths[i] << ">" << std::endl;
      }

    } else if (file->extension() == "frag") {

      FragmentShader::ShPtr fshader(new FragmentShader(source_array));
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

void ShaderManager::insert_includes(std::string& source) {
  substitute_text(source,
    std::string("#include cosmos.matrices\n"),
    std::string(
      "layout (std140) uniform matrices {\n"
      "  mat4 c_ViewMatrix;\n"
      "  mat4 c_ProjectionMatrix;\n"
      "  mat4 c_ShadowMatrix;\n"
      "  mat4 c_ModelMatrix;\n"
      "  mat4 c_ModelViewMatrix;\n"
      "  mat4 c_ModelViewProjectionMatrix;\n"
      "};\n"
    )
  );
  substitute_text(source,
    std::string("#include cosmos.attrib_array\n"),
    std::string(
      "layout (location = 0) in vec3 pos;\n"
      "layout (location = 1) in vec2 tex;\n"
      "layout (location = 2) in vec3 norm;\n"
    )
  );

}

void ShaderManager::substitute_text(std::string& source, const std::string& marker, const std::string& replacement) {
  std::size_t index = source.find(marker);

  if (index != std::string::npos) {
    source.erase(index, marker.size());
    source.insert(index, replacement);
  }
}


const std::string ShaderManager::default_frag(
  "void main() { "
    "gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); "
  "}"
);

const std::string ShaderManager::default_vert(
  "#version 150\n"
  "#include cosmos.attrib_array\n"
  "#include cosmos.matrices\n"
  "void main() { "
    "gl_Position = gl_ModelViewProjectionMatrix * vec4(pos, 1.0); "
  "}"
);
