#ifndef __audi_dense_expression_h__
#define __audi_dense_expression_h__

namespace audi {

  template<typename E>
  struct expression
  {
    inline const E& staticCast() const
    {
      return static_cast<const E&>(*this);
    }

    inline E& staticCast()
    {
      return static_cast<E&>(*this);
    }
  };

}

#endif /* __audi_dense_expression_h__ */
