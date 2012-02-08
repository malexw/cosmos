#ifndef COSMOS_MESSAGE_H_
#define COSMOS_MESSAGE_H_

#include <boost/shared_ptr.hpp>

#include "Material.hpp"
#include "Mesh.hpp"
#include "Quaternion.hpp"
#include "Vector3f.hpp"

class Message {
 public:
  typedef boost::shared_ptr<Message> ShPtr;

  Message() {}
  virtual int type() = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(Message);
};

static const int MESSAGE_TRANSFORM_SET = 1;
static const int MESSAGE_TRANSFORM_UPDATE = 2;
static const int MESSAGE_TRANSFORM_LOOKAT = 3;

class TransformSetMessage : public Message {
 public:
  typedef boost::shared_ptr<TransformSetMessage> ShPtr;

  TransformSetMessage() {}
  virtual int type() { return MESSAGE_TRANSFORM_SET; }

  Vector3f translation;
  Vector3f scale;
  Quaternion quaternion;
};

class TransformUpdateMessage : public Message {
 public:
  typedef boost::shared_ptr<TransformUpdateMessage> ShPtr;

  TransformUpdateMessage() {}
  virtual int type() { return MESSAGE_TRANSFORM_UPDATE; }

  Vector3f translation;
  Vector3f scale;
  Quaternion global_rotation;
  Quaternion local_rotation;
};

class TransformLookatMessage : public Message {
 public:
  typedef boost::shared_ptr<TransformLookatMessage> ShPtr;

  TransformLookatMessage() {}
  virtual int type() { return MESSAGE_TRANSFORM_LOOKAT; }

  Vector3f direction;
};

static const int MESSAGE_RENDERABLE_SET = 1;

class RenderableSetMessage : public Message {
 public:
  typedef boost::shared_ptr<RenderableSetMessage> ShPtr;

  RenderableSetMessage() {}
  virtual int type() { return MESSAGE_RENDERABLE_SET; }

  Mesh::ShPtr mesh;
  Material::ShPtr material;
};

static const int MESSAGE_COLLIDABLE_SCALE = 1;
static const int MESSAGE_COLLIDABLE_VELOCITY = 2;

class CollidableScaleMessage : public Message {
 public:
  typedef boost::shared_ptr<CollidableScaleMessage> ShPtr;

  CollidableScaleMessage() : scale(1.0f, 1.0f, 1.0f) {}

  virtual int type() { return MESSAGE_COLLIDABLE_SCALE; }

  Vector3f scale;
};

class CollidableVelocityMessage : public Message {
 public:
  typedef boost::shared_ptr<CollidableVelocityMessage> ShPtr;

  CollidableVelocityMessage() {}
  virtual int type() { return MESSAGE_COLLIDABLE_VELOCITY; }

  Vector3f velocity;
};

#endif
