// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef __audi_memory_pool_h__
#define __audi_memory_pool_h__

#include <vector>
#include <memory>

namespace audi {

  template<typename T>
  class pool {
  public:

    static const int max_size = 400;

    pool() : m_memory(new T[max_size]) {}

    inline static pool<T>& get()
    {
      if(m_instance.get() == nullptr) {
        m_instance.reset(new pool<T>);
      }
      return *m_instance.get();
    }

    T* operator()() { return &(m_memory[m_current++]); }

    void reset() { m_current = 0; }

    int current() const { return m_current; }

  private:

    static std::unique_ptr<pool<T>> m_instance;

    T* m_memory;
    int m_current = 0;
  };

  template<typename T>
  std::unique_ptr<pool<T>> pool<T>::m_instance;

//
//  Allocator* Allocator::m_pool = nullptr;
//
//
//    //make empty node
//    inline T* make_node() {
//      T *ret = ((pools.back()) + next_pos);
//      ++next_pos;
//      return ret;
//    }
//
//    //make an initialised node
//    inline T* make_node(double aa ,const audi::scalar<double>* ss , audi::node<double>* nes , audi::node<double>* nex  ) {
//      T *ret = ((pools.back()) + next_pos);
//      ret->coef= aa ;
//      ret->s= ss;
//      ret->nested = nes;
//      ret->next = nex ;
//      ++next_pos;
//      return ret;
//    }
//
//    static Allocator* get(){
//      if (m_pool == nullptr) {
//        m_pool = new pool::Allocator();
//      }
//      return m_pool;
//    }
//
//    //appel du destructeur
//    inline static void kill () {
//      if (NULL != m_pool)
//      {
//        delete m_pool;
//        m_pool = NULL;
//      }
//    }
//
//    //reset the cursor
//    inline void clear () {
//      next_pos = 0;
//    }
//
//  private:
//
//    static Allocator* m_pool;
//
//    // constructeur
//    Allocator() {
//      next_pos = 0;
//      void *temp = operator new(pool_size);
//      pools.push_back(static_cast<T *>(temp));
//    }
//
//    //distructeur
//    ~Allocator() {
//      std::cout << "/* finn */" << '\n';
//      while (!pools.empty()) {
//            T *p = pools.back();
//        for (size_t pos = max_size; pos > 0; --pos)
//        p[pos - 1].~T();
//        operator delete(static_cast<void *>(p));
//        pools.pop_back();
//      }
//    }
//
//    //argument
//    const size_t pool_size = max_size * sizeof(T);
//    std::vector<T*> pools;
//    size_t next_pos;
//  };
//
//  Allocator* Allocator::m_pool = nullptr;

}

#endif /* __audi_memory_pool_h__ */
