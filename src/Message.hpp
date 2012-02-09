#ifndef COSMOS_MESSAGE_H_
#define COSMOS_MESSAGE_H_

#include <map>
#include <string>

#include <boost/shared_ptr.hpp>

#include "Variant.hpp"

class Message {
 public:
  typedef boost::shared_ptr<Message> ShPtr;
  typedef std::map<std::string, Variant> ArgList;
  typedef std::map<std::string, std::string> ArgStringList;

  enum MessageType {
    TRANSFORM_SET,
    TRANSFORM_UPDATE,
    TRANSFORM_LOOKAT,

    RENDERABLE_SET,

    COLLIDABLE_SCALE,
    COLLIDABLE_VELOCITY
  };

  const MessageType type_;

  explicit Message(MessageType mt) : type_(mt) {}

  Message& add_arg(std::string ref, int val) {
    Variant nv;
    nv.type = Variant::TYPE_INT;
    nv.int_t = val;
    args_.insert(ArgList::value_type(ref, nv));
    return *this;
  }

  Message& add_arg(std::string ref, unsigned int val) {
    Variant nv;
    nv.type = Variant::TYPE_UINT;
    nv.uint_t = val;
    args_.insert(ArgList::value_type(ref, nv));
    return *this;
  }

  Message& add_arg(std::string ref, float val) {
    Variant nv;
    nv.type = Variant::TYPE_FLOAT;
    nv.float_t = val;
    args_.insert(ArgList::value_type(ref, nv));
    return *this;
  }
  
  Message& add_arg(std::string ref, std::string val) {
    arg_strs_.insert(ArgStringList::value_type(ref, val));
    return *this;
  }

  int get_int(std::string ref) {
    ArgList::iterator i = args_.find(ref);
    if (i != args_.end()) {
      return i->second.int_t;
    } else {
      return 0;
    }
  }
  
  unsigned int get_uint(std::string ref) {
    ArgList::iterator i = args_.find(ref);
    if (i != args_.end()) {
      return i->second.uint_t;
    } else {
      return 0;
    }
  }
  
  float get_float(std::string ref) {
    ArgList::iterator i = args_.find(ref);
    if (i != args_.end()) {
      return i->second.float_t;
    } else {
      return 0;
    }
  }
  
  std::string get_string(std::string ref) {
    ArgStringList::iterator i = arg_strs_.find(ref);
    if (i != arg_strs_.end()) {
      return i->second;
    } else {
      return std::string();
    }
  }

  bool has_key(std::string ref) {
    bool ret = args_.count(ref) >= 1;
    
    if (!ret) {
      ret = arg_strs_.count(ref) >= 1;
    }
    
    return ret;
  }

 private:
  ArgList args_;
  ArgStringList arg_strs_;

  DISALLOW_COPY_AND_ASSIGN(Message);
};

#endif
