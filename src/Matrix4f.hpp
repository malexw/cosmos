#ifndef COSMOS_MATRIX4F_H_
#define COSMOS_MATRIX4F_H_

#include <iostream>
//#include <math.h>

#include <memory>

//#include "util.hpp"

// Basic object for representing a vector in 3 dimensions
class Matrix4f {
 public: 
	typedef std::shared_ptr<Matrix4f> ShPtr;

  Matrix4f() {
    m_[0]=1.0f; m_[1]=0.0f; m_[2]=0.0f; m_[3]=0.0f;
    m_[4]=0.0f; m_[5]=1.0f; m_[6]=0.0f; m_[7]=0.0f;
    m_[8]=0.0f; m_[9]=0.0f; m_[10]=1.0f; m_[11]=0.0f;
    m_[12]=0.0f; m_[13]=0.0f; m_[14]=0.0f; m_[15]=1.0f;
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
  
  float& operator[](int i) { return m_[i]; }
  const float operator[](int i) const { return m_[i]; }
  float* to_array() { return m_; }
  
  Matrix4f transpose() const {
    return Matrix4f(m_[0], m_[4], m_[8], m_[12],
                    m_[1], m_[5], m_[9], m_[13],
                    m_[2], m_[6], m_[10], m_[14],
                    m_[3], m_[7], m_[11], m_[15]);
  }
  // Lazy implementation - we'll build it when we need it
  //Matrix4f invert() const;
  
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
