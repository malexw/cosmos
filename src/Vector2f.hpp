#ifndef COSMOS_VECTOR2F_H_
#define COSMOS_VECTOR2F_H_

#include <iostream>

#include <memory>

#include "util.hpp"

// Basic object for representing a vector in 2 dimensions
class Vector2f {
 public: 
	typedef std::shared_ptr<Vector2f> ShPtr;
  
  Vector2f(): u_(0.0f), v_(0.0f) {}
	Vector2f(float u, float v): u_(u), v_(v) {}
  Vector2f(const Vector2f& rhs): u_(rhs.u()), v_(rhs.v()) {}
  
  Vector2f& operator =(const Vector2f& rhs) {
    u_ = rhs.u();
    v_ = rhs.v();
    return *this;
  }
  
  float& u() { return u_; }
  float u() const { return u_; }
  float& v() { return v_; }
  float v() const { return v_; }
  
 private:

  float u_, v_;
};

inline std::ostream& operator <<(std::ostream& os, const Vector2f& v)
{
  return os << "vt<" << v.u() << "," << v.v() << ">";
}

#endif
