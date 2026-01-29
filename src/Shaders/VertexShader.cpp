#include "VertexShader.hpp"

std::string VertexShader::get_name() const {
	return name_;
}

bool VertexShader::is_name(const std::string& rhs) const {
	return name_.compare(rhs) == 0;
}
