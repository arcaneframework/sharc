// -*- C++ -*-
#ifndef __audi_memory_allocator_h__
#define __audi_memory_allocator_h__

#include <memory>
#include <stack>
#include <vector>

namespace audi {

  template <class T>
  class allocator
  {
  private:

    struct stack {

      inline std::vector<T>& retain()
      {
        auto stack_size = m_stack.size();
        if (stack_size > 0) {
          auto* a = m_stack.top();
          m_stack.pop();
          return *a;
        } else {
          return *(new std::vector<T>());
        }
      }

      inline void release(std::vector<T>& buffer)
      {
        m_stack.push(&buffer);
      }

      ~stack()
      {
        while(!m_stack.empty())
        {
          delete m_stack.top();
          m_stack.pop();
        }
      }

       std::stack<std::vector<T>*> m_stack;
    };

    static std::unique_ptr<stack> m_stack;

    inline static stack& get()
    {
      if(m_stack.get() == nullptr) {
        m_stack.reset(new stack);
      }
      return *m_stack.get();
    }

  public:

    typedef T value_type;

	  inline  allocator()
      : m_memory(get().retain()) {}

    inline ~allocator()
    {
      get().release(m_memory);
    }

    template <class U>
    inline allocator(const allocator<U>&)
      : m_memory(get().retain()) {}

    inline T *allocate(::std::size_t n) {
      m_memory.resize(n);
      return m_memory.data();
    }

    inline void deallocate (T* p, std::size_t n) {}

  private:

    std::vector<T>& m_memory;
  };


  template <class T, class U>
  inline /* constexpr */ bool operator== (const allocator<T>&, const allocator<U>&)
  {
    return std::is_same<T,U>::value;
  }

  template <class T, class U>
  inline /* constexpr */ bool operator!= (const allocator<T>&, const allocator<U>&)
  {
    return not std::is_same<T,U>::value;
  }

  template<typename T>
  std::unique_ptr<typename allocator<T>::stack> allocator<T>::m_stack;
}

#endif /* __audi_memory_allocator_h__ */
