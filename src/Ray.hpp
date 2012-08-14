#ifndef COSMOS_RAY_H_
#define COSMOS_RAY_H_

#include <iostream>
#include <math.h>

#include <boost/shared_ptr.hpp>

#include "util.hpp"
#include "Vector2f.hpp"
#include "Vector3f.hpp"

// A Ray is a line that passes through a point pos_ with direction dir_
// Should be constructed with a normalized dir_ vector
class Ray {
 public:
  typedef boost::shared_ptr<Ray> ShPtr;
  static const Ray UNIT_X;
  static const Ray UNIT_Y;
  static const Ray UNIT_Z;

  Ray(): pos_(Vector3f::ZEROS), dir_(Vector3f::UNIT_X) {}
  Ray(float x, float y, float z, float a, float b, float c): pos_(Vector3f(x, y, z)), dir_(Vector3f(a, b, c)) {}
  Ray(Vector3f position, Vector3f direction): pos_(position), dir_(direction) {}
  Ray(const Ray& rhs): pos_(rhs.position()), dir_(rhs.direction()) {}

  Ray& operator=(const Ray& rhs) {
    pos_ = rhs.position();
    dir_ = rhs.direction();
    return *this;
  }

  Vector2f z_intercept() {
    Vector3f p = this->at(-(pos_.z()/dir_.z()));
    return Vector2f(p.x(), p.y());
  }

  Vector3f at(float t) {
    return pos_ + (t*dir_);
  }

  Vector3f& position() { return pos_; }
  const Vector3f position() const { return pos_; }
  Vector3f& direction() { return dir_; }
  const Vector3f direction() const { return dir_; }

 private:

  Vector3f pos_;
  Vector3f dir_;
};

inline std::ostream& operator <<(std::ostream& os, const Ray& r)
{
  return os << "r< " << r.position() << "," << r.direction() << " >";
}

#endif
