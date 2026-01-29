#include "Material.hpp"

Material::Material(std::string name)
  : textured_(false), name_(name) { }

std::string Material::get_name() const {
	return name_;
}

bool Material::is_name(const std::string& rhs) const {
	return name_.compare(rhs) == 0;
}
