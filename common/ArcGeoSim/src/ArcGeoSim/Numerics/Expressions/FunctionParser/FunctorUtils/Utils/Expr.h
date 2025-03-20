#ifndef EXPR_H
#define EXPR_H

#include "../../rdtsc.h"

/*!
 * \file Expr.h
 * \brief Manipulation des expressions 
 * \date 01/04/2008
 * \author Xavier TUNC
 * 
 * Definition de trois types generiques pour encapsuler toutes les operations. \n
 * La classe Expr est la classe "normale", elle reference les operandes ainsi que les operations. \n
 * La classe ExprG sert dans le cas ou l'operande de gauche est un scalaire : elle copie l'operande de gauche et reference l'operande de droite. \n
 * La classe ExprD est le pendant de la classe ExprG mais a droite. Elle copie l'operande de droite, cette derniere etant un scalaire, et reference l'operande de gauche. \n
 */

#define maxx(A,B) ((A)>(B)?(A):(B))
#define minn(A,B) ((A)<(B)?(A):(B))

template <typename xpr1, typename xpr2, typename xpr3>
class Expr ;

template <typename T>
class IExpr
{
public:
  typedef T value_type ;

  virtual inline value_type getValue(int ) const
  {

  }

  virtual inline void getValue(int, T* ) const
  {

  }

  virtual inline int getSize() const
  {
    return -120 ;
  }

  virtual inline bool isContainer() const
  {
    return true ;
  }
};

/*!
 * \brief Expr : Manipulation des expressions
 * 
 * Cette classe reference les operandes de droite et gauche. \n
 * Elle dispose d'une methode getSize() pour recuperer la taille de l'expression et d'une methode getValue() pour recuperer la valeur de l'expression a l'index i. \n
 * L'operateur n'est pas reference mais est stocke via le template. Il est utilise lors de l'appel a la methode getValue() en faisant appel a la methode getValue() de l'operateur. \n
 * Cette classe est utilise en interne, il n'est pas conseille de l'utiliser directement. \n
 */

template <typename L, typename OP, typename R>
class Expr
{
private :
  //! \brief Reference sur l'operande de gauche.
  const L & m_left ;
  //! \brief Reference sur l'operande de droite.
  const R & m_right ;

public :
  //! \brief Type du resultat de l'expression
  typedef typename OP::value_type value_type ;

  //! \brief Type de l'expression
  typedef Expr<L,OP,R> expr_type ;

  //! @name Constructeur
  //! @{
  /*! \brief Construit l'expression en referencant les operandes
   * 
   * \param left operande de gauche
   * \param right operande de droite
   */
  inline explicit Expr(const L & left, const R & right)
  : m_left(left), m_right(right)
  {
    ;
  }
  //! @}

  //! @name Destructeur
  //! @{

  /*! \brief Destructeur.
   */
  inline ~Expr()
  {
    ;
  }
  //! @}

  //! @name Accesseurs
  //! @{
  /*! \brief Retourne la taille de l'expression
   * 
   * Verifie si les operandes de gauche et de droite ont la meme taille, ou si l'une des deux est une constante. \n
   * Renvoie la taille de l'expression ou exit(1) si les operandes n'ont pas la meme taille. \n
   */
  inline int getSize() const 
  {
    int leftSize = m_left.getSize() ;
    int rightSize = m_right.getSize() ;
    if (leftSize == rightSize || leftSize == -1 || rightSize == -1 || rightSize == 1 || leftSize == 1)
    {
      return maxx(leftSize,rightSize) ;
    }
    else
    {
      std::cout << "LHS & RHS de taille differentes : exit(1)" << std::endl ;
      exit(1) ;
    }
  }

  /*! \brief Retourne le nombre d'inconnues de l'expression
   */
  inline int getNbInconnues() const 
  {
    return maxx(m_left.getNbInconnues(),m_right.getNbInconnues()) ;
  }
  /*! \brief Retourne la valeur de l'expression a l'index i
   * 
   * Fait appel a la methode getValue() de l'operande. \n 
   * \param i Index de la valeur recherchee
   * \warning Ne verifie pas si i valable pour l'expression
   */
  inline value_type getValue(int i) const
  {
    return OP::getValue(m_left, m_right,i) ;
  }

  /*! \brief Retourne la valeur de la derivee de l'expression a l'index ligne,colonne
   * 
   * Fait appel a la methode getDerivedValue() de l'operande. \n 
   * \param ligne Premier index
   * \param colonne Deuxieme index
   * \warning Ne verifie pas si ligne et colonne sont valables pour l'expression
   */
  inline value_type getDerivedValue(int ligne, int colonne) const
  {
    return OP::getDerivedValue(m_left, m_right, ligne,colonne) ;
  }

  inline bool isContainer() const
  {
    return true ;
  }

  //! @}

  //! Retourne la valeur de x
  inline double getX() const
  {
    return OP::getX(m_left,m_right) ;
  }

  //! Retourne la valeur de y
  inline double getY() const
  {
    return OP::getY(m_left,m_right) ;
  }

  //! Retourne la valeur de z
  inline double getZ() const
  {
    return OP::getZ(m_left,m_right) ;
  }

};
/*!
 * \brief ExprG : Manipulation des expressions quand l'operande de gauche est un scalaire
 * 
 * Cette classe reference l'operande de droite et copie l'operande de gauche. \n
 * Elle dispose d'une methode getSize() pour recuperer la taille de l'expression et d'une methode getValue() pour recuperer la valeur de l'expression a l'index i. \n
 * L'operateur n'est pas reference mais est stocke via le template. Il est utilise lors de l'appel a la methode getValue() en faisant appel a la methode getValue() de l'operateur. \n
 * Cette classe est utilise en interne, il n'est pas conseille de l'utiliser directement. \n
 */

template <typename L, typename OP, typename R>
class ExprG
{
private :

  //! \brief Operande de gauche.
  const L m_left ;
  //! \brief Reference sur l'operande de droite.
  const R & m_right ;

public :

  //! \brief Type du resultat de l'expression
  typedef typename OP::value_type value_type ;

  //! \brief Type de l'expression
  typedef Expr<L,OP,R> expr_type ;

  //! @name Constructeurs
  //! @{
  /*! \brief Construit l'expression en referencant l'operande de droite et en copiant l'operande de gauche
   * 
   * \param left operande de gauche
   * \param right operande de droite
   */
  inline explicit ExprG(const L & left, const R & right)
  : m_left(left), m_right(right)
  {
    ;
  }

  //! @}

  //! @name Destructeur
  //! @{
  /*! \brief Destructeur.
   */
  inline ~ExprG()
  {
    ;
  }
  //! @}

  //! @name Accesseurs
  //! @{
  /*! \brief Retourne la taille de l'expression
   * Verifie si les operandes de gauche et de droite ont la meme taille, ou si l'une des deux est une constante. \n
   * Renvoie la taille de l'expression ou exit(1) si les operandes n'ont pas la meme taille. \n
   */
  inline int getSize() const 
  {
    int leftSize = m_left.getSize() ;
    int rightSize = m_right.getSize() ;
    if (leftSize == rightSize || leftSize == -1 || rightSize == -1 || rightSize == 1 || leftSize == 1)
    {
      return maxx(leftSize,rightSize) ;
    }
    else
    {
      std::cout << "LHS & RHS de taille differentes : exit(1)" << std::endl ;
      exit(1) ;
    }
  }

  /*! \brief Retourne le nombre d'inconnues de l'expression
   */
  inline int getNbInconnues() const 
  {
    return maxx(m_left.getNbInconnues(),m_right.getNbInconnues()) ;
  }

  /*! \brief Retourne la valeur de l'expression a l'index i
   * 
   * Fait appel a la methode getValue() de l'operande. \n 
   * \param i Index de la valeur recherchee
   * \warning Ne verifie pas si i valable pour l'expression
   */
  inline value_type getValue(int i) const
  {
    return OP::getValue(m_left, m_right,i) ;
  }

  /*! \brief Retourne la valeur de la derivee de l'expression a l'index ligne,colonne
   * 
   * Fait appel a la methode getDerivedValue() de l'operande. \n 
   * \param ligne Premier index
   * \param colonne Deuxieme index
   * \warning Ne verifie pas si ligne et colonne sont valables pour l'expression
   */
  inline value_type getDerivedValue(int ligne, int colonne) const
  {
    return OP::getDerivedValue(m_left, m_right, ligne,colonne) ;
  }

  inline bool isContainer() const
  {
    return true ;
  }
  //! @}

  //! Retourne la valeur de x
  inline double getX() const
  {
    return OP::getX(m_left,m_right) ;
  }

  //! Retourne la valeur de y
  inline double getY() const
  {
    return OP::getY(m_left,m_right) ;
  }

  //! Retourne la valeur de z
  inline double getZ() const
  {
    return OP::getZ(m_left,m_right) ;
  }
};

/*!
 * \brief ExprD : Manipulation des expressions quand l'operande de droite est un scalaire
 * 
 * Cette classe reference l'operande de gauche et copie l'operande de droite. \n
 * Elle dispose d'une methode getSize() pour recuperer la taille de l'expression et d'une methode getValue() pour recuperer la valeur de l'expression a l'index i. \n
 * L'operateur n'est pas reference mais est stocke via le template. Il est utilise lors de l'appel a la methode getValue() en faisant appel a la methode getValue() de l'operateur. \n
 * Cette classe est utilise en interne, il n'est pas conseille de l'utiliser directement. \n
 */

template <typename L, typename OP, typename R>
class ExprD
{
private :

  //! \brief Reference sur l'operande de gauche.
  const L & m_left ;
  //! \brief Operande de droite.
  const R m_right ;

public :

  //! \brief Type du resultat de l'expression
  typedef typename OP::value_type value_type ;

  //! \brief Type de l'expression
  typedef Expr<L,OP,R> expr_type ;

  //! @name Constructeurs
  //! @{
  /*! \brief Construit l'expression en referencant l'operande de gauche et en copiant l'operande de droite
   * 
   * \param left operande de gauche
   * \param right operande de droite
   */
  inline explicit ExprD(const L & left, const R & right)
  : m_left(left), m_right(right)
  {
    ;
  }

  //! @}

  //! @name Destructeur
  //! @{
  /*! \brief Destructeur.
   */
  inline ~ExprD()
  {
    ;
  }
  //! @}

  //! @name Accesseurs
  //! @{
  /*! \brief Retourne la taille de l'expression
   * Verifie si les operandes de gauche et de droite ont la meme taille, ou si l'une des deux est une constante. \n
   * Renvoie la taille de l'expression ou exit(1) si les operandes n'ont pas la meme taille. \n
   */
  inline int getSize() const 
  {
    int leftSize = m_left.getSize() ;
    int rightSize = m_right.getSize() ;
    if (leftSize == rightSize || leftSize == -1 || rightSize == -1 || rightSize == 1 || leftSize == 1)
    {
      return maxx(leftSize,rightSize) ;
    }
    else
    {
      std::cout << "LHS & RHS de taille differentes : exit(1)" << std::endl ;
      exit(1) ;
    }
  }

  /*! \brief Retourne le nombre d'inconnues de l'expression
   */
  inline int getNbInconnues() const 
  {
    return maxx(m_left.getNbInconnues(),m_right.getNbInconnues()) ;
  }

  /*! \brief Retourne la valeur de l'expression a l'index i
   * 
   * Fait appel a la methode getValue() de l'operande. \n 
   * \param i Index de la valeur recherchee
   * \warning Ne verifie pas si i valable pour l'expression
   */
  inline value_type getValue(int i) const
  {
    return OP::getValue(m_left, m_right,i) ;
  }

  /*! \brief Retourne la valeur de la derivee de l'expression a l'index ligne,colonne
   * 
   * Fait appel a la methode getDerivedValue() de l'operande. \n 
   * \param ligne Premier index
   * \param colonne Deuxieme index
   * \warning Ne verifie pas si ligne et colonne sont valables pour l'expression
   */
  inline value_type getDerivedValue(int ligne, int colonne) const
  {
    return OP::getDerivedValue(m_left, m_right, ligne,colonne) ;
  }

  inline bool isContainer() const
  {
    return true ;
  }
  //! @}

  //! Retourne la valeur de x
  inline double getX() const
  {
    return OP::getX(m_left,m_right) ;
  }

  //! Retourne la valeur de y
  inline double getY() const
  {
    return OP::getY(m_left,m_right) ;
  }

  //! Retourne la valeur de z
  inline double getZ() const
  {
    return OP::getZ(m_left,m_right) ;
  }
};

template <typename T>
class IExprProxy : public IExpr<typename T::value_type>
{
  T& m_exp ;
public:
  typedef typename T::value_type value_type ;
  IExprProxy(T& exp) : m_exp(exp) {} 
  virtual ~IExprProxy() {}

  inline void getValue(int size, value_type* val) const
  {
    register value_type tmp ;
    for (int i=size-1;i>=0;--i)
    {
      tmp = m_exp.getValue(i) ;
      val[i] = tmp ;
    }
  }

  inline value_type getValue(int i) const
  {
    return m_exp.getValue(i) ;
  }

  inline int getSize() const
  {
    return m_exp.getSize() ;
  }

  virtual inline bool isContainer() const
  {
    return m_exp.isContainer() ;
  }

};
#endif
