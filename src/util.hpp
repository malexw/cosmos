#ifndef COSMOS_UTIL_HPP
#define COSMOS_UTIL_HPP

#include <string>
#include <vector>

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
// Sourced from Google - see also: Effective C++ by Scott Meyers
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

// Simple string split function (replaces boost::split)
inline std::vector<std::string> split(const std::string& str, const std::string& delimiters) {
  std::vector<std::string> tokens;
  size_t start = 0;
  size_t end = 0;
  while ((end = str.find_first_of(delimiters, start)) != std::string::npos) {
    if (end > start) {
      tokens.push_back(str.substr(start, end - start));
    }
    start = end + 1;
  }
  if (start < str.length()) {
    tokens.push_back(str.substr(start));
  }
  return tokens;
}

#endif
