#ifndef COSMOS_FILEBLOB_H_
#define COSMOS_FILEBLOB_H_

#include <filesystem>
#include <string>

#include <memory>

// A wrapper for the binary contents of a file. Makes the bytes of a file available like an array.
class FileBlob {

 public:
  using ShPtr = std::shared_ptr<FileBlob>;

  // Build a new instance based on a supplied path
  explicit FileBlob(std::string path);

  const char& operator[](std::size_t position) const { return bytes_[position]; }
  int size() const { return static_cast<int>(bytes_.size()); }
  std::string extension() const {
    auto ext = std::filesystem::path(file_path_).extension().string();
    return ext.empty() ? ext : ext.substr(1);
  }
  std::string path() const { return file_path_; }
  char* get_bytes() { return bytes_.data(); }

 private:
  std::string file_path_;
  std::string bytes_;

  FileBlob(const FileBlob&) = delete;
  FileBlob& operator=(const FileBlob&) = delete;
};

#endif
