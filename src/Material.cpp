#include "Material.hpp"

Material::Material(std::string name)
  : textured_(false), name_(name) { }

const std::string Material::get_name() const {
  return name_;
}

const bool Material::is_name(const std::string& rhs) const {
  return name_.compare(rhs) == 0;
}
