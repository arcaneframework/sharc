#ifndef __audi_sparse_expression_h__
#define __audi_sparse_expression_h__

#include "ArcGeoSim/Numerics/AuDi/memory/pool.h"
#include "ArcGeoSim/Numerics/AuDi/sparse/scalar_wrapper.h"

namespace audi{

  namespace details {

    template<typename T>
    struct node {
      T coef = 1.;
      //int stencil_size = 0;
      int size = 0;
      const T* gradient = nullptr;
      const int* indexes = nullptr;
      const node<T> *nested = nullptr;
      const node<T> *next = nullptr;
    };

  }

  template<typename T>
  class sparse_expression: public std::pair<T, details::node<T>*>
  {
    public:

    sparse_expression()
      : std::pair<T, details::node<T>*>(),
        m_index_redir(),
        m_is_unpack(false)
      {};

    sparse_expression(T arg1, details::node<T>* arg2)
      : std::pair<T, details::node<T>*>(arg1, arg2),
        m_index_redir(),
        m_is_unpack(false)
     {};

    sparse_expression(const scalar_wrapper<T>& s)
    {
      this->first = s.value();
      // dangereux a revoir
      using node_type = details::node<T>;
      auto& node_pool = pool<node_type>::get();
      auto* n = node_pool();
      {
        n->coef = T(1);
        n->size = s.size();
        n->gradient = s.gradient().data();
        n->indexes = s.indexes().data();
        n->nested = nullptr;
        n->next = nullptr;
      }
      this->second = n;
      this->m_index_redir = {};
      this->m_is_unpack = false;
    };

    sparse_expression(T val)
    {
      this->first = val;
      // dangereux a revoir
      using node_type = details::node<T>;
      auto& node_pool = pool<node_type>::get();
      auto* n = node_pool();
      {
    	n->coef = T(1);
    	n->size = 0;
    	n->gradient = nullptr;
        n->indexes = nullptr;
        n->nested = nullptr;
        n->next = nullptr;
      }
      this->second = n;
      this->m_index_redir = {};
      this->m_is_unpack = false;
    };

    //virtual ~sparse_expression(){};
    inline const T& value() const { return this->first;};
    inline T& value() { return this->first; };
    inline std::size_t size() const { return this->second->size; };
    inline bool is_unpack() const { return m_is_unpack; };
    inline void is_unpack(bool is_unpack) const { m_is_unpack = is_unpack; };
    inline const std::vector<int>& index_redir() const { return m_index_redir; };
    inline void index_redir(const std::vector<int>& redir) { m_index_redir = redir; };

    private:

    std::vector<int> m_index_redir;
    mutable bool m_is_unpack = false;

  };


}

#endif /* __audi_sparse_expression_h__ */
