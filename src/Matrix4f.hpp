#ifndef COSMOS_MATRIX4F_H_
#define COSMOS_MATRIX4F_H_

#include <iostream>
#include <math.h>

#include <boost/shared_ptr.hpp>

#include "Quaternion.hpp"
#include "Vector3f.hpp"

//#include "util.hpp"

// Basic object for representing a vector in 3 dimensions
class Matrix4f {
 public:
  typedef boost::shared_ptr<Matrix4f> ShPtr;

  static const Matrix4f IDENTITY;
  static const Matrix4f ZEROS;

  Matrix4f() {
    m_[0]=1.0f; m_[1]=0.0f; m_[2]=0.0f; m_[3]=0.0f;
    m_[4]=0.0f; m_[5]=1.0f; m_[6]=0.0f; m_[7]=0.0f;
    m_[8]=0.0f; m_[9]=0.0f; m_[10]=1.0f; m_[11]=0.0f;
    m_[12]=0.0f; m_[13]=0.0f; m_[14]=0.0f; m_[15]=1.0f;
  }

  Matrix4f(const float (&rhs)[16]) {
    for(int i = 15; i >= 0; --i) {
      m_[i] = rhs[i];
    }
  }

  Matrix4f( float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33) {
    m_[0]=m00; m_[1]=m01; m_[2]=m02; m_[3]=m03;
    m_[4]=m10; m_[5]=m11; m_[6]=m12; m_[7]=m13;
    m_[8]=m20; m_[9]=m21; m_[10]=m22; m_[11]=m23;
    m_[12]=m30; m_[13]=m31; m_[14]=m32; m_[15]=m33;
  }

  Matrix4f(const Matrix4f& rhs) {
    m_[0]=rhs[0]; m_[1]=rhs[1]; m_[2]=rhs[2]; m_[3]=rhs[3];
    m_[4]=rhs[4]; m_[5]=rhs[5]; m_[6]=rhs[6]; m_[7]=rhs[7];
    m_[8]=rhs[8]; m_[9]=rhs[9]; m_[10]=rhs[10]; m_[11]=rhs[11];
    m_[12]=rhs[12]; m_[13]=rhs[13]; m_[14]=rhs[14]; m_[15]=rhs[15];
  }

  Matrix4f& operator =(const Matrix4f& rhs) {
    // Well, I guess we *could* do it in a loop
    m_[0]=rhs[0]; m_[1]=rhs[1]; m_[2]=rhs[2]; m_[3]=rhs[3];
    m_[4]=rhs[4]; m_[5]=rhs[5]; m_[6]=rhs[6]; m_[7]=rhs[7];
    m_[8]=rhs[8]; m_[9]=rhs[9]; m_[10]=rhs[10]; m_[11]=rhs[11];
    m_[12]=rhs[12]; m_[13]=rhs[13]; m_[14]=rhs[14]; m_[15]=rhs[15];
    return *this;
  }

  bool operator==(const Matrix4f& rhs) {
    float delta = 0.00001;
    for (int i = 15; i >= 0; --i) {
      float diff = m_[i] - rhs[i];
      if (diff > delta || -diff > delta) {
        return false;
      }
    }
    return true;
  }

  // HACK
  Vector3f operator*(const Vector3f& rhs) {
    float nx = (m_[0] * rhs.x()) + (m_[4] * rhs.y()) + (m_[8] * rhs.z()) + m_[12];
    float ny = (m_[1] * rhs.x()) + (m_[5] * rhs.y()) + (m_[9] * rhs.z()) + m_[13];
    float nz = (m_[2] * rhs.x()) + (m_[6] * rhs.y()) + (m_[10] * rhs.z()) + m_[14];
    return Vector3f(nx, ny, nz);
  }

  float& operator[](int i) { return m_[i]; }
  const float operator[](int i) const { return m_[i]; }
  float* to_array() { return m_; }
  const float* const to_array() const { return m_; }

  Matrix4f transpose() const {
    return Matrix4f(m_[0], m_[4], m_[8],  m_[12],
                    m_[1], m_[5], m_[9],  m_[13],
                    m_[2], m_[6], m_[10], m_[14],
                    m_[3], m_[7], m_[11], m_[15]);
  }

  Matrix4f invert() const {
    // Algorithm from Ogre's OgreMatrix4 class
    float m00 = m_[0], m01 = m_[1], m02 = m_[2], m03 = m_[3];
    float m10 = m_[4], m11 = m_[5], m12 = m_[6], m13 = m_[7];
    float m20 = m_[8], m21 = m_[9], m22 = m_[10], m23 = m_[11];
    float m30 = m_[12], m31 = m_[13], m32 = m_[14], m33 = m_[15];

    float v0 = m20 * m31 - m21 * m30;
    float v1 = m20 * m32 - m22 * m30;
    float v2 = m20 * m33 - m23 * m30;
    float v3 = m21 * m32 - m22 * m31;
    float v4 = m21 * m33 - m23 * m31;
    float v5 = m22 * m33 - m23 * m32;

    float t00 = + (v5 * m11 - v4 * m12 + v3 * m13);
    float t10 = - (v5 * m10 - v2 * m12 + v1 * m13);
    float t20 = + (v4 * m10 - v2 * m11 + v0 * m13);
    float t30 = - (v3 * m10 - v1 * m11 + v0 * m12);

    float invDet = 1 / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

    float d00 = t00 * invDet;
    float d10 = t10 * invDet;
    float d20 = t20 * invDet;
    float d30 = t30 * invDet;

    float d01 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
    float d11 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
    float d21 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
    float d31 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

    v0 = m10 * m31 - m11 * m30;
    v1 = m10 * m32 - m12 * m30;
    v2 = m10 * m33 - m13 * m30;
    v3 = m11 * m32 - m12 * m31;
    v4 = m11 * m33 - m13 * m31;
    v5 = m12 * m33 - m13 * m32;

    float d02 = + (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
    float d12 = - (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
    float d22 = + (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
    float d32 = - (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

    v0 = m21 * m10 - m20 * m11;
    v1 = m22 * m10 - m20 * m12;
    v2 = m23 * m10 - m20 * m13;
    v3 = m22 * m11 - m21 * m12;
    v4 = m23 * m11 - m21 * m13;
    v5 = m23 * m12 - m22 * m13;

    float d03 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
    float d13 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
    float d23 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
    float d33 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

    return Matrix4f(d00, d01, d02, d03,
                    d10, d11, d12, d13,
                    d20, d21, d22, d23,
                    d30, d31, d32, d33);
  }

  static Matrix4f projectionPerspectiveMatrix(float fov, float aspect_ratio, float clip_near, float clip_far) {

    float top = tan(fov * (M_PI / 360)) * clip_near;
    float bottom = -top;
    float left = aspect_ratio * bottom;
    float right = aspect_ratio * top;

    float width = right - left;
    float w = 2 * clip_near / width;

    float height = top - bottom;
    float h = 2 * clip_near / height;

    float depth = clip_far - clip_near;
    float q = -1 * (clip_far + clip_near) / depth;
    float qn = -2 * (clip_far * clip_near) / depth;

    return Matrix4f(w, 0, 0,  0,
                    0, h, 0,  0,
                    0, 0, q, -1,
                    0, 0, qn, 0);

  }

  static Matrix4f projectionOrthoMatrix(float left, float right, float bottom, float top, float near, float far) {

    float width = right - left;
    float height = top - bottom;
    float depth = far - near;
    float x = -(right + left) / width;
    float y = -(top + bottom) / height;
    float z = -(far + near) / depth;

    return Matrix4f(2.0f/width, 0.0f,        0.0f,        0.0f,
                    0.0f,       2.0f/height, 0.0f,        0.0f,
                    0.0f,       0.0f,        -2.0f/depth, 0.0f,
                    x,          y,           z,           1.0f);
  }

  static Matrix4f viewFromPositionDirection(const Vector3f& position, const Vector3f& direction) {

    Vector3f ndir = -(direction.normalize());
    Vector3f vx = Vector3f::UNIT_Y.cross(ndir).normalize();
    Vector3f vy = ndir.cross(vx).normalize();
    Quaternion q(vx, vy, ndir);

    Vector3f iT = -position;
    Quaternion iR = q.invert();

    iT = iR * iT; // rotate

    float x = iR.x(); float y = iR.y(); float z = iR.z(); float w = iR.w();
    float dx = x+x; float dy = y+y; float dz = z+z;
    float xy = x*dy; float xz = x*dz; float xw = dx*w; float yz = y*dz; float yw = dy*w; float zw = dz*w;
    float x2 = dx*x; float y2 = dy*y; float z2 = dz*z;

    return Matrix4f( 1.0f-(y2+z2), (xy+zw),      (xz-yw),      0.0f,
                    (xy-zw),       1.0f-(x2+z2), (yz+xw),      0.0f,
                    (xz+yw),       (yz-xw),      1.0f-(x2+y2), 0.0f,
                    iT.x(),        iT.y(),       iT.z(),       1.0f);
  }

  static Matrix4f modelFromSqt(const Vector3f& scale, const Quaternion& quaternion, const Vector3f& translation) {
    float x = quaternion.x(); float y = quaternion.y(); float z = quaternion.z(); float w = quaternion.w();
    float dx = x+x; float dy = y+y; float dz = z+z;
    float xy = x*dy; float xz = x*dz; float xw = dx*w; float yz = y*dz; float yw = dy*w; float zw = dz*w;
    float x2 = dx*x; float y2 = dy*y; float z2 = dz*z;

    return Matrix4f((1.0f-(y2+z2))*scale.x(), (xy+zw)*scale.x(),        (xz-yw)*scale.x(),        0.0f,
                    (xy-zw)*scale.y(),        (1.0f-(x2+z2))*scale.y(), (yz+xw)*scale.y(),        0.0f,
                    (xz+yw)*scale.z(),        (yz-xw)*scale.z(),        (1.0f-(x2+y2))*scale.z(), 0.0f,
                    translation.x(),          translation.y(),          translation.z(),          1.0f);
  }

 private:

  float m_[16];
};

inline Matrix4f operator*(const Matrix4f& lhs, const Matrix4f& rhs)
{
  Matrix4f ret;

  for (int i = 0; i < 15; i += 4) {
    for (int j = 0; j < 4; ++j) {
      ret[j+i] = lhs[i]*rhs[j] + lhs[i+1]*rhs[j+4] + lhs[i+2]*rhs[j+8] + lhs[i+3]*rhs[j+12];
    }
  }

  return ret;
}

inline std::ostream& operator <<(std::ostream& os, const Matrix4f& m)
{
  return os << "m[" << m[0] << "," << m[1] << "," << m[2] << "," << m[3] << "]" << std::endl
            << " [" << m[4] << "," << m[5] << "," << m[6] << "," << m[7] << "]" << std::endl
            << " [" << m[8] << "," << m[9] << "," << m[10] << "," << m[11] << "]" << std::endl
            << " [" << m[12] << "," << m[13] << "," << m[14] << "," << m[15] << "]";
}

#endif
