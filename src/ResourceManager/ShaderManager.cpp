#include <iostream>
#include <vector>

#include <glm/glm.hpp>

#include "CosmosConfig.hpp"
#include "FileBlob.hpp"
#include "ShaderManager.hpp"
#include "TextureManager.hpp"

ShaderManager::ShaderManager()
  : loaded_(false), per_frame_ubo_(0), per_draw_ubo_(0) {
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
  shader_names_.push_back(std::string("res/shaders/flat.vert"));
  shader_names_.push_back(std::string("res/shaders/flat.frag"));
  shader_names_.push_back(std::string("res/shaders/unlit.vert"));
  shader_names_.push_back(std::string("res/shaders/unlit.frag"));
  shader_names_.push_back(std::string("res/shaders/simple.vert"));
  shader_names_.push_back(std::string("res/shaders/simple.frag"));
  shader_names_.push_back(std::string("res/shaders/blinn.frag"));
  shader_names_.push_back(std::string("res/shaders/particle.vert"));
  shader_names_.push_back(std::string("res/shaders/particle.frag"));
  shader_names_.push_back(std::string("res/shaders/bumpdec_instanced.vert"));
  shader_names_.push_back(std::string("res/shaders/flat_instanced.vert"));
  shader_names_.push_back(std::string("res/shaders/resolve.frag"));
  shader_names_.push_back(std::string("res/shaders/ssao.frag"));
  shader_names_.push_back(std::string("res/shaders/ssao_blur.frag"));
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
  bindStandardAttribs(p);
  glLinkProgram(p);
  print_program_log(p);
  ShaderProgram::ShPtr program(new ShaderProgram("bumpdec", p));
  programs_.push_back(program);
  glUseProgram(p);
  
  GLint texSampler = glGetUniformLocation(p, "tex");
  GLint bumpSampler = glGetUniformLocation(p, "bump");
  GLint decalSampler = glGetUniformLocation(p, "decal");
  GLint shadowSampler = glGetUniformLocation(p, "shadowMap");
  glUniform1i(texSampler, 0);
  glUniform1i(bumpSampler, 1);
  glUniform1i(decalSampler, 2);
  glUniform1i(shadowSampler, 3);

  // The shadow program
  p = glCreateProgram();
  v = vshaders_[1]->get_id();
  f = fshaders_[1]->get_id();

  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glLinkProgram(p);
  print_program_log(p);
  ShaderProgram::ShPtr shadow(new ShaderProgram("shadow", p));
  programs_.push_back(shadow);
  glUseProgram(p);
  texSampler = glGetUniformLocation(p, "tex");
  shadowSampler = glGetUniformLocation(p, "shadowMap");
  glUniform1i(texSampler, 0);
  glUniform1i(shadowSampler, 3);

  // HDR program
  p = glCreateProgram();
  v = vshaders_[2]->get_id();
  f = fshaders_[2]->get_id();

  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glLinkProgram(p);
  print_program_log(p);
  ShaderProgram::ShPtr hdr(new ShaderProgram("hdr", p));
  programs_.push_back(hdr);
  // Set default exposure from HDR texture (used by resolve shader)
  Texture::ShPtr hdrTex = TextureManager::get().get_texture("res/textures/qwantani.hdr");
  float default_exp = hdrTex ? hdrTex->get_default_exposure() : 1.0f;
  CosmosConfig::get().set_exposure(default_exp);
  
  // The bump program
  p = glCreateProgram();
  v = vshaders_[3]->get_id();
  f = fshaders_[3]->get_id();

  glAttachShader(p,v);
  glAttachShader(p,f);
  bindStandardAttribs(p);
  glLinkProgram(p);
  print_program_log(p);
  ShaderProgram::ShPtr bump(new ShaderProgram("bump", p));
  programs_.push_back(bump);
  glUseProgram(p);
  
  texSampler = glGetUniformLocation(p, "tex");
  bumpSampler = glGetUniformLocation(p, "bump");
  glUniform1i(texSampler, 0);
  glUniform1i(bumpSampler, 1);

  // The flat program (wireframes, depth-only passes)
  p = glCreateProgram();
  v = vshaders_[4]->get_id();
  f = fshaders_[4]->get_id();

  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glLinkProgram(p);
  print_program_log(p);
  ShaderProgram::ShPtr flat(new ShaderProgram("flat", p));
  programs_.push_back(flat);

  // The unlit program (textured, no lighting — skybox, HUD, particles)
  p = glCreateProgram();
  v = vshaders_[5]->get_id();
  f = fshaders_[5]->get_id();

  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glLinkProgram(p);
  print_program_log(p);
  ShaderProgram::ShPtr unlit(new ShaderProgram("unlit", p));
  programs_.push_back(unlit);
  glUseProgram(p);
  texSampler = glGetUniformLocation(p, "tex");
  glUniform1i(texSampler, 0);

  // The simple program (per-vertex lighting, no shadows)
  p = glCreateProgram();
  v = vshaders_[6]->get_id();
  f = fshaders_[6]->get_id();

  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glLinkProgram(p);
  print_program_log(p);
  ShaderProgram::ShPtr simple(new ShaderProgram("simple", p));
  programs_.push_back(simple);

  // The blinn program (untextured diffuse lighting — bumpdec.vert + blinn.frag)
  p = glCreateProgram();
  v = vshaders_[0]->get_id();
  f = fshaders_[7]->get_id();

  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glLinkProgram(p);
  print_program_log(p);
  ShaderProgram::ShPtr blinn(new ShaderProgram("blinn", p));
  programs_.push_back(blinn);
  glUseProgram(p);
  shadowSampler = glGetUniformLocation(p, "shadowMap");
  glUniform1i(shadowSampler, 3);

  // The particle program (instanced, textured, color-tinted billboard)
  p = glCreateProgram();
  v = vshaders_[7]->get_id();   // particle.vert
  f = fshaders_[8]->get_id();   // particle.frag
  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glBindAttribLocation(p, 6, "instancePosition");
  glBindAttribLocation(p, 7, "instanceColor");
  glBindAttribLocation(p, 8, "instanceScale");
  glLinkProgram(p);
  print_program_log(p);
  ShaderProgram::ShPtr particle(new ShaderProgram("particle", p));
  programs_.push_back(particle);
  glUseProgram(p);
  GLint particleTexLoc = glGetUniformLocation(p, "tex");
  glUniform1i(particleTexLoc, 0);

  // The bumpdec_instanced program (instanced terrain rendering)
  p = glCreateProgram();
  v = vshaders_[8]->get_id();   // bumpdec_instanced.vert
  f = fshaders_[0]->get_id();   // bumpdec.frag
  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glBindAttribLocation(p, 6, "instanceModelCol0");
  glBindAttribLocation(p, 7, "instanceModelCol1");
  glBindAttribLocation(p, 8, "instanceModelCol2");
  glBindAttribLocation(p, 9, "instanceModelCol3");
  glLinkProgram(p);
  print_program_log(p);
  ShaderProgram::ShPtr bumpdecInst(new ShaderProgram("bumpdec_instanced", p));
  programs_.push_back(bumpdecInst);
  glUseProgram(p);
  texSampler = glGetUniformLocation(p, "tex");
  bumpSampler = glGetUniformLocation(p, "bump");
  decalSampler = glGetUniformLocation(p, "decal");
  shadowSampler = glGetUniformLocation(p, "shadowMap");
  glUniform1i(texSampler, 0);
  glUniform1i(bumpSampler, 1);
  glUniform1i(decalSampler, 2);
  glUniform1i(shadowSampler, 3);

  // The flat_instanced program (instanced shadow pass)
  p = glCreateProgram();
  v = vshaders_[9]->get_id();   // flat_instanced.vert
  f = fshaders_[4]->get_id();   // flat.frag
  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glBindAttribLocation(p, 6, "instanceModelCol0");
  glBindAttribLocation(p, 7, "instanceModelCol1");
  glBindAttribLocation(p, 8, "instanceModelCol2");
  glBindAttribLocation(p, 9, "instanceModelCol3");
  glLinkProgram(p);
  print_program_log(p);
  ShaderProgram::ShPtr flatInst(new ShaderProgram("flat_instanced", p));
  programs_.push_back(flatInst);

  // The blinn_instanced program (instanced untextured materials)
  p = glCreateProgram();
  v = vshaders_[8]->get_id();   // bumpdec_instanced.vert
  f = fshaders_[7]->get_id();   // blinn.frag
  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glBindAttribLocation(p, 6, "instanceModelCol0");
  glBindAttribLocation(p, 7, "instanceModelCol1");
  glBindAttribLocation(p, 8, "instanceModelCol2");
  glBindAttribLocation(p, 9, "instanceModelCol3");
  glLinkProgram(p);
  print_program_log(p);
  ShaderProgram::ShPtr blinnInst(new ShaderProgram("blinn_instanced", p));
  programs_.push_back(blinnInst);
  glUseProgram(p);
  shadowSampler = glGetUniformLocation(p, "shadowMap");
  glUniform1i(shadowSampler, 3);

  // The resolve program (fullscreen HDR-to-SDR tone mapping)
  p = glCreateProgram();
  v = vshaders_[5]->get_id();   // unlit.vert
  f = fshaders_[9]->get_id();   // resolve.frag
  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glLinkProgram(p);
  print_program_log(p);
  ShaderProgram::ShPtr resolve(new ShaderProgram("resolve", p));
  programs_.push_back(resolve);
  glUseProgram(p);
  texSampler = glGetUniformLocation(p, "tex");
  glUniform1i(texSampler, 0);
  GLint aoSampler = glGetUniformLocation(p, "aoTex");
  glUniform1i(aoSampler, 1);

  // The SSAO program (unlit.vert + ssao.frag)
  p = glCreateProgram();
  v = vshaders_[5]->get_id();   // unlit.vert
  f = fshaders_[10]->get_id();  // ssao.frag
  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glLinkProgram(p);
  print_program_log(p);
  ShaderProgram::ShPtr ssao(new ShaderProgram("ssao", p));
  programs_.push_back(ssao);
  glUseProgram(p);
  GLint depthSampler = glGetUniformLocation(p, "depthTex");
  GLint noiseSampler = glGetUniformLocation(p, "noiseTex");
  glUniform1i(depthSampler, 0);
  glUniform1i(noiseSampler, 1);

  // The SSAO blur program (unlit.vert + ssao_blur.frag)
  p = glCreateProgram();
  v = vshaders_[5]->get_id();   // unlit.vert
  f = fshaders_[11]->get_id();  // ssao_blur.frag
  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glLinkProgram(p);
  print_program_log(p);
  ShaderProgram::ShPtr ssaoBlur(new ShaderProgram("ssao_blur", p));
  programs_.push_back(ssaoBlur);
  glUseProgram(p);
  GLint ssaoTexSampler = glGetUniformLocation(p, "ssaoTex");
  glUniform1i(ssaoTexSampler, 0);
  GLint blurDepthSampler = glGetUniformLocation(p, "depthTex");
  glUniform1i(blurDepthSampler, 1);

  // PerFrame UBO (binding point 0)
  glGenBuffers(1, &per_frame_ubo_);
  glBindBuffer(GL_UNIFORM_BUFFER, per_frame_ubo_);
  glBufferData(GL_UNIFORM_BUFFER, 208, nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, per_frame_ubo_);

  // PerDraw UBO (binding point 1)
  glGenBuffers(1, &per_draw_ubo_);
  glBindBuffer(GL_UNIFORM_BUFFER, per_draw_ubo_);
  glBufferData(GL_UNIFORM_BUFFER, 112, nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, per_draw_ubo_);

  // Bind uniform blocks for bumpdec (programs_[0])
  GLuint blockIdx;
  blockIdx = glGetUniformBlockIndex(programs_[0]->get_id(), "PerFrame");
  if (blockIdx != GL_INVALID_INDEX) glUniformBlockBinding(programs_[0]->get_id(), blockIdx, 0);
  blockIdx = glGetUniformBlockIndex(programs_[0]->get_id(), "PerDraw");
  if (blockIdx != GL_INVALID_INDEX) glUniformBlockBinding(programs_[0]->get_id(), blockIdx, 1);

  // Bind uniform blocks for blinn (programs_[7])
  blockIdx = glGetUniformBlockIndex(programs_[7]->get_id(), "PerFrame");
  if (blockIdx != GL_INVALID_INDEX) glUniformBlockBinding(programs_[7]->get_id(), blockIdx, 0);
  blockIdx = glGetUniformBlockIndex(programs_[7]->get_id(), "PerDraw");
  if (blockIdx != GL_INVALID_INDEX) glUniformBlockBinding(programs_[7]->get_id(), blockIdx, 1);

  // Bind uniform blocks for bumpdec_instanced (programs_[9])
  blockIdx = glGetUniformBlockIndex(programs_[9]->get_id(), "PerFrame");
  if (blockIdx != GL_INVALID_INDEX) glUniformBlockBinding(programs_[9]->get_id(), blockIdx, 0);

  // Bind uniform blocks for blinn_instanced (programs_[11])
  blockIdx = glGetUniformBlockIndex(programs_[11]->get_id(), "PerFrame");
  if (blockIdx != GL_INVALID_INDEX) glUniformBlockBinding(programs_[11]->get_id(), blockIdx, 0);

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

void ShaderManager::bindStandardAttribs(int program) {
  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "texCoord");
  glBindAttribLocation(program, 2, "normal");
  glBindAttribLocation(program, 3, "color");
  glBindAttribLocation(program, 4, "tangent");
  glBindAttribLocation(program, 5, "bitangent");
}

void ShaderManager::print_shader_log(int id) {

  int length = 0;
  int count = 0;

  glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

  if (length > 0) {
    std::vector<char> data(length);
    glGetShaderInfoLog(id, length, &count, data.data());
    std::cout << data.data();
  }
}

void ShaderManager::print_program_log(int id) {

  int length = 0;
  int count = 0;

  glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);

  if (length > 0) {
    std::vector<char> data(length);
    glGetProgramInfoLog(id, length, &count, data.data());
    std::cout << data.data();
  }
}

void ShaderManager::set_per_frame(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& light, const glm::mat4& shadowMatrix) {
    struct {
        glm::mat4 projection;
        glm::mat4 view;
        glm::vec4 lightPosEye;
        glm::mat4 shadowMatrix;
    } data;
    data.projection = projection;
    data.view = view;
    data.lightPosEye = glm::vec4(light, 0.0f);
    data.shadowMatrix = shadowMatrix;
    glBindBuffer(GL_UNIFORM_BUFFER, per_frame_ubo_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(data), &data);
}

void ShaderManager::set_per_draw(const glm::mat4& model, const glm::mat3& normal) {
    struct {
        glm::mat4 model;
        glm::vec4 n0, n1, n2;
    } data;
    data.model = model;
    data.n0 = glm::vec4(normal[0], 0.0f);
    data.n1 = glm::vec4(normal[1], 0.0f);
    data.n2 = glm::vec4(normal[2], 0.0f);
    glBindBuffer(GL_UNIFORM_BUFFER, per_draw_ubo_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(data), &data);
}
