#ifndef REAL3_H
#define REAL3_H

/*!
 * \file Real3.h
 * \brief Un point reel en 3 dimension
 * \date 22/04/2008
 * \author Xavier TUNC
 */

/*!
 * \brief Real3 : un point reel en trois dimensions
 * 
 * Cette classe encapsule un point en trois dimensions (x,y et z). \n
 * La methode getX() (resp. getY() et getZ) permet de recuperer la valeur de m_x (resp. m_y et m_z). \n
 * L'operateur = est surcharge pour pouvoir construire un Real3 a partir d'une expression. \n
*/

struct Real3
{
//! Les coordonnees suivant x
  double m_x ;
//! Les coordonnees suivant y
  double m_y ;
//! Les coordonnees suivant z
  double m_z ;

//! @name Constructeur
//! @{

/*! Constructeur par defaut
 * 
 * Cree un point en (0,0,0)
 */
  inline Real3() : m_x(0), m_y(0), m_z(0) {} ;

/*! Construit un point a partir des valeurs passees en parametres
 * 
 * Cree un point en (x,y,z)
 * \param x Les coordonnees en x
 * \param y Les coordonnees en y
 * \param z Les coordonnees en z
*/
  inline Real3(double x, double y, double z) : m_x(x), m_y(y), m_z(z) 
  {
    ;
  }
//! @}
//! @name Destructeur
//! @{
//! \brief Destructeur
  inline ~Real3() 
  {
    ;
  }
//! @}

//! @name Surcharge d'operateurs
//! @{

/*! Operateur d'affectation
 * 
 * Construit un point a partir d'un autre point
 * 
 * \param pt Point Real3
*/
  inline Real3 & operator=(const Real3 & pt )
  {
    m_x = pt.m_x ;
    m_y = pt.m_y ;
    m_z = pt.m_z ;
    return (*this) ;
  }

/*! Operateur d'affectation
 * 
 * Construit un point a partir d'une expression
 * 
 * \param re Expression de type Expr<xpr1,xpr2,xpr3>
 */
  template <typename xpr1, typename xpr2, typename xpr3>
  inline Real3 & operator=(const Expr<xpr1,xpr2,xpr3> & re )
  {
    m_x = re.getX() ;
    m_y = re.getY() ;
    m_z = re.getZ() ;
    return (*this) ;
  }

//! @}

//! @name Accesseurs
//! @{

//! Retourne la valeur m_x
  inline double getX() const 
  {
    return m_x ;
  }
  
//! Retourne la valeur m_y
  inline double getY() const 
  {
    return m_y ;
  }

//! Retourne la valeur m_z
  inline double getZ() const 
  {
    return m_z ;
  }

//! Affiche les coordonnes du point
  void affiche() 
  {
    std::cout << "x : " << m_x << std::endl ;
    std::cout << "y : " << m_y << std::endl ;
    std::cout << "z : " << m_z << std::endl ;
  }
};

#endif
