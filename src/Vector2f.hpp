#ifndef COSMOS_VECTOR2F_H_
#define COSMOS_VECTOR2F_H_

#include <iostream>

#include <boost/shared_ptr.hpp>

#include "util.hpp"

// Basic object for representing a vector in 2 dimensions
class Vector2f {
 public:
  typedef boost::shared_ptr<Vector2f> ShPtr;
  static const Vector2f UNIT_U;
  static const Vector2f NEGATIVE_U;
  static const Vector2f UNIT_V;
  static const Vector2f NEGATIVE_V;
  static const Vector2f ZEROS;
  static const Vector2f ONES;


  Vector2f(): u_(0.0f), v_(0.0f) {}
  Vector2f(float u, float v): u_(u), v_(v) {}
  Vector2f(const Vector2f& rhs): u_(rhs.u()), v_(rhs.v()) {}

  Vector2f& operator=(const Vector2f& rhs) {
    u_ = rhs.u();
    v_ = rhs.v();
    return *this;
  }

  Vector2f& operator+=(float rhs) {
    u_ += rhs;
    v_ += rhs;
    return *this;
  }

  Vector2f& operator+=(const Vector2f& rhs) {
    u_ += rhs.u();
    v_ += rhs.v();
  }

  Vector2f& operator*=(float rhs) {
    u_ *= rhs;
    v_ *= rhs;
    return *this;
  }

  Vector2f& operator*=(const Vector2f& rhs) {
    u_ *= rhs.u();
    v_ *= rhs.v();
    return *this;
  }

  float& u() { return u_; }
  const float u() const { return u_; }
  float& v() { return v_; }
  const float v() const { return v_; }

 private:

  float u_, v_;
};

inline const Vector2f operator *(float s, const Vector2f& rhs)
{
  return Vector2f(s*rhs.u(), s*rhs.v());
}

inline const Vector2f operator +(const Vector2f& lhs, const Vector2f& rhs)
{
  return Vector2f(lhs.u()+rhs.u(), lhs.v()+rhs.v());
}

inline const Vector2f operator -(const Vector2f& lhs, const Vector2f& rhs)
{
  return Vector2f(lhs.u()-rhs.u(), lhs.v()-rhs.v());
}

inline const Vector2f operator -(const Vector2f& v)
{
  return Vector2f(-v.u(), -v.v());
}

inline std::ostream& operator <<(std::ostream& os, const Vector2f& v)
{
  return os << "vt<" << v.u() << "," << v.v() << ">";
}

#endif
