#ifndef UTILS_H
#define UTILS_H

#include <cmath>

template<typename T>
struct Vector3
{
  Vector3()
    : data{}
  {

  }

  Vector3(T x, T y, T z)
    : data{x, y, z}
  {

  }

  void Set(T x, T y, T z)
  {
    data[0] = x;
    data[1] = y;
    data[2] = z;
  }

  T operator[](int i) const
  {
    return data[i];
  }

  T & operator[](int i)
  {
    return data[i];
  }

  Vector3 operator-(Vector3 const & other) const
  {
    return Vector3(data[0] - other.data[0],
                   data[1] - other.data[1],
                   data[2] - other.data[2]);
  }

  T Length() const
  {
    return sqrt(data[0] * data[0] + data[1] * data[1] + data[2] * data[2]);
  }

  T data[3];
};

typedef Vector3<float> Float3;

#endif