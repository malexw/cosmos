struct Variant {
  enum Type {
    TYPE_FLOAT
  };
  
  Type type_;
  
  union {
    float t_float;
  };
}
