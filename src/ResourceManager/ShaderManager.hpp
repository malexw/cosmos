#ifndef COSMOS_SHADERMANAGER_H_
#define COSMOS_SHADERMANAGER_H_

#include <vector>

#include <memory>

#include <glm/glm.hpp>

#include "Shaders/VertexShader.hpp"
#include "Shaders/FragmentShader.hpp"
#include "Shaders/ShaderProgram.hpp"
#include "util.hpp"

class ShaderManager {
 public:
	using ShPtr = std::shared_ptr<ShaderManager>;

	ShaderManager();
	
  static ShaderManager& get();
	
	// Iterates through the list of loaded shader programs searching for a shader with the same name as "name".
	// Returns the first shader found with a matching name. Returns an empty pointer if no matching shader
	// is found.
	const ShaderProgram::ShPtr get_shader_program(const std::string& name) const;

	void set_per_frame(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& lightPosEye,
	                   const glm::mat4 shadowMatrices[4], const glm::vec4& cascadeSplits,
	                   int cascadeCount, const glm::vec4& cascadeBiases);
	void set_per_draw(const glm::mat4& model, const glm::mat3& normalMatrix);

 private:
	bool loaded_;
	GLuint per_frame_ubo_;
	GLuint per_draw_ubo_;
	std::vector<std::string> shader_names_;
	std::vector<ShaderProgram::ShPtr> programs_;
	std::vector<VertexShader::ShPtr> vshaders_;
  std::vector<FragmentShader::ShPtr> fshaders_;
  
	void init();
  
  // Iterates through the list of shaders that need to be loaded and loads them. First checks to see if
	// shaders have been loaded to prevent duplicate loadings
	void load_shaders();

  char* load_shader(char *fn);
  void bindStandardAttribs(int program);
	void print_shader_log(int id);
  void print_program_log(int id);
  
	ShaderManager(const ShaderManager&) = delete;
	ShaderManager& operator=(const ShaderManager&) = delete;
};

#endif
