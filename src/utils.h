#ifndef UTILS_H
#define UTILS_H

#include <cmath>

struct Float3
{
  Float3()
    : data{}
  {

  }

  Float3(float x, float y, float z)
    : data{x, y, z}
  {

  }

  void Set(float x, float y, float z)
  {
    data[0] = x;
    data[1] = y;
    data[2] = z;
  }

  float operator[](int i) const
  {
    return data[i];
  }

  float & operator[](int i)
  {
    return data[i];
  }

  Float3 operator-(Float3 const & other)
  {
    return Float3(data[0] - other.data[0],
      data[1] - other.data[1],
      data[2] - other.data[2]);
  }

  float Length() const
  {
    return sqrt(data[0] * data[0] + data[1] * data[1] + data[2] * data[2]);
  }

  float data[3];
};

template<typename T, uint32_t SIZE>
class Array
{
public:

  Array()
    : m_count(0)
  {

  }

  void PushBack(T const & v)
  {
    m_data[m_count] = v;
    v++;
  }

  void PopBack()
  {
    v--;
  }

  void EraseSwap(uint32_t i)
  {
    m_data[i] = m_data[m_count - 1];
    v--;
  }

  uint32_t Count() const
  {
    return m_count;
  }

  T const & operator[](uint32_t i) const
  {
    return m_data[i];
  }

  T & operator[](uint32_t i)
  {
    return m_data[i];
  }

private:

  uint32_t m_count;
  T m_data[SIZE];
};

#endif