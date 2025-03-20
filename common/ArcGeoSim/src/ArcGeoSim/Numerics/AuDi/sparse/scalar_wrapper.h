#ifndef __audi_sparse_scalar_h__
#define __audi_sparse_scalar_h__

namespace audi {

  template<class T = double>
  class scalar_wrapper
  {
  public:

    // pratique mais voir si on garde
    scalar_wrapper(const T value, int deriv_index, T deriv_value)
    : m_value(value)
    , m_size(1)
    , m_indexes(1, deriv_index)
    , m_values(1, deriv_value)
    {}

    scalar_wrapper(const T value=T(0))
    : m_value(value)
    , m_size(0)
    , m_indexes()
    , m_values()
    {}

    scalar_wrapper(const T value, const std::vector<int>& indexes)
    : m_value(value)
    , m_size(indexes.size())
    , m_indexes(indexes)
    // utiliser -1 size plutôt a voir
    , m_values()
    {}

    scalar_wrapper(const T value, const std::size_t size, const std::vector<int>& indexes)
    : m_value(value)
    , m_size(size)
    , m_indexes(indexes)
    // utiliser -1 size plutôt a voir
    , m_values()
    { assert(m_size <= m_indexes.size());}

    scalar_wrapper(const T value, const std::vector<int>& indexes, const std::vector<T>& gradient)
    : m_value(value)
    , m_size(indexes.size())
    , m_indexes(indexes)
    , m_values(gradient)
    {
      assert(m_size == m_values.size());
    }

    scalar_wrapper(const scalar_wrapper<T>& n)
    : m_value(n.m_value)
    , m_size(n.m_size)
    , m_indexes(n.m_indexes)
    , m_values(n.m_values)
    {}

    ~scalar_wrapper()
    {}

  public:

    inline std::size_t size() const { return m_size; }

    inline T& value() { return m_value; }

    inline const T& value() const { return m_value; }

    inline const std::vector<T>& gradient() const { return m_values; }

    // add operator [] sur size return gradient[indexes[i]]

    inline void setGradient(const T* data)
    {
      //assert((sizeof(data) / sizeof(T)) == m_size);
      m_values.assign(data, data + m_size);
    }

    inline const std::vector<int>& indexes() const { return m_indexes; }

  private:

    T m_value;

    std::size_t m_size;

    std::vector<int> m_indexes;

    std::vector<T> m_values;

    // a activer pour le multipoint
    // int m_stencil_size;

  };

}

#endif /* __audi_sparse_scalar_h__ */
