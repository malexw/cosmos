#ifndef COSMOS_SHADERMANAGER_H_
#define COSMOS_SHADERMANAGER_H_

#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "Renderer.hpp"

#include "Shaders/VertexShader.hpp"
#include "Shaders/FragmentShader.hpp"
#include "Shaders/ShaderProgram.hpp"
#include "util.hpp"

class ShaderManager {
 public:
  typedef boost::shared_ptr<ShaderManager> ShPtr;
  typedef std::map<std::string, ShaderProgram::ShPtr> ShaderTable;

  ShaderManager();

  /*
   * Iterates through the list of loaded shader programs searching for a shader with the same name as "name".
   * Returns the first shader found with a matching name. Returns an empty pointer if no matching shader
   * is found.
   */
  ShaderProgram::ShPtr get_program(const std::string& name);

  /*
   * Loads a group of vertex and fragment shaders from the file system and compiles them into a shader program.
   * Returns true if the program was loaded, false if compilation failed (for any reason)
   * Returns true if there is already a program with name "name", but does not replace the older program.
   */
  const bool create_program(const std::string& program_name, const std::vector<std::string>& paths);

 private:
  std::vector<std::string> shader_names_;
  std::vector<VertexShader::ShPtr> vshaders_;
  std::vector<FragmentShader::ShPtr> fshaders_;
  ShaderTable programs_;

  static const char* default_frag[];
  static const char* default_vert[];

  void init();

  DISALLOW_COPY_AND_ASSIGN(ShaderManager);
};

#endif
