#include <iostream>
#include <memory>
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
	shader_names_.emplace_back("res/shaders/bumpdec.vert");       // vshaders_[0]
  shader_names_.emplace_back("res/shaders/bumpdec.frag");       // fshaders_[0]
  shader_names_.emplace_back("res/shaders/hdr.vert");           // vshaders_[1]
  shader_names_.emplace_back("res/shaders/hdr.frag");           // fshaders_[1]
  shader_names_.emplace_back("res/shaders/bump.vert");          // vshaders_[2]
  shader_names_.emplace_back("res/shaders/bump.frag");          // fshaders_[2]
  shader_names_.emplace_back("res/shaders/flat.vert");          // vshaders_[3]
  shader_names_.emplace_back("res/shaders/flat.frag");          // fshaders_[3]
  shader_names_.emplace_back("res/shaders/unlit.vert");         // vshaders_[4]
  shader_names_.emplace_back("res/shaders/unlit.frag");         // fshaders_[4]
  shader_names_.emplace_back("res/shaders/simple.vert");        // vshaders_[5]
  shader_names_.emplace_back("res/shaders/simple.frag");        // fshaders_[5]
  shader_names_.emplace_back("res/shaders/blinn.frag");         // fshaders_[6]
  shader_names_.emplace_back("res/shaders/particle.vert");      // vshaders_[6]
  shader_names_.emplace_back("res/shaders/particle.frag");      // fshaders_[7]
  shader_names_.emplace_back("res/shaders/bumpdec_instanced.vert"); // vshaders_[7]
  shader_names_.emplace_back("res/shaders/flat_instanced.vert");    // vshaders_[8]
  shader_names_.emplace_back("res/shaders/resolve.frag");       // fshaders_[8]
  shader_names_.emplace_back("res/shaders/ssao.frag");          // fshaders_[9]
  shader_names_.emplace_back("res/shaders/ssao_blur.frag");     // fshaders_[10]
  shader_names_.emplace_back("res/shaders/cube_debug.frag");    // fshaders_[11]
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
    auto file = std::make_shared<FileBlob>(shader_names_[j]);
    if (file->extension() == "vert") {
      int vname = glCreateShader(GL_VERTEX_SHADER);
      auto vshader = std::make_shared<VertexShader>(shader_names_[j], vname);
      vshaders_.push_back(vshader);
      const char* src = file->get_bytes();
      glShaderSource(vname, 1, &src, 0);
      glCompileShader(vname);
      print_shader_log(vname);
    } else if (file->extension() == "frag") {
      int fname = glCreateShader(GL_FRAGMENT_SHADER);
      auto fshader = std::make_shared<FragmentShader>(shader_names_[j], fname);
      fshaders_.push_back(fshader);
      const char* src = file->get_bytes();
      glShaderSource(fname, 1, &src, 0);
      glCompileShader(fname);
      print_shader_log(fname);
    } else {
      std::cout << "ShaderManager error: shader type not recognized" << std::endl;
    }
	}
  // The bumpdec program (programs_[0])
  int p = glCreateProgram();
  int v = vshaders_[0]->get_id();
  int f = fshaders_[0]->get_id();

  glAttachShader(p,v);
  glAttachShader(p,f);
  bindStandardAttribs(p);
  glLinkProgram(p);
  print_program_log(p);
  auto program = std::make_shared<ShaderProgram>("bumpdec", p);
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

  // HDR program (programs_[1])
  p = glCreateProgram();
  v = vshaders_[1]->get_id();
  f = fshaders_[1]->get_id();

  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glLinkProgram(p);
  print_program_log(p);
  auto hdr = std::make_shared<ShaderProgram>("hdr", p);
  programs_.push_back(hdr);
  // Set default exposure from HDR texture (used by resolve shader)
  Texture::ShPtr hdrTex = TextureManager::get().get_texture("res/textures/qwantani.hdr");
  float default_exp = hdrTex ? hdrTex->get_default_exposure() : 1.0f;
  CosmosConfig::get().set_exposure(default_exp);

  // The bump program (programs_[2])
  p = glCreateProgram();
  v = vshaders_[2]->get_id();
  f = fshaders_[2]->get_id();

  glAttachShader(p,v);
  glAttachShader(p,f);
  bindStandardAttribs(p);
  glLinkProgram(p);
  print_program_log(p);
  auto bump = std::make_shared<ShaderProgram>("bump", p);
  programs_.push_back(bump);
  glUseProgram(p);

  texSampler = glGetUniformLocation(p, "tex");
  bumpSampler = glGetUniformLocation(p, "bump");
  glUniform1i(texSampler, 0);
  glUniform1i(bumpSampler, 1);

  // The flat program (programs_[3])
  p = glCreateProgram();
  v = vshaders_[3]->get_id();
  f = fshaders_[3]->get_id();

  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glLinkProgram(p);
  print_program_log(p);
  auto flat = std::make_shared<ShaderProgram>("flat", p);
  programs_.push_back(flat);

  // The unlit program (programs_[4])
  p = glCreateProgram();
  v = vshaders_[4]->get_id();
  f = fshaders_[4]->get_id();

  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glLinkProgram(p);
  print_program_log(p);
  auto unlit = std::make_shared<ShaderProgram>("unlit", p);
  programs_.push_back(unlit);
  glUseProgram(p);
  texSampler = glGetUniformLocation(p, "tex");
  glUniform1i(texSampler, 0);

  // The simple program (programs_[5])
  p = glCreateProgram();
  v = vshaders_[5]->get_id();
  f = fshaders_[5]->get_id();

  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glLinkProgram(p);
  print_program_log(p);
  auto simple = std::make_shared<ShaderProgram>("simple", p);
  programs_.push_back(simple);

  // The blinn program (programs_[6])
  p = glCreateProgram();
  v = vshaders_[0]->get_id();
  f = fshaders_[6]->get_id();

  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glLinkProgram(p);
  print_program_log(p);
  auto blinn = std::make_shared<ShaderProgram>("blinn", p);
  programs_.push_back(blinn);
  glUseProgram(p);
  shadowSampler = glGetUniformLocation(p, "shadowMap");
  glUniform1i(shadowSampler, 3);

  // The particle program (programs_[7])
  p = glCreateProgram();
  v = vshaders_[6]->get_id();   // particle.vert
  f = fshaders_[7]->get_id();   // particle.frag
  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glBindAttribLocation(p, 6, "instancePosition");
  glBindAttribLocation(p, 7, "instanceColor");
  glBindAttribLocation(p, 8, "instanceScale");
  glLinkProgram(p);
  print_program_log(p);
  auto particle = std::make_shared<ShaderProgram>("particle", p);
  programs_.push_back(particle);
  glUseProgram(p);
  GLint particleTexLoc = glGetUniformLocation(p, "tex");
  glUniform1i(particleTexLoc, 0);

  // The bumpdec_instanced program (programs_[8])
  p = glCreateProgram();
  v = vshaders_[7]->get_id();   // bumpdec_instanced.vert
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
  auto bumpdecInst = std::make_shared<ShaderProgram>("bumpdec_instanced", p);
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

  // The flat_instanced program (programs_[9])
  p = glCreateProgram();
  v = vshaders_[8]->get_id();   // flat_instanced.vert
  f = fshaders_[3]->get_id();   // flat.frag
  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glBindAttribLocation(p, 6, "instanceModelCol0");
  glBindAttribLocation(p, 7, "instanceModelCol1");
  glBindAttribLocation(p, 8, "instanceModelCol2");
  glBindAttribLocation(p, 9, "instanceModelCol3");
  glLinkProgram(p);
  print_program_log(p);
  auto flatInst = std::make_shared<ShaderProgram>("flat_instanced", p);
  programs_.push_back(flatInst);

  // The blinn_instanced program (programs_[10])
  p = glCreateProgram();
  v = vshaders_[7]->get_id();   // bumpdec_instanced.vert
  f = fshaders_[6]->get_id();   // blinn.frag
  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glBindAttribLocation(p, 6, "instanceModelCol0");
  glBindAttribLocation(p, 7, "instanceModelCol1");
  glBindAttribLocation(p, 8, "instanceModelCol2");
  glBindAttribLocation(p, 9, "instanceModelCol3");
  glLinkProgram(p);
  print_program_log(p);
  auto blinnInst = std::make_shared<ShaderProgram>("blinn_instanced", p);
  programs_.push_back(blinnInst);
  glUseProgram(p);
  shadowSampler = glGetUniformLocation(p, "shadowMap");
  glUniform1i(shadowSampler, 3);

  // The resolve program (programs_[11])
  p = glCreateProgram();
  v = vshaders_[4]->get_id();   // unlit.vert
  f = fshaders_[8]->get_id();   // resolve.frag
  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glLinkProgram(p);
  print_program_log(p);
  auto resolve = std::make_shared<ShaderProgram>("resolve", p);
  programs_.push_back(resolve);
  glUseProgram(p);
  texSampler = glGetUniformLocation(p, "tex");
  glUniform1i(texSampler, 0);
  GLint aoSampler = glGetUniformLocation(p, "aoTex");
  glUniform1i(aoSampler, 1);

  // The SSAO program (programs_[12])
  p = glCreateProgram();
  v = vshaders_[4]->get_id();   // unlit.vert
  f = fshaders_[9]->get_id();   // ssao.frag
  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glLinkProgram(p);
  print_program_log(p);
  auto ssao = std::make_shared<ShaderProgram>("ssao", p);
  programs_.push_back(ssao);
  glUseProgram(p);
  GLint depthSampler = glGetUniformLocation(p, "depthTex");
  GLint noiseSampler = glGetUniformLocation(p, "noiseTex");
  glUniform1i(depthSampler, 0);
  glUniform1i(noiseSampler, 1);

  // The SSAO blur program (programs_[13])
  p = glCreateProgram();
  v = vshaders_[4]->get_id();   // unlit.vert
  f = fshaders_[10]->get_id();  // ssao_blur.frag
  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glLinkProgram(p);
  print_program_log(p);
  auto ssaoBlur = std::make_shared<ShaderProgram>("ssao_blur", p);
  programs_.push_back(ssaoBlur);
  glUseProgram(p);
  GLint ssaoTexSampler = glGetUniformLocation(p, "ssaoTex");
  glUniform1i(ssaoTexSampler, 0);
  GLint blurDepthSampler = glGetUniformLocation(p, "depthTex");
  glUniform1i(blurDepthSampler, 1);

  // The cube map debug program (programs_[14])
  p = glCreateProgram();
  v = vshaders_[4]->get_id();   // unlit.vert
  f = fshaders_[11]->get_id();  // cube_debug.frag
  glAttachShader(p, v);
  glAttachShader(p, f);
  bindStandardAttribs(p);
  glLinkProgram(p);
  print_program_log(p);
  auto cubeDebug = std::make_shared<ShaderProgram>("cube_debug", p);
  programs_.push_back(cubeDebug);
  glUseProgram(p);
  GLint cubeSampler = glGetUniformLocation(p, "cubeMap");
  glUniform1i(cubeSampler, 0);

  // PerFrame UBO (binding point 0)
  glGenBuffers(1, &per_frame_ubo_);
  glBindBuffer(GL_UNIFORM_BUFFER, per_frame_ubo_);
  glBufferData(GL_UNIFORM_BUFFER, 688, nullptr, GL_DYNAMIC_DRAW);
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

  // Bind uniform blocks for blinn (programs_[6])
  blockIdx = glGetUniformBlockIndex(programs_[6]->get_id(), "PerFrame");
  if (blockIdx != GL_INVALID_INDEX) glUniformBlockBinding(programs_[6]->get_id(), blockIdx, 0);
  blockIdx = glGetUniformBlockIndex(programs_[6]->get_id(), "PerDraw");
  if (blockIdx != GL_INVALID_INDEX) glUniformBlockBinding(programs_[6]->get_id(), blockIdx, 1);

  // Bind uniform blocks for bumpdec_instanced (programs_[8])
  blockIdx = glGetUniformBlockIndex(programs_[8]->get_id(), "PerFrame");
  if (blockIdx != GL_INVALID_INDEX) glUniformBlockBinding(programs_[8]->get_id(), blockIdx, 0);

  // Bind uniform blocks for blinn_instanced (programs_[10])
  blockIdx = glGetUniformBlockIndex(programs_[10]->get_id(), "PerFrame");
  if (blockIdx != GL_INVALID_INDEX) glUniformBlockBinding(programs_[10]->get_id(), blockIdx, 0);

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

void ShaderManager::set_per_frame(const glm::mat4& projection, const glm::mat4& view,
                                   const glm::mat4 shadowMatrices[4],
                                   const glm::vec4& cascadeSplits, int cascadeCount,
                                   const glm::vec4& cascadeBiases,
                                   const glm::vec4 lightPosDir[kMaxLights],
                                   const glm::vec4 lightColor[kMaxLights], int lightCount) {
    struct {
        glm::mat4 projection;              // offset 0
        glm::mat4 view;                    // offset 64
        glm::mat4 shadowMatrices[4];       // offset 128
        glm::vec4 cascadeSplits;           // offset 384
        glm::ivec4 counts;                 // offset 400 (x = cascadeCount, y = lightCount)
        glm::vec4 cascadeBiases;           // offset 416
        glm::vec4 lightPosDir[kMaxLights]; // offset 432
        glm::vec4 lightColor[kMaxLights];  // offset 560
    } data;
    data.projection = projection;
    data.view = view;
    for (int i = 0; i < 4; ++i) data.shadowMatrices[i] = shadowMatrices[i];
    data.cascadeSplits = cascadeSplits;
    data.counts = glm::ivec4(cascadeCount, lightCount, 0, 0);
    data.cascadeBiases = cascadeBiases;
    for (int i = 0; i < kMaxLights; ++i) {
        data.lightPosDir[i] = i < lightCount ? lightPosDir[i] : glm::vec4(0.0f);
        data.lightColor[i] = i < lightCount ? lightColor[i] : glm::vec4(0.0f);
    }
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
