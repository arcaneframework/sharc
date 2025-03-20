#ifndef PROMOTE_H
#define PROMOTE_H

/*!
 * \file Promote.h
 * \brief Definition du type resultant des operations
 * \date 01/04/2008
 * \author Xavier TUNC
 * 
 * La classe Undefined est une classe vide servant a promouvoir le type resultant des operations. \n
 * La classe Promote permet de promouvoir le type resultant des operations. \n
 * Elle est specialisee pour chaque cas different que l'on peut rencontrer. \n
 * 
 */

/*! \brief Undefined : classe vide utilisee dans la promotion du type resultant des operations.
 */
class Undefined
{
};

/*! \brief Promote : Definition du type de resultat des operations
 * 
 * Classe promouvant le type du resultat des operations
 */

template <typename typeA, typename typeB>
class Promote
{
public :
//! \brief Type promu : type par defaut
  typedef Undefined type ;
};

/*! \brief Promote : specialisation de la classe 
 * 
 * Si les deux types sont des types de bases, similaires, on le promeut. \n
 * Une macro permet de generaliser cette specialisation pour tous les types de base.
 */
#define STD_OP_UN(type1,type2) \
template <> class Promote<type1,type2> \
{ \
public : \
  typedef type1 type ; \
} ;

STD_OP_UN(int, int)
STD_OP_UN(double,double)
STD_OP_UN(float,float)
#undef STD_OP_UN

/*! \brief Promote : specialisation de la classe 
 * 
 * Si les deux types sont des types de bases, non-similaires, on promeut le moins restrictif. \n
 * Une macro permet de generaliser cette specialisation pour tous les types de base.
 */
#define STD_OP_GAUCHE(type1,type2) \
template <> class Promote<type1,type2> \
{ \
public : \
  typedef type1 type ; \
} ;

STD_OP_GAUCHE(double,int)
STD_OP_GAUCHE(float,int)
STD_OP_GAUCHE(double,float)
#undef STD_OP_GAUCHE

/*! \brief Promote : specialisation de la classe 
 * 
 * Si les deux types sont des types de bases, non-similaires, on promeut le moins restrictif. \n
 * Une macro permet de generaliser cette specialisation pour tous les types de base.
 */
#define STD_OP_DROITE(type1,type2) \
template <> class Promote<type1,type2> { \
	public : \
	typedef type2 type ; \
} ;

STD_OP_DROITE(int,double)
STD_OP_DROITE(int,float)
STD_OP_DROITE(float,double)
#undef STD_OP_DROITE

/*! \brief Promote : specialisation de la classe 
 * 
 * Si les deux types ne sont pas des types de bases, on promeut le moins restrictif. \n
  */
template <typename type1, typename type2,typename type3, typename type4,typename type5, typename type6>
class Promote<Expr<type1,type2,type3>,Expr<type4,type5,type6> > 
{
public :
//! \brief value_type1 : type de la premiere expression		
typedef typename type2::value_type value_type1 ;
//! \brief value_type2 : type de la deuxieme expression	
typedef typename type4::value_type value_type2 ;
//! \brief value_type : type du resultat entre la premiere et la deuxieme expression	
typedef typename Promote<value_type1,value_type2>::type value_type ;
//! \brief type : type promu	
typedef value_type type;
} ;

/*! \brief Promote : specialisation de la classe 
 * 
 * Si les deux types ne sont pas des types de bases, on promeut le moins restrictif. \n
  */
template <typename type1, typename type2,typename type3, typename type4,typename type5, typename type6>
class Promote<Expr<type1,type2,type3>,ExprG<type4,type5,type6> > 
{
public :
//! \brief value_type1 : type de la premiere expression		
typedef typename type2::value_type value_type1 ;
//! \brief value_type2 : type de la deuxieme expression	
typedef typename type4::value_type value_type2 ;
//! \brief value_type : type du resultat entre la premiere et la deuxieme expression	
typedef typename Promote<value_type1,value_type2>::type value_type ;
//! \brief type : type promu	
typedef value_type type ;
} ;

/*! \brief Promote : specialisation de la classe 
 * 
 * Si les deux types ne sont pas des types de bases, on promeut le moins restrictif. \n
  */
template <typename type1, typename type2,typename type3, typename type4,typename type5, typename type6>
class Promote<ExprG<type1,type2,type3>,Expr<type4,type5,type6> > 
{
public :
//! \brief value_type1 : type de la premiere expression		
typedef typename type2::value_type value_type1 ;
//! \brief value_type2 : type de la deuxieme expression	
typedef typename type4::value_type value_type2 ;
//! \brief value_type : type du resultat entre la premiere et la deuxieme expression	
typedef typename Promote<value_type1,value_type2>::type value_type ;
//! \brief type : type promu	
typedef value_type type ;
} ;

/*! \brief Promote : specialisation de la classe 
 * 
 * Si les deux types ne sont pas des types de bases, on promeut le moins restrictif. \n
  */
template <typename type1, typename type2,typename type3, typename type4,typename type5, typename type6>
class Promote<ExprD<type1,type2,type3>,Expr<type4,type5,type6> > 
{
public :
//! \brief value_type1 : type de la premiere expression		
typedef typename type2::value_type value_type1 ;
//! \brief value_type2 : type de la deuxieme expression	
typedef typename type4::value_type value_type2 ;
//! \brief value_type : type du resultat entre la premiere et la deuxieme expression	
typedef typename Promote<value_type1,value_type2>::type value_type ;
//! \brief type : type promu	
typedef value_type type ;
} ;

/*! \brief Promote : specialisation de la classe 
 * 
 * Si les deux types ne sont pas des types de bases, on promeut le moins restrictif. \n
  */
template <typename type1, typename type2,typename type3, typename type4,typename type5, typename type6>
class Promote<Expr<type1,type2,type3>,ExprD<type4,type5,type6> > 
{
public :
//! \brief value_type1 : type de la premiere expression		
typedef typename type2::value_type value_type1 ;
//! \brief value_type2 : type de la deuxieme expression	
typedef typename type4::value_type value_type2 ;
//! \brief value_type : type du resultat entre la premiere et la deuxieme expression	
typedef typename Promote<value_type1,value_type2>::type value_type ;
//! \brief type : type promu	
typedef value_type type ;
} ;

/*! \brief Promote : specialisation de la classe 
 * 
 * Si les deux types ne sont pas des types de bases, on promeut le moins restrictif. \n
  */
template <typename type1, typename type2,typename type3, typename type4,typename type5, typename type6>
class Promote<ExprG<type1,type2,type3>,ExprG<type4,type5,type6> > 
{
public :
//! \brief value_type1 : type de la premiere expression		
typedef typename type2::value_type value_type1 ;
//! \brief value_type2 : type de la deuxieme expression	
typedef typename type4::value_type value_type2 ;
//! \brief value_type : type du resultat entre la premiere et la deuxieme expression	
typedef typename Promote<value_type1,value_type2>::type value_type ;
//! \brief type : type promu	
typedef value_type type ;
} ;

/*! \brief Promote : specialisation de la classe 
 * 
 * Si les deux types ne sont pas des types de bases, on promeut le moins restrictif. \n
  */
template <typename type1, typename type2,typename type3, typename type4,typename type5, typename type6>
class Promote<ExprG<type1,type2,type3>,ExprD<type4,type5,type6> > 
{
public :
//! \brief value_type1 : type de la premiere expression		
typedef typename type2::value_type value_type1 ;
//! \brief value_type2 : type de la deuxieme expression	
typedef typename type4::value_type value_type2 ;
//! \brief value_type : type du resultat entre la premiere et la deuxieme expression	
typedef typename Promote<value_type1,value_type2>::type value_type ;
//! \brief type : type promu	
typedef value_type type ;
} ;

/*! \brief Promote : specialisation de la classe 
 * 
 * Si les deux types ne sont pas des types de bases, on promeut le moins restrictif. \n
  */
template <typename type1, typename type2,typename type3, typename type4,typename type5, typename type6>
class Promote<ExprD<type1,type2,type3>,ExprG<type4,type5,type6> > 
{
public :
//! \brief value_type1 : type de la premiere expression		
typedef typename type2::value_type value_type1 ;
//! \brief value_type2 : type de la deuxieme expression	
typedef typename type4::value_type value_type2 ;
//! \brief value_type : type du resultat entre la premiere et la deuxieme expression	
typedef typename Promote<value_type1,value_type2>::type value_type ;
//! \brief type : type promu	
typedef value_type type ;
} ;

/*! \brief Promote : specialisation de la classe 
 * 
 * Si les deux types ne sont pas des types de bases, on promeut le moins restrictif. \n
  */
template <typename type1, typename type2,typename type3, typename type4,typename type5, typename type6>
class Promote<ExprD<type1,type2,type3>,ExprD<type4,type5,type6> > 
{
public :
//! \brief value_type1 : type de la premiere expression		
typedef typename type2::value_type value_type1 ;
//! \brief value_type2 : type de la deuxieme expression	
typedef typename type4::value_type value_type2 ;
//! \brief value_type : type du resultat entre la premiere et la deuxieme expression	
typedef typename Promote<value_type1,value_type2>::type value_type ;
//! \brief type : type promu	
typedef value_type type ;
} ;

#endif
