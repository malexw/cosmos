#include "ShaderProgram.hpp"

const std::string ShaderProgram::get_name() const {
	return name_;
}

const bool ShaderProgram::is_name(const std::string& rhs) const {
	return name_.compare(rhs) == 0;
}
