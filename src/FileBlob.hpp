#ifndef COSMOS_FILEBLOB_H_
#define COSMOS_FILEBLOB_H_

#include <string>

#include <memory>

#include "util.hpp"

// A wrapper for the binary contents of a file. Makes the bytes of a file available like an array.
class FileBlob {

 public:
  typedef std::shared_ptr<FileBlob> ShPtr;
  
  // Build a new instance based on a supplied path
  explicit FileBlob(std::string path);
  ~FileBlob();
  
  const char& operator[](std::size_t position) const { return bytes_[position]; }
  int size() const;
  const std::string extension() const;
  const std::string path() const { return file_path_; }
  char* get_bytes() { return bytes_; }

 private:
  void init();

  std::string file_path_;
  int file_size_;
  
  // TODO: Change this to a boost::shared_array
  char * bytes_;

  DISALLOW_COPY_AND_ASSIGN(FileBlob);
};

#endif
