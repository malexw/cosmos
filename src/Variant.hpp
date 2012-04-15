struct Variant {
  enum Type {
    TYPE_INT,
    TYPE_UINT,
    TYPE_FLOAT
  };
  
  Type type;
  
  union {
    int int_t;
    unsigned int uint_t;
    float float_t;
  };
};
