#ifndef COSMOS_MATERIALMANAGER_H_
#define COSMOS_MATERIALMANAGER_H_

#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"

#include "TextureManager.hpp"
#include "FileBlob.hpp"
#include "Material.hpp"
#include "util.hpp"

class MaterialManager {
 public:
  typedef boost::shared_ptr<MaterialManager> ShPtr;
  typedef std::map<std::string, Material::ShPtr> MaterialTable;

  explicit MaterialManager(TextureManager::ShPtr texture_manager);

  /*
   * Look up the requested material in the materials table. If the material isn't there, try to load it.
   */
  const Material::ShPtr get_material(const std::string& path);

 private:
  TextureManager::ShPtr texture_manager_;
  MaterialTable materials_;

  void init();

  /*
   * Attempts to load and parse the material file pointed at by path. Returns true if the material is successfully
   * loaded, false otherwise.
   */
  const bool load_material(const std::string& path);

  // OBJ Decoding stuff -------------
  Material::ShPtr decode(FileBlob& b);
  // Returns the index of the first character following a group of newline characters after the offset
  const unsigned int newline_index(const FileBlob& b, const unsigned int offset) const;
  // Returns a collection of whitespace-separated character strings occuring between offset and the end of the
  // line
  const std::vector<std::string> Tokenize(const FileBlob& b, const unsigned int offset) const;

  DISALLOW_COPY_AND_ASSIGN(MaterialManager);
};

#endif
