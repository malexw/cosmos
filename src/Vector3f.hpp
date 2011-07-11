#ifndef COSMOS_VECTOR3F_H_
#define COSMOS_VECTOR3F_H_

#include <iostream>
#include <math.h>

#include <boost/shared_ptr.hpp>

#include "util.hpp"

// Basic object for representing a vector in 3 dimensions
class Vector3f {
 public: 
	typedef boost::shared_ptr<Vector3f> ShPtr;

  Vector3f(): x_(0.0f), y_(0.0f), z_(0.0f) {}
	Vector3f(float x, float y, float z): x_(x), y_(y), z_(z) {}
  Vector3f(const Vector3f& rhs): x_(rhs.x()), y_(rhs.y()), z_(rhs.z()) {}
  
  Vector3f& operator =(const Vector3f& rhs) {
    x_ = rhs.x();
    y_ = rhs.y();
    z_ = rhs.z();
    return *this;
  }
  
  Vector3f& operator *=(float rhs) {
    x_ *= rhs;
    y_ *= rhs;
    z_ *= rhs;
    return *this;
  }
  
  Vector3f& operator *=(const Vector3f& rhs) {
    x_ *= rhs.x();
    y_ *= rhs.y();
    z_ *= rhs.z();
    return *this;
  }
  
  const Vector3f cross(const Vector3f& rhs) const {
    return Vector3f(
      y_*rhs.z() - z_*rhs.y(),
      z_*rhs.x() - x_*rhs.z(),
      x_*rhs.y() - y_*rhs.x());
  }
  
  const float dot(const Vector3f& rhs) const { return x_*rhs.x() + y_*rhs.y() + z_*rhs.z(); }
  const float lengthSquare() const { return x_*x_ + y_*y_ + z_*z_; }
  const float length() const { return sqrt(lengthSquare()); }
  //Vector3f& normalize();
  const Vector3f normalize() const { float len = 1.0f/length(); return Vector3f(x_*len, y_*len, z_*len); }
  
  float& x() { return x_; }
  const float x() const { return x_; }
  float& y() { return y_; }
  const float y() const { return y_; }
  float& z() { return z_; }
  const float z() const { return z_; }
  
 private:

  float x_, y_, z_;
};

inline const Vector3f operator *(float s, const Vector3f& rhs)
{
  return Vector3f(s*rhs.x(), s*rhs.y(), s*rhs.z());
}

inline const Vector3f operator +(const Vector3f& lhs, const Vector3f& rhs)
{
  return Vector3f(lhs.x()+rhs.x(), lhs.y()+rhs.y(), lhs.z()+rhs.z());
}

inline const Vector3f operator -(const Vector3f& lhs, const Vector3f& rhs)
{
  return Vector3f(lhs.x()-rhs.x(), lhs.y()-rhs.y(), lhs.z()-rhs.z());
}

inline const Vector3f operator -(const Vector3f& v)
{
  return Vector3f(-v.x(), -v.y(), -v.z());
}

inline std::ostream& operator <<(std::ostream& os, const Vector3f& v)
{
  return os << "v<" << v.x() << "," << v.y() << "," << v.z() << ">";
}

#endif
