#ifndef COSMOS_Quaternion_H_
#define COSMOS_Quaternion_H_

#include <iostream>
#include <math.h>

#include <boost/shared_ptr.hpp>

#include "util.hpp"
#include "Vector3f.hpp"

// Represents an axis of rotation and an angle
class Quaternion {
 public:
  typedef boost::shared_ptr<Quaternion> ShPtr;

  Quaternion(): x_(0.0f), y_(0.0f), z_(0.0f), w_(1.0f) {}
  Quaternion(float x, float y, float z, float w): x_(x), y_(y), z_(z), w_(w) {}

  // TODO Clean me up!!
  Quaternion(const Vector3f& x, const Vector3f& y, const Vector3f& z) {
    // Quaternion Calculus and Fast Animation - Ken Shoemake, SIGGRAPH 87

    float fTrace = x.x()+y.y()+z.z();
    float fRoot;

    if ( fTrace > 0.0 )
    {
        // |w| > 1/2, may as well choose w > 1/2
        fRoot = sqrt(fTrace + 1.0f);  // 2w
        w_ = 0.5f*fRoot;
        fRoot = 0.5f/fRoot;  // 1/(4w)
        x_ = (y.z()-z.y())*fRoot;
        y_ = (z.x()-x.z())*fRoot;
        z_ = (x.y()-y.x())*fRoot;
    }
    else
    {
      if (x.x() > y.y() && x.x() > z.z()) {
        fRoot = sqrt(x.x()-y.y()-z.z() + 1.0f);
        x_ = 0.5f*fRoot;
        fRoot = 0.5f/fRoot;
        w_ = (y.z()-z.y())*fRoot;
        y_ = (x.y()+y.x())*fRoot;
        z_ = (x.z()+z.x())*fRoot;
      } else if (y.y() > z.z()) {
        fRoot = sqrt(y.y()-z.z()-x.x() + 1.0f);
        y_ = 0.5f*fRoot;
        fRoot = 0.5f/fRoot;
        w_ = (z.x()-x.z())*fRoot;
        z_ = (y.z()+z.y())*fRoot;
        x_ = (y.x()+x.y())*fRoot;
      } else {
        fRoot = sqrt(z.z()-x.x()-y.y() + 1.0f);
        z_ = 0.5f*fRoot;
        fRoot = 0.5f/fRoot;
        w_ = (x.y()-y.x())*fRoot;
        x_ = (z.x()+x.z())*fRoot;
        y_ = (z.y()+y.z())*fRoot;
      }
    }
  }
  /*Quaternion(Vector3f axis, float angle)
   :  x_(axis.x() * sin(angle * M_PI / 180)),
      y_(axis.y() * sin(angle * M_PI / 180)),
      z_(axis.z() * sin(angle * M_PI / 180)),
      w_(cos(angle * M_PI / 180)) {}*/
      /*Radian fHalfAngle ( 0.5*rfAngle );
        Real fSin = Math::Sin(fHalfAngle);
        w = Math::Cos(fHalfAngle);
        x = fSin*rkAxis.x;
        y = fSin*rkAxis.y;
        z = fSin*rkAxis.z;*/
  Quaternion(Vector3f axis, float angle)
   :  x_(axis.x() * sin(angle * M_PI / 360)),
      y_(axis.y() * sin(angle * M_PI / 360)),
      z_(axis.z() * sin(angle * M_PI / 360)),
      w_(cos(angle * M_PI / 360)) {}
  /*Quaternion(Vector3f axis, float angle)
   :  x_(axis.x() * sin(angle / 2)),
      y_(axis.y() * sin(angle / 2)),
      z_(axis.z() * sin(angle / 2)),
      w_(cos(angle / 2)) {}*/

  Quaternion(const Quaternion& rhs): x_(rhs.x()), y_(rhs.y()), z_(rhs.z()), w_(rhs.w()) {}

  Quaternion& operator =(const Quaternion& rhs) {
    x_ = rhs.x();
    y_ = rhs.y();
    z_ = rhs.z();
    w_ = rhs.w();
    return *this;
  }

  const Quaternion normalize() const {
    float f = 1/sqrt(x_*x_ + y_*y_ + z_*z_ + w_*w_);
    return Quaternion(x_*f, y_*f, z_*f, w_*f);
  }

  //Quaternion& invert() { x_ = -x_; y_ = -y_; z = -z_; return *this; }
  const Quaternion invert() const { return Quaternion(-(vector().x()), -(vector().y()), -(vector().z()), w()); }

  Vector3f operator*(const Vector3f& v) const {
    // nVidia SDK implementation
    Vector3f uv, uuv;
    Vector3f qvec(x_, y_, z_);
    uv = qvec.cross(v);
    uuv = qvec.cross(uv);
    uv *= (2.0f * w_);
    uuv *= 2.0f;

    return v + uv + uuv;
  }

  float& x() { return x_; }
  const float x() const { return x_; }
  float& y() { return y_; }
  const float y() const { return y_; }
  float& z() { return z_; }
  const float z() const { return z_; }
  float& w() { return w_; }
  const float w() const { return w_; }

  const Vector3f vector() const { return Vector3f(x_, y_, z_); }

 private:

  float x_, y_, z_, w_;
};

inline const Quaternion operator *(const Quaternion& lhs, const Quaternion& rhs)
{
  return Quaternion( lhs.w()*rhs.x() + lhs.x()*rhs.w() + lhs.y()*rhs.z() - lhs.z()*rhs.y(),
                      lhs.w()*rhs.y() + lhs.y()*rhs.w() + lhs.z()*rhs.x() - lhs.x()*rhs.z(),
                      lhs.w()*rhs.z() + lhs.z()*rhs.w() + lhs.x()*rhs.y() - lhs.y()*rhs.x(),
                      lhs.w()*rhs.w() - lhs.x()*rhs.x() - lhs.y()*rhs.y() - lhs.z()*rhs.z());
}
/*inline const Quaternion operator *(const Quaternion& lhs, const Quaternion& rhs)
{
  return Quaternion( ((lhs.w()*rhs.vector()) + (rhs.w()*lhs.vector()) + (lhs.vector().cross(rhs.vector()))).normalize(),
                      ((lhs.w()*rhs.w()) - lhs.vector().dot(rhs.vector())));
}*/

inline std::ostream& operator <<(std::ostream& os, const Quaternion& q)
{
  return os << "q<" << q.x() << "," << q.y() << "," << q.z() << "," << q.w() << ">";
}

#endif
