#pragma once
namespace avos {
namespace perception {
namespace xlidarMOT {  
class Numerical
{
public:
Numerical(){}
~Numerical(){}

template <typename T>
inline bool Near(T value, T tar, float tolerance){
  if(value==tar){
    return true;
  }
  if(tolerance<0){
    tolerance=-tolerance;
  }
  return (value>=tar-tolerance && value<=tar+tolerance);
}

template <typename T>
inline bool Near(T value, T tar){
  return Near<T>(value, tar, 0.000001);
}

template <typename T>
T Clamp(const T value, T bound1, T bound2) {
  if (bound1 > bound2) {
    std::swap(bound1, bound2);
  }

  if (value < bound1) {
    return bound1;
  } else if (value > bound2) {
    return bound2;
  }
  return value;
}

};

}
}
}

