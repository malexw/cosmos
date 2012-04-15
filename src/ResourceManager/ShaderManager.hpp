#ifndef COSMOS_SHADERMANAGER_H_
#define COSMOS_SHADERMANAGER_H_

#include <map>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "Renderer.hpp"
#include "Shaders/ShaderProgram.hpp"
#include "Shaders/Shader.hpp"
#include "util.hpp"

class ShaderManager {
 public:
  typedef boost::shared_ptr<ShaderManager> ShPtr;
  typedef std::map<std::string, ShaderProgram::ShPtr> ShaderTable;

  ShaderManager();

  /*
   * Iterates through the list of loaded shader programs searching for a shader with the same name as "name".
   * Returns the first shader found with a matching name. Returns the default program if no matching program is found.
   */
  ShaderProgram::ShPtr get_program(const std::string& name);

  /*
   * Loads a group of vertex and fragment shaders from the file system and compiles them into a shader program. If a
   * program with name program_name already exists, the old program is replaced by the new program.
   * Returns true if the program was loaded, false if compilation failed (for any reason)
   */
  const bool create_program(const std::string& program_name, const std::vector<std::string>& paths);

 private:
  std::vector<std::string> shader_names_;
  std::vector<VertexShader::ShPtr> vshaders_;
  std::vector<FragmentShader::ShPtr> fshaders_;
  ShaderTable programs_;

  static const std::string default_frag;
  static const std::string default_vert;

  void init();

  /*
   * Lazy man's preprocessor for GLSL
   */
  void insert_includes(std::string& source);

  /*
   * Searches through source for the first instance of a string equal to marker. Replaces the marker text with the
   * replacement string.
   */
  void substitute_text(std::string& source, const std::string& marker, const std::string& replacement);

  DISALLOW_COPY_AND_ASSIGN(ShaderManager);
};

#endif
