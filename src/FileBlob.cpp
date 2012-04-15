#include <iostream>
#include <string>

#include "FileBlob.hpp"
#include "Freader.hpp"

FileBlob::FileBlob(std::string path)
      : file_path_ (path),
        file_size_ (0),
      bytes_ (NULL) {
  init();
}

FileBlob::~FileBlob() {
  delete[] bytes_;
}

int FileBlob::size() const {
  return file_size_;
}

const std::string FileBlob::extension() const {
  std::string ext;
  int i = file_path_.length();

  // TODO Is there a Boost method for doing a string split?
  while (i > 0 && file_path_[i] != '.') {
    --i;
  }
  if (i > 0) {
    ext = file_path_.substr(i+1, file_path_.length() - (i+1));
  }

  return ext;
}

void FileBlob::init() {
  FileReader fr(file_path_);
  valid_ = fr.exists();
  file_size_ = fr.GetFileSize();

  // +1 for a null termination (needed when using FileBlob for loading shaders)
  bytes_ = new char[file_size_+1];
  fr.Read(bytes_, file_size_);
  bytes_[file_size_] = '\0';
}
