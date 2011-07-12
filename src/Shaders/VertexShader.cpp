#include "VertexShader.hpp"

const std::string VertexShader::get_name() const {
	return name_;
}

const bool VertexShader::is_name(const std::string& rhs) const {
	return name_.compare(rhs) == 0;
}
