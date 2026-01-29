#ifndef COSMOS_FREADER_H_
#define COSMOS_FREADER_H_

#include <string>
#include <fstream>

#include "util.hpp"

/*
 *  Reads a file from disk
 */
class FileReader {

 public:
  explicit FileReader(const std::string path);
  ~FileReader();

  // Returns the size of the file in bytes
  int GetFileSize();

  // Copy the contents of the file into the supplied buffer
  void Read(char * const buf, int fsize);

 private:
  std::string file_path_;
  std::ifstream file_stream_;

  DISALLOW_COPY_AND_ASSIGN(FileReader);
};

#endif
