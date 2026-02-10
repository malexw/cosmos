#include <fstream>

#include "FileBlob.hpp"

FileBlob::FileBlob(std::string path) : file_path_(std::move(path)) {
  auto fsize = std::filesystem::file_size(file_path_);
  bytes_.resize(fsize);
  std::ifstream ifs(file_path_, std::ios::binary);
  ifs.read(bytes_.data(), fsize);
}
