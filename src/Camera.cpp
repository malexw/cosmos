#include "Camera.hpp"

#include <glm/glm.hpp>

glm::mat4 Camera::matrixFromPositionDirection(Vector3f position, Vector3f direction) {
  Vector3f ndir = -(direction.normalize());
  Vector3f vx = Vector3f::UNIT_Y.cross(ndir).normalize();
  Vector3f vy = ndir.cross(vx).normalize();
  Quaternion q(vx, vy, ndir);

  Vector3f iT = -position;
  Quaternion iR = q.invert();

  iT = iR * iT; // rotate

  float x = iR.x(), y = iR.y(), z = iR.z(), w = iR.w();
  float dx = x+x, dy = y+y, dz = z+z;
  float xy = x*dy, xz = x*dz, xw = dx*w, yz = y*dz, yw = dy*w, zw = dz*w;
  float x2 = dx*x, y2 = dy*y, z2 = dz*z;

  // Column-major (GLM default)
  return glm::mat4(
    1.0f-(y2+z2), (xy+zw),      (xz-yw),      0.0f,
    (xy-zw),      1.0f-(x2+z2), (yz+xw),      0.0f,
    (xz+yw),      (yz-xw),      1.0f-(x2+y2), 0.0f,
    iT.x(),       iT.y(),       iT.z(),        1.0f
  );
}
