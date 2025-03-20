#ifndef __audi_dense_scalar_h__
#define __audi_dense_scalar_h__

#include "ArcGeoSim/Numerics/AuDi/dense/assign.h"
#include "ArcGeoSim/Numerics/AuDi/dense/expression.h"

namespace audi {

  template<typename T, template<typename> class Allocator = allocator>
  class scalar
    : public expression<scalar<T,Allocator>>
  {
  public:

    using data_type = T;

    class range
    {
    public:
      range(std::size_t size, const T* data)
        : m_size(size)
        , m_data(data) {}
      const T* begin() const { return m_data; }
      const T* end() const { return m_data + m_size; }
    private:
      std::size_t m_size;
      const T* m_data;
    };

  public:

    explicit scalar()
      : m_capacity(0)
      , m_size(0)
      , m_value(T(0))
      , m_gradient(nullptr) {}

    explicit scalar(std::size_t size)
      : m_capacity(size)
      , m_size(size)
      , m_value(T(0))
      , m_gradient(m_allocator.allocate(m_size))
    {
      std::fill(m_gradient, m_gradient + m_size, T(0));
    }

    explicit scalar(const scalar& n)
      : m_capacity(n.m_size)
      , m_size(n.m_size)
      , m_value(n.m_value)
      , m_gradient(m_allocator.allocate(m_capacity))
    {
      std::copy(n.m_gradient, n.m_gradient + m_size, m_gradient);
    }

    ~scalar()
    {
      m_allocator.deallocate(m_gradient, m_capacity);
    }

    template<typename E>
    scalar(const expression<E>& vec)
      : m_capacity(vec.staticCast().size())
      , m_size(m_capacity)
      , m_value(T(0))
      , m_gradient(m_allocator.allocate(m_capacity))
    {
      assign(*this, vec.staticCast());
    }

  public:

    template<typename E>
    inline scalar& operator=(const expression<E>& vec)
    {
      assign(*this, vec.staticCast());
      return *this;
    }

    template<typename E>
    inline scalar& operator+=(const expression<E>& vec)
    {
      addAssign(*this, vec.staticCast());
      return *this;
    }

    template<typename E>
    inline scalar& operator-=(const expression<E>& vec)
    {
      subAssign(*this, vec.staticCast());
      return *this;
    }

    template<typename E>
    inline scalar& operator*=(const expression<E>& vec)
    {
      multAssign(*this, vec.staticCast());
      return *this;
    }

  public:

    inline std::size_t capacity() const { return m_capacity; }

    inline std::size_t size() const { return m_size; }

    inline T& value() { return m_value; }

    inline const T& value() const { return m_value; }

    inline T& gradient(std::size_t i)
    {
      return m_gradient[i];
    }

    inline const T& gradient(std::size_t i) const
    {
      return m_gradient[i];
    }

    inline range gradient() const { return range(m_size, m_gradient); }

    inline T* derivatives() const { return m_gradient; }

    inline T& operator[](std::size_t i)
    {
      return m_gradient[i];
    }

    inline const T& operator[](std::size_t i) const
    {
      return m_gradient[i];
    }

    inline scalar& operator=(T d)
    {
      std::fill(m_gradient, m_gradient + m_size, T(0));
      m_value = d;
      return *this;
    }

    inline scalar& operator=(const scalar& n)
    {
      m_value = n.m_value;
      //m_capacity = n.m_size;
      m_size = n.m_size;
      resize(m_size);
      std::copy(n.m_gradient, n.m_gradient + m_size, m_gradient);
      return *this;
    }

    void resize(std::size_t size)
    {
      if(size <= m_capacity) {
        m_size = size;
        return;
      }
      T* gradient = m_allocator.allocate(size);
      m_allocator.deallocate(m_gradient, m_capacity);
      m_gradient = gradient;
      m_size = m_capacity = size;
    }

  private:

    Allocator<T> m_allocator;

    std::size_t m_capacity;
    std::size_t m_size;

    T m_value;
    T* m_gradient;
  };

}

#endif /* __audi_dense_scalar_h__ */
