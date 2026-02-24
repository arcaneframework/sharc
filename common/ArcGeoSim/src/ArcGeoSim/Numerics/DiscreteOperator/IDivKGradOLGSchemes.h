// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef IDivKGradOLGSchemes_H
#define IDivKGradOLGSchemes_H

#include <arcane/ArcaneVersion.h>

#include <boost/version.hpp>
#if BOOST_VERSION >= 106400
#include <boost/serialization/array_wrapper.hpp>
#endif
#include <boost/multi_array.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/shared_ptr.hpp>

#include <arcane/ItemPairGroup.h>
#include "../../Mesh/Utils/AmrUtils.h"
#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"

#include "ArcGeoSim/Numerics/Utils/Algorithms/LUSolver.h"
#include "ArcGeoSim/Numerics/Utils/Algorithms/lu.h"
#include "ArcGeoSim/Numerics/DiscreteOperator/IDivKGradDiscreteOperator.h"
#include "ArcGeoSim/Numerics/DiscreteOperator/StencilBuilder.h"
#include "ArcGeoSim/Numerics/DiscreteOperator/TensorAlgebra.h"
#include "ArcGeoSim/Numerics/DiscreteOperator/TypesDivKGradOLGSchemes.h"
#include "ArcGeoSim/Utils/ItemComparator.h"
#include "ArcGeoSim/Utils/Utils.h"

using namespace Arcane;

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
class IDivKGradOLGSchemes
  : public IDivKGradDiscreteOperator
{
protected:
  typedef boost::numeric::ublas::vector<Real> RealVector;
  typedef boost::numeric::ublas::zero_vector<Real> ZeroVector;
  typedef boost::numeric::ublas::matrix<Real> RealMatrix;
  typedef boost::numeric::ublas::zero_matrix<Real> ZeroMatrix;
  typedef boost::numeric::ublas::range Range;
  typedef boost::numeric::ublas::matrix_range<RealMatrix> RealMatrixRange;
  typedef boost::numeric::ublas::permutation_matrix<std::size_t> PermutationMatrix;

public:
  enum ErrorCode {
    E_NotInitialized                = 0,
    E_UnknownBoundaryDiscretization = 1,
    E_CellGradientComputation       = 2,
    E_OBoundaryConditionComputation = 3,
    E_InconsistentConstruction      = 4,
    E_UnknownProperty               = 5
  };

  enum Property {
    P_Epsilon,
    P_CheckSign,
    P_DiagonalToOffDiagonalRatio
  };

  // Constructor
  IDivKGradOLGSchemes() :
    m_status(DiscreteOperatorProperty::S_NONE),
    m_epsilon(0.),
    m_check_sign(false),
    m_dtodr(-1.),
    m_amr(false),
    m_mesh(NULL),
    m_geometry_service(NULL),
    m_type(TypesDivKGradOLGSchemes::BC_TwoPoints),
    m_f_centers(NULL),
    m_f_normals(NULL),
    m_c_centers(NULL),
    m_f_measures(NULL)
  {
    // do nothing
  }

  //! Destructor
  ~IDivKGradOLGSchemes() {}

  struct Error {
    std::string msg;
    ErrorCode code;
    Error(const std::string& _msg, ErrorCode _code)
      : msg(_msg),
        code(_code)
    {
      // do nothing
    }
  };
  //! Preliminary computations (including connectivity)
  void prepare(IMesh * mesh,
               CoefficientArrayT<Cell>* cell_coefficients,
               CoefficientArrayT<Face>* face_coefficients);

  //! Preliminary computations (including connectivity)
  ARCANE_DEPRECATED void prepare(const FaceGroup& internal_faces,
                                 const FaceGroup& boundary_faces,
                                 FaceGroup& c_internal_faces,
                                 FaceGroup& cf_internal_faces,
                                 CoefficientArrayT<Cell>* cell_coefficients,
                                 CoefficientArrayT<Face>* face_coefficients);

  //! Reset the operator
  void finalize();

  //! Required cell geometric properties
  inline Integer getCellGeometricProperties() {
    return (IGeometryProperty::PCenter | IGeometryProperty::PMeasure);
  }
  //! Required face geometric properties
  inline Integer getFaceGeometricProperties() {
    return (IGeometryProperty::PCenter | IGeometryProperty::PNormal | IGeometryProperty::PMeasure);
  }
  //! Return the current status
  const Integer & status() const
  {
    return m_status;
  }

public:
  //! Return faces
  const FaceGroup& faces() const { return m_faces; }
  //! Return cells
  const CellGroup& cells() const { return m_cells; }
  //! Return boundary faces
  const FaceGroup& boundaryFaces() const { return m_boundary_faces; }
  //! Return internal faces
  const FaceGroup& internalFaces() const { return m_internal_faces; }

  //! Set property
  void set(Property a_property, const Real & a_value)
  {
    switch(a_property) {
    case P_Epsilon:
      m_epsilon=a_value;
      break;
    case P_DiagonalToOffDiagonalRatio:
      m_dtodr=a_value;
      break;
    default:
      throw(Error("Unknown property", E_UnknownProperty));
    }
  }

  //! Set property
  void set(Property a_property, const bool & a_value)
  {
    switch(a_property) {
    case P_CheckSign:
      m_check_sign=a_value;
      break;
    default:
      throw(Error("Unknown property", E_UnknownProperty));
    }
  }

protected:
  Integer m_status;  
  Real m_epsilon;
  // Enable/disable sign check for diagonal coefficients
  bool m_check_sign;
  // Diagonal to off-diagonal max ratio
  Real m_dtodr;

  bool m_amr;
  // Mesh
  IMesh * m_mesh;

  // Geometry service
  IGeometryMng* m_geometry_service;

  // Boundary condition type: two-points or Oscheme
  TypesDivKGradOLGSchemes::eType m_type;

  // Faces group
  FaceGroup m_faces;
  // Cells group
  CellGroup m_cells;
  // Boundary faces
  FaceGroup m_boundary_faces;
  // Internal faces
  FaceGroup m_internal_faces;
  // Internal faces whose stencil contains cell unknowns only
  FaceGroup m_c_internal_faces;
  // Internal faces whose stencil contains both cells and face unknowns
  FaceGroup m_cf_internal_faces;

  // Boundary nodes group
  NodeGroup m_boundary_nodes;
  // Boundary nodes group
  NodeGroup m_internal_nodes;

  // Local cell numbers around one node
  typedef VariableCellInteger CellNum;
  // Le type FaceNum permettra de stocker les numeros locaux des faces autour d'un noeud
  typedef std::vector<Integer> FaceNum;
  // Le type FaceNumByCell permettra de stocker pour chaque maille autour d'un noeud,
  // Les numeros locaux de ses faces qui sont faces voisines du noeud
  typedef std::vector<FaceNum> FaceNumByCell;

  // Cells group name
  String m_cells_group_name;
  // Faces group name
  String m_faces_group_name;

  // Cell coefficients
  CoefficientArrayT<Cell>* m_cell_coefficients;
  // Face coefficients
  CoefficientArrayT<Face>* m_face_coefficients;

  inline Integer _global_index(const ItemGroupRangeIteratorT<Face>& iface) {
    const Face& F = *iface;
    Integer i = iface.index();
    if( F.isSubDomainBoundary() ) i += m_internal_faces.size();
    return i;
  }

  TypesDivKGradOLGSchemes::eType getType() { return m_type; }
  void _compute_local_connectivity(const Node& node,
                                   CellNum & NumLocCell,
                                   FaceNumByCell& NumLocfacebyCell);

  void _compute_local_connectivity_amr(const Node& node,
                                      ItemVectorT<Cell> node_cells,
                                      CellNum & NumLocCell,
                                      FaceNumByCell& NumLocfacebyCell);
  void computeStencils();
  void computeStencils_amr();

  mutable const IGeometryMng::Real3Variable * m_f_centers;
  mutable const IGeometryMng::Real3Variable * m_f_normals;
  mutable const IGeometryMng::Real3Variable * m_c_centers;
  mutable const IGeometryMng::RealVariable * m_f_measures;

  boost::shared_ptr<VariableCellInteger> m_cell_numbering;

public:
  //! Two-point boundary treatment
  inline bool twoPointBoundary()
  {
    if(m_type==TypesDivKGradOLGSchemes::BC_TwoPoints ||
       m_type==TypesDivKGradOLGSchemes::BC_StandardTwoPoints)
      return true;
    else
      return false;
  }

  // Variables
  //! Form the discrete operator
  virtual void formDiscreteOperator(const VariableCellReal& k) = 0;
  //! Form the discrete operator
  virtual void formDiscreteOperator(const VariableCellReal3& k) = 0;
  //! Form the discrete operator
  virtual void formDiscreteOperator(const VariableCellReal3x3& k) = 0;

  // Shared variables
  //! Form the discrete operator
  virtual void formDiscreteOperator(const SharedVariableCellReal& k) = 0;
  //! Form the discrete operator
  virtual void formDiscreteOperator(const SharedVariableCellReal3& k) = 0;
  //! Form the discrete operator
  virtual void formDiscreteOperator(const SharedVariableCellReal3x3& k) = 0;

protected:
  // N is the space dimension
#if (ARCANE_VERSION >= 12201)
  template<Integer N, typename Comparator>
  void _compute_cell_gradient(const Node & node,
  		FaceNumByCell& NumLocfacesbyCell,
  		Array2<Real3> & Gradient,
  		Comparator & degeneration_comparator);

  void _compute_weak_cell_gradient(const Node & node,
  		FaceNumByCell & NumLocfacesbyCell,
  		Array2<Real3> & Gradient);
#else /* ARCANE_VERSION */
  template<Integer N, typename Comparator>
  void _compute_cell_gradient(const Node & node,
  		FaceNumByCell& NumLocfacesbyCell,
  		CArray2T<Real3> & Gradient,
  		Comparator & degeneration_comparator);

  void _compute_weak_cell_gradient(const Node & node,
  		FaceNumByCell & NumLocfacesbyCell,
  		CArray2T<Real3> & Gradient);
#endif /* ARCANE_VERSION */

  template<typename VariableTypeT, typename CellCoefficientArrayBuilder, typename FaceCoefficientArrayBuilder>
  void _compute_two_point_boundary_conditions(const Node & node,
  		const VariableTypeT & kappa,
  		CellCoefficientArrayBuilder & cell_builder,
  		FaceCoefficientArrayBuilder & face_builder);

  template<typename VariableTypeT, typename CellCoefficientArrayBuilder, typename FaceCoefficientArrayBuilder>
  void _compute_standard_two_point_boundary_conditions(const Node & node,
  		const VariableTypeT & kappa,
  		CellCoefficientArrayBuilder & cell_builder,
  		FaceCoefficientArrayBuilder & face_builder);

#if (ARCANE_VERSION >= 12201)
  template<typename VariableTypeT, typename CellCoefficientArrayBuilder, typename FaceCoefficientArrayBuilder>
  void _compute_o_scheme_boundary_conditions(const Node & node,
  		CellNum & NumLocCell,
  		FaceNumByCell& NumLocfacebyCell,
  		Array2<Real3> &Gradient,
  		const VariableTypeT & kappa,
  		CellCoefficientArrayBuilder & cell_ca_builder,
  		FaceCoefficientArrayBuilder & face_ca_builder);
#else /* ARCANE_VERSION */
  template<typename VariableTypeT, typename CellCoefficientArrayBuilder, typename FaceCoefficientArrayBuilder>
  void _compute_o_scheme_boundary_conditions(const Node & node,
  		CellNum & NumLocCell,
  		FaceNumByCell& NumLocfacebyCell,
  		CArray2T<Real3> &Gradient,
  		const VariableTypeT & kappa,
  		CellCoefficientArrayBuilder & cell_ca_builder,
  		FaceCoefficientArrayBuilder & face_ca_builder);
#endif /* ARCANE_VERSION */

private:
  // Compute stencils
  template<typename CellStencilBuilder, typename FaceStencilBuilder>
  void _compute_stencils(const FaceCellGroup & a_face_to_cell,
  		const FaceFaceGroup & a_face_to_bface,
  		const FaceCellGroup & a_bface_to_cell,
  		CellStencilBuilder & a_cell_stencil_builder,
  		FaceStencilBuilder & a_face_stencil_builder);

  template<typename CellStencilBuilder, typename FaceStencilBuilder>
  void _compute_stencils_amr(CellStencilBuilder & a_cell_stencil_builder,
                             FaceStencilBuilder & a_face_stencil_builder);
};

/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
template<typename VariableTypeT, typename CellCoefficientArrayBuilder, typename FaceCoefficientArrayBuilder>
void IDivKGradOLGSchemes<TCellStencilBuilder, TFaceStencilBuilder,
                         TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
_compute_two_point_boundary_conditions(const Node& node,
                                       const VariableTypeT& kappa,
                                       CellCoefficientArrayBuilder & cell_ca_builder,
                                       FaceCoefficientArrayBuilder & face_ca_builder)
{
  typedef typename VariableTraitsT<VariableTypeT>::DataType DiffusionType;

  int nbface = node.nbFace();
  // int nbmaille = node.nbCell();

  for(Integer iface=0;iface<nbface;++iface) {
    const Face& face = node.face(iface);

    Real3 nF          =  (*m_f_normals)[face];
    const Real3& xa   =  (*m_f_centers)[face];

    if(face.isSubDomainBoundary()){ // Two-point method for boundary faces
      const Cell& K = face.boundaryCell();
      const DiffusionType & kappa_K   = kappa[K];
      const Real3& xK   = (*m_c_centers)[K];

      const Real dKa    = math::abs(math::scaMul(xa-xK,nF));//distance de xK a la face
      const Real dK_A    = math::sqrt(math::scaMul(xa-xK,xa-xK));//distance de xK a xa
      const Real norme_nF = math::sqrt(math::scaMul(nF,nF));

      const Real tK     = math::scaMul(nF, DiscreteOperator::tensor_vector_prod<DiffusionType>::eval(kappa_K, nF)) / (dKa);
      Real cosO   = math::abs(math::scaMul(xa-xK,nF))/(dK_A * norme_nF);
      cosO *= cosO ;

      // Compute transmissivities
      const Real tau = tK  * cosO / face.nbNode();
      //         m_cell_coefficients->coefficient(face, K) += tau;
      //         m_face_coefficients->coefficient(face, face) -= tau;
      if(K.isActive()){
      cell_ca_builder.sum(face, K, tau);
        face_ca_builder.subtract(face, face, tau);
      }
    } else { // Try to extend MP method for non-boundary faces
      // Count the number of inner nodes
      Integer number_of_inner_nodes = 0;
      ENUMERATE_NODE(iP, face.nodes()) {
        const Node & P = *iP;
        bool is_inner_node = true;
        ENUMERATE_FACE(iF, P.faces()) {
          if(iF->isSubDomainBoundary()) {
            is_inner_node = false;
            break;
          }
        }
        if(is_inner_node)
          number_of_inner_nodes++;
      }

      if(number_of_inner_nodes > 0) { // Extend MP method
        Integer number_of_boundary_nodes = face.nbNode() - number_of_inner_nodes;
        // Adjusting factor
        Real adjusting_factor = std::pow((Real)face.nbNode() / (Real)number_of_inner_nodes, 1./number_of_boundary_nodes);
        // Extend MP method
        if(ArcGeoSim::Mesh::isActive(face))
          cell_ca_builder.multiply(face, adjusting_factor);
      } else { // Fall back to TP method
        const Cell& K   = face.backCell();
        const Cell& L   = face.frontCell();
        const DiffusionType & kappa_K   = kappa[K];
        const DiffusionType & kappa_L   = kappa[L];
        const Real3& xK   = (*m_c_centers)[K];
        const Real3& xL   = (*m_c_centers)[L];
        const Real dKa    = math::abs(math::scaMul(xa-xK,nF));//distance de xK a la face
        const Real dLa    = math::abs(math::scaMul(xa-xL,nF));//distance de xL a la face
        const Real dKL    = math::sqrt(math::scaMul(xL-xK,xL-xK));
        const Real norme_nF = math::sqrt(math::scaMul(nF,nF));

        const Real tK     = math::scaMul(nF, DiscreteOperator::tensor_vector_prod<DiffusionType>::eval(kappa_K, nF)) / (dKa * norme_nF);
        const Real tL     = math::scaMul(nF, DiscreteOperator::tensor_vector_prod<DiffusionType>::eval(kappa_L, nF)) / (dLa * norme_nF);


        Real cosO   = math::abs(math::scaMul(xL-xK,nF))/(dKL * norme_nF);
        cosO *= cosO ;
        // Compute transmissivities
        const Real tau = tK * tL * norme_nF * cosO /((tK + tL) * face.nbNode());

        if(ArcGeoSim::Mesh::isActive(face)){
          cell_ca_builder.sum(face, K, tau);
          cell_ca_builder.subtract(face, L, tau);
        }
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
template<typename VariableTypeT, typename CellCoefficientArrayBuilder, typename FaceCoefficientArrayBuilder>
void IDivKGradOLGSchemes<TCellStencilBuilder, TFaceStencilBuilder,
                         TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
_compute_standard_two_point_boundary_conditions(const Node& node,
                                                const VariableTypeT & kappa,
                                                CellCoefficientArrayBuilder & cell_ca_builder,
                                                FaceCoefficientArrayBuilder & face_ca_builder)
{
  typedef typename VariableTraitsT<VariableTypeT>::DataType DiffusionType;

  int nbface = node.nbFace();
  // int nbmaille = node.nbCell();

  for(Integer iface=0;iface<nbface;++iface) {
    const Face& face = node.face(iface);

    Real3 nF          =  (*m_f_normals)[face];
    const Real3& xa   =  (*m_f_centers)[face];
    if(face.isSubDomainBoundary()){ // Two-point method for boundary faces
      const Cell& K = face.boundaryCell();
      const DiffusionType & kappa_K   = kappa[K];
      const Real3& xK   = (*m_c_centers)[K];

      const Real dKa    = math::abs(math::scaMul(xa-xK,nF));//distance de xK a la face
      const Real tK     = math::scaMul(nF, DiscreteOperator::tensor_vector_prod<DiffusionType>::eval(kappa_K, nF)) / (dKa);

      // Compute transmissivities
      const Real tau = tK / face.nbNode();
      //         m_cell_coefficients->coefficient(face, K) += tau;
      //         m_face_coefficients->coefficient(face, face) -= tau;
      if(K.isActive()){
        cell_ca_builder.sum(face, K, tau);
        face_ca_builder.subtract(face, face, tau);
      }
    } else { // Try to extend MP method for non-boundary faces
      // Count the number of inner nodes
      Integer number_of_inner_nodes = 0;
      ENUMERATE_NODE(iP, face.nodes()) {
        const Node & P = *iP;
        bool is_inner_node = true;
        ENUMERATE_FACE(iF, P.faces()) {
          if(iF->isSubDomainBoundary()) {
            is_inner_node = false;
            break;
          }
        }
        if(is_inner_node)
          number_of_inner_nodes++;
      }

      if(number_of_inner_nodes > 0) { // Extend MP method
        Integer number_of_boundary_nodes = face.nbNode() - number_of_inner_nodes;
        // Adjusting factor
        Real adjusting_factor = std::pow((Real)face.nbNode() / (Real)number_of_inner_nodes, 1./number_of_boundary_nodes);
        // Extend MP method
        if(ArcGeoSim::Mesh::isActive(face))
          cell_ca_builder.multiply(face, adjusting_factor);
      } else { // Fall back to TP method
        const Cell& K   = face.backCell();
        const Cell& L   = face.frontCell();
        const DiffusionType & kappa_K   = kappa[K];
        const DiffusionType & kappa_L   = kappa[L];
        const Real3& xK   = (*m_c_centers)[K];
        const Real3& xL   = (*m_c_centers)[L];
        const Real dKa    = math::abs(math::scaMul(xa-xK,nF));//distance de xK a la face
        const Real dLa    = math::abs(math::scaMul(xa-xL,nF));//distance de xL a la face
        const Real norme_nF = math::sqrt(math::scaMul(nF,nF));

        const Real tK     = math::scaMul(nF, DiscreteOperator::tensor_vector_prod<DiffusionType>::eval(kappa_K, nF)) / (dKa * norme_nF);
        const Real tL     = math::scaMul(nF, DiscreteOperator::tensor_vector_prod<DiffusionType>::eval(kappa_L, nF)) / (dLa * norme_nF);

        // Compute transmissivities
        const Real tau = tK * tL * norme_nF /((tK + tL) * face.nbNode());
        if(ArcGeoSim::Mesh::isActive(face)){
          cell_ca_builder.sum(face, K, tau);
          cell_ca_builder.subtract(face, L, tau);
        }
      }
    }
  }
}

/*----------------------------------------------------------------------------*/

#if (ARCANE_VERSION >= 12201)
template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
template<typename VariableTypeT, typename CellCoefficientArrayBuilder, typename FaceCoefficientArrayBuilder>
void IDivKGradOLGSchemes<TCellStencilBuilder, TFaceStencilBuilder,
TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
_compute_o_scheme_boundary_conditions(const Node& node,
		CellNum & NumLocCell,
		FaceNumByCell& NumLocfacesbyCell,
		Array2<Real3> &Gradient,
		const VariableTypeT & kappa,
		CellCoefficientArrayBuilder & cell_ca_builder,
		FaceCoefficientArrayBuilder & face_ca_builder)
		{
	Integer nbface   = node.nbFace();
	Integer nbcell = node.nbCell();
	Integer N = m_mesh->dimension(); //space dimension

	typedef typename VariableTraitsT<VariableTypeT>::DataType DiffusionType;

	RealMatrix A = ZeroMatrix(nbface, nbface);
	RealMatrix B = ZeroMatrix(nbface, nbcell);
	RealMatrix C = ZeroMatrix(nbface, nbface);

	// Multiply gradients by kappa
	SharedArray2<Real3> kappa_gradient(node.nbCell(),node.nbFace()+1);
	for (Integer i = 0 ; i < node.nbCell() ; i++){
		const Cell & K = node.cell(i);
		const DiffusionType & kappa_K = kappa[K];
		for (Integer j = 0 ; j < node.nbFace()+1; j++){
			kappa_gradient[i][j] = DiscreteOperator::tensor_vector_prod<DiffusionType>::eval(kappa_K, Gradient[i][j]);
		}
	}

	// Find maximum number of faces
	UInt32 max_nb_local_faces=0;
	for(Integer i=0; i<nbcell; i++) {
		const FaceNum & face_local = NumLocfacesbyCell[i];
		if(face_local.size()>max_nb_local_faces)
			max_nb_local_faces = face_local.size();
	}
	// Initialize multi array for fluxes
	boost::multi_array<Real, 3> Fluxes(boost::extents[nbcell][nbface][max_nb_local_faces+1]);

	// Retrieve the normals
	SharedArray<Real3> normals(nbface);
	for (Integer i = 0 ; i < nbface ; i++){
		const Face& face  = node.face(i);
		normals[i] = (*m_f_normals)[face];
		if (face.isSubDomainBoundary() && (!face.isSubDomainBoundaryOutside()))
			normals[i] = -normals[i];
	}

	// Build the fluxes in terms of face variables and one cell variable
	for (Integer icell = 0 ; icell < nbcell ; icell++){
		const Cell & K = node.cell(icell);
		Integer mK = NumLocCell[K];
		FaceNum & face_local = NumLocfacesbyCell[mK];
		Integer nbfaceK =  face_local.size();

		// Main part of the flux
		for (Integer i = 0 ; i < nbfaceK ; ++i){  // Loop over the faces of cell sharing the node (rows of the matrix)
			Integer numloc_face = face_local[i];
			const Face & face = node.face(numloc_face);
			for (Integer j = 0 ; j <= nbfaceK ; j++)
				Fluxes[icell][numloc_face][j]  = -math::scaMul(kappa_gradient[mK][j], normals[numloc_face]) / face.nbNode();// We divide by the number of nodes
		}

		// Add the residual terms for the cells with more than 3 (in 3D) faces sharing a node
		if (nbfaceK != N){
			//calculate the residuals
		  SharedArray2<Real> Resid(nbfaceK, nbfaceK+1);
			SharedArray<Real3> dK_sigma(nbfaceK);
			const Real3 xK = (*m_c_centers)[K];
			Resid.fill(0.);
			for (Integer i = 0 ; i < nbfaceK ; i++){
				Integer numloc_face = face_local[i];
				const Face & face = node.face(numloc_face);
				dK_sigma[i] = ((*m_f_centers)[face] - xK);
			}
			for (Integer i = 0 ; i < nbfaceK ; i++){
				Resid[i][i] =1.;
				Resid[i][nbfaceK] = -1.;
				for (Integer j = 0 ; j <= nbfaceK ; j++){
					Resid[i][j] -= math::scaMul(Gradient[mK][j] , dK_sigma[i]);
				}
			}
			//calculation of the term with the sum
			SharedArray<Real3> sum(nbfaceK+1);
			sum.fill(Real3(0.,0.,0.));
			for (Integer i = 0 ; i <= nbfaceK ; i++)
				for (Integer j = 0 ; j < nbfaceK ; j++)
					sum[i] += Resid[j][i] / dK_sigma[j].abs2() *  dK_sigma[j];
			//Add the additional terms
			const DiffusionType & kappa_K = kappa[K];
			Real alpha = DiscreteOperator::MinimumEigenvalue::compute(kappa_K);
			Real m_Ks = 0;
			for (Integer i = 0 ; i < nbfaceK ; i++){
				Integer numloc_face = face_local[i];
				const Face & face = node.face(numloc_face);
				m_Ks += (*m_f_measures)[face] / face.nbNode() * dK_sigma[i].abs();
			}
			m_Ks /= N;
			Real coef = alpha * m_Ks;
			for (Integer i = 0 ; i < nbfaceK ; i++){
				Integer numloc_face = face_local[i];
				const Face & face = node.face(numloc_face);
				for (Integer j = 0 ; j <= nbfaceK ; j++)
					Fluxes[icell][numloc_face][j] -= coef * (Resid[i][j] / dK_sigma[i].abs2() - math::scaMul(Gradient[mK][i], sum[j])  ) / face.nbNode();// We divide by the number of nodes
			}
		}
	}

	// Construct the matrices ensuring the flux continuity
	for(Integer iface = 0 ; iface < nbface ; ++iface){
		const Face& face  = node.face(iface);
		if(face.isSubDomainBoundary()){
			A(iface, iface) = 1.0;
			C(iface, iface) = 1.0;
		}
		else{
			const Cell& K = face.backCell();
			const Cell& L = face.frontCell();
			//numerotation locale des mailles autour du noeud
			Integer mK = NumLocCell[K];
			Integer mL = NumLocCell[L];
			FaceNum & face_localK = NumLocfacesbyCell[mK];
			FaceNum & face_localL = NumLocfacesbyCell[mL];
			Integer nbfaceK =  face_localK.size();
			Integer nbfaceL =  face_localL.size();
			for(Integer i = 0 ; i < nbfaceK ; ++i){
				Integer numloc_face = face_localK[i];
				A(iface, numloc_face) += Fluxes[mK][iface][i] ;
			}
			for(Integer i = 0 ; i < nbfaceL ; ++i){
				Integer numloc_face = face_localL[i];
				A(iface, numloc_face) -= Fluxes[mL][iface][i] ;
			}
			B(iface, mK) = -Fluxes[mK][iface][nbfaceK] ;
			B(iface, mL) = Fluxes[mL][iface][nbfaceL] ;
		}
	}

	{
		PermutationMatrix P(nbface);
		Integer success = Numerics::lu_factorize(A, P, m_epsilon);
		if(success!=0)
			throw(Error("Fatal error in O boundary computation",
					E_OBoundaryConditionComputation));
		lu_substitute(A, P, B);
		lu_substitute(A, P, C);
	}

	// Compute the fluxes in terms of cell and boundary face variables
	for(Integer iface = 0; iface < nbface; ++iface){
		const Face & face = node.face(iface);
		Cell K;
		if(face.isSubDomainBoundary())
			K  = face.boundaryCell();
		else
			K  =face.backCell();
		// Local cell number around the node
		Integer mK   = NumLocCell[K];
		FaceNum & face_local = NumLocfacesbyCell[mK];
		Integer nbfaceK =  face_local.size();

		// Update the flux across the face "face"
		// Loop over the unknowns of the face belonging to the cell K
		for (Integer i = 0 ; i < nbfaceK ; ++i ) {
			// Local face number around the node
			Integer numloc_face = face_local[i];

			for(Integer icell = 0 ; icell < nbcell ; ++icell) {
				// Retrieve the cell with local number icell
				const Cell& M = node.cell(icell);
				// Add the contribution -inner_prod(prod(kappa, grad), normal)
				cell_ca_builder.sum(face, M, Fluxes[mK][iface][i]*B(numloc_face,icell));
			}
			for(Integer iface_ = 0 ; iface_ < nbface ; ++iface_){
				// Retrieve the cell with local number icell
				const Face& F = node.face(iface_);
				if(F.isSubDomainBoundary()) {
					// Add the face unknown contribution -inner_prod(prod(kappa, gradient), normal)
					face_ca_builder.sum(face, F, Fluxes[mK][iface][i]*C(numloc_face,iface_));
				}
			}
		}
		cell_ca_builder.sum(face, K, Fluxes[mK][iface][nbfaceK]);
	}
		}
#else /* ARCANE_VERSION */
template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
template<typename VariableTypeT, typename CellCoefficientArrayBuilder, typename FaceCoefficientArrayBuilder>
void IDivKGradOLGSchemes<TCellStencilBuilder, TFaceStencilBuilder,
                         TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
_compute_o_scheme_boundary_conditions(const Node& node,
                                      CellNum & NumLocCell,
                                      FaceNumByCell& NumLocfacesbyCell,
                                      CArray2T<Real3> &Gradient,
                                      const VariableTypeT & kappa,
                                      CellCoefficientArrayBuilder & cell_ca_builder,
                                      FaceCoefficientArrayBuilder & face_ca_builder)
{
  Integer nbface   = node.nbFace();
  Integer nbcell = node.nbCell();
  Integer N = m_mesh->dimension(); //space dimension

  typedef typename VariableTraitsT<VariableTypeT>::DataType DiffusionType;

  RealMatrix A = ZeroMatrix(nbface, nbface);
  RealMatrix B = ZeroMatrix(nbface, nbcell);
  RealMatrix C = ZeroMatrix(nbface, nbface);

  // Multiply gradients by kappa
  CArray2T<Real3> kappa_gradient(node.nbCell(),node.nbFace()+1);
  for (Integer i = 0 ; i < node.nbCell() ; i++){
    const Cell & K = node.cell(i);
    const DiffusionType & kappa_K = kappa[K];
    for (Integer j = 0 ; j < node.nbFace()+1; j++){
      kappa_gradient[i][j] = DiscreteOperator::tensor_vector_prod<DiffusionType>::eval(kappa_K, Gradient[i][j]);
    }
  }

  // Find maximum number of faces
  UInt32 max_nb_local_faces=0;
  for(Integer i=0; i<nbcell; i++) {
    const FaceNum & face_local = NumLocfacesbyCell[i];
    if(face_local.size()>max_nb_local_faces)
      max_nb_local_faces = face_local.size();
  }
  // Initialize multi array for fluxes
  boost::multi_array<Real, 3> Fluxes(boost::extents[nbcell][nbface][max_nb_local_faces+1]);

  // Retrieve the normals
  SharedArray<Real3> normals(nbface);
  for (Integer i = 0 ; i < nbface ; i++){
    const Face& face  = node.face(i);
    normals[i] = (*m_f_normals)[face];
    if (face.isSubDomainBoundary() && (!face.isSubDomainBoundaryOutside()))
      normals[i] = -normals[i];
  }

  // Build the fluxes in terms of face variables and one cell variable
  for (Integer icell = 0 ; icell < nbcell ; icell++){
    const Cell & K = node.cell(icell);
    Integer mK = NumLocCell[K];
    FaceNum & face_local = NumLocfacesbyCell[mK];
    Integer nbfaceK =  face_local.size();

    // Main part of the flux
    for (Integer i = 0 ; i < nbfaceK ; ++i){  // Loop over the faces of cell sharing the node (rows of the matrix)
      Integer numloc_face = face_local[i];
      const Face & face = node.face(numloc_face);
      for (Integer j = 0 ; j <= nbfaceK ; j++)
        Fluxes[icell][numloc_face][j]  = -math::scaMul(kappa_gradient[mK][j], normals[numloc_face]) / face.nbNode();// We divide by the number of nodes
    }

    // Add the residual terms for the cells with more than 3 (in 3D) faces sharing a node
    if (nbfaceK != N){
      //calculate the residuals
      CArray2T<Real> Resid(nbfaceK, nbfaceK+1);
      SharedArray<Real3> dK_sigma(nbfaceK);
      const Real3 xK = (*m_c_centers)[K];
      Resid.fill(0.);
      for (Integer i = 0 ; i < nbfaceK ; i++){
        Integer numloc_face = face_local[i];
        const Face & face = node.face(numloc_face);
        dK_sigma[i] = ((*m_f_centers)[face] - xK);
      }
      for (Integer i = 0 ; i < nbfaceK ; i++){
        Resid[i][i] =1.;
        Resid[i][nbfaceK] = -1.;
        for (Integer j = 0 ; j <= nbfaceK ; j++){
          Resid[i][j] -= math::scaMul(Gradient[mK][j] , dK_sigma[i]);
        }
      }
      //calculation of the term with the sum
      SharedArray<Real3> sum(nbfaceK+1);
      sum.fill(Real3(0.,0.,0.));
      for (Integer i = 0 ; i <= nbfaceK ; i++)
        for (Integer j = 0 ; j < nbfaceK ; j++)
          sum[i] += Resid[j][i] / dK_sigma[j].abs2() *  dK_sigma[j];
      //Add the additional terms
      const DiffusionType & kappa_K = kappa[K];
      Real alpha = DiscreteOperator::MinimumEigenvalue::compute(kappa_K);
      Real m_Ks = 0;
      for (Integer i = 0 ; i < nbfaceK ; i++){
        Integer numloc_face = face_local[i];
        const Face & face = node.face(numloc_face);
        m_Ks += (*m_f_measures)[face] / face.nbNode() * dK_sigma[i].abs();
      }
      m_Ks /= N;
      Real coef = alpha * m_Ks;
      for (Integer i = 0 ; i < nbfaceK ; i++){
        Integer numloc_face = face_local[i];
        const Face & face = node.face(numloc_face);
        for (Integer j = 0 ; j <= nbfaceK ; j++)
          Fluxes[icell][numloc_face][j] -= coef * (Resid[i][j] / dK_sigma[i].abs2() - math::scaMul(Gradient[mK][i], sum[j])  ) / face.nbNode();// We divide by the number of nodes
      }
    }
  }

  // Construct the matrices ensuring the flux continuity
  for(Integer iface = 0 ; iface < nbface ; ++iface){
    const Face& face  = node.face(iface);
    if(face.isSubDomainBoundary()){
      A(iface, iface) = 1.0;
      C(iface, iface) = 1.0;
    }
    else{
      const Cell& K = face.backCell();
      const Cell& L = face.frontCell();
      //numerotation locale des mailles autour du noeud
      Integer mK = NumLocCell[K];
      Integer mL = NumLocCell[L];
      FaceNum & face_localK = NumLocfacesbyCell[mK];
      FaceNum & face_localL = NumLocfacesbyCell[mL];
      Integer nbfaceK =  face_localK.size();
      Integer nbfaceL =  face_localL.size();
      for(Integer i = 0 ; i < nbfaceK ; ++i){
        Integer numloc_face = face_localK[i];
        A(iface, numloc_face) += Fluxes[mK][iface][i] ;
      }
      for(Integer i = 0 ; i < nbfaceL ; ++i){
        Integer numloc_face = face_localL[i];
        A(iface, numloc_face) -= Fluxes[mL][iface][i] ;
      }
      B(iface, mK) = -Fluxes[mK][iface][nbfaceK] ;
      B(iface, mL) = Fluxes[mL][iface][nbfaceL] ;
    }
  }

  {
    PermutationMatrix P(nbface);
    Integer success = Numerics::lu_factorize(A, P, m_epsilon);
    if(success!=0)
      throw(Error("Fatal error in O boundary computation",
                  E_OBoundaryConditionComputation));
    lu_substitute(A, P, B);
    lu_substitute(A, P, C);
  }

  // Compute the fluxes in terms of cell and boundary face variables
  for(Integer iface = 0; iface < nbface; ++iface){
    const Face & face = node.face(iface);
    Cell K;
    if(face.isSubDomainBoundary())
      K  = face.boundaryCell();
    else
      K  =face.backCell();
    // Local cell number around the node
    Integer mK   = NumLocCell[K];
    FaceNum & face_local = NumLocfacesbyCell[mK];
    Integer nbfaceK =  face_local.size();

    // Update the flux across the face "face"
    // Loop over the unknowns of the face belonging to the cell K
    for (Integer i = 0 ; i < nbfaceK ; ++i ) {
      // Local face number around the node    
      Integer numloc_face = face_local[i];

      for(Integer icell = 0 ; icell < nbcell ; ++icell) {
        // Retrieve the cell with local number icell
        const Cell& M = node.cell(icell);
        // Add the contribution -inner_prod(prod(kappa, grad), normal)
        cell_ca_builder.sum(face, M, Fluxes[mK][iface][i]*B(numloc_face,icell));
      }
      for(Integer iface_ = 0 ; iface_ < nbface ; ++iface_){
        // Retrieve the cell with local number icell
        const Face& F = node.face(iface_);
        if(F.isSubDomainBoundary()) {
          // Add the face unknown contribution -inner_prod(prod(kappa, gradient), normal)
          face_ca_builder.sum(face, F, Fluxes[mK][iface][i]*C(numloc_face,iface_));
        }
      }
    }
    cell_ca_builder.sum(face, K, Fluxes[mK][iface][nbfaceK]);
  }
}
#endif /* ARCANE_VERSION */

/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
template<typename CellStencilBuilder, typename FaceStencilBuilder>
void IDivKGradOLGSchemes<TCellStencilBuilder, TFaceStencilBuilder,
                         TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
_compute_stencils(const FaceCellGroup & a_face_to_cell,
                  const FaceFaceGroup & a_face_to_bface,
                  const FaceCellGroup & a_bface_to_cell,
                  CellStencilBuilder & a_cell_stencil_builder,
                  FaceStencilBuilder & a_face_stencil_builder)
{
  // Compute cell stencils for internal faces
  ENUMERATE_ITEMPAIR(Face, Cell, iface, a_face_to_cell) {
    const Face& F = *iface;
    ENUMERATE_SUB_ITEM(Cell, isubcell, iface) {
      a_cell_stencil_builder.insert(F, *isubcell);
    }
  }
 // Cell stencils for boundary faces and face stencils for boundary
  // and near boundary faces
  switch(m_type) {
  case TypesDivKGradOLGSchemes::BC_TwoPoints:
  case TypesDivKGradOLGSchemes::BC_StandardTwoPoints:
      ENUMERATE_FACE(iface, m_boundary_faces) {
      const Face& F = *iface;
      a_cell_stencil_builder.insert(F, F.boundaryCell());
      a_face_stencil_builder.insert(F, F);
    }
    break;
  case TypesDivKGradOLGSchemes::BC_Gscheme:
  case TypesDivKGradOLGSchemes::BC_Lscheme:
  case TypesDivKGradOLGSchemes::BC_Oscheme:
    // Compute cell stencils
    ENUMERATE_ITEMPAIR(Face, Cell, iface, a_bface_to_cell) {
      const Face& F = *iface;

      ENUMERATE_SUB_ITEM(Cell, isubcell, iface) {
        a_cell_stencil_builder.insert(F, *isubcell);
      }
    }

    // Compute face stencils
    ENUMERATE_ITEMPAIR(Face, Face, iface, a_face_to_bface) {
      const Face& F = *iface;
      if (F.isSubDomainBoundary())
        a_face_stencil_builder.insert(F, F);

      ENUMERATE_SUB_ITEM(Face, isubface, iface) {
        a_face_stencil_builder.insert(F, *isubface);
      }
    }
    break;
  }
}

/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
template<typename CellStencilBuilder, typename FaceStencilBuilder>
void IDivKGradOLGSchemes<TCellStencilBuilder, TFaceStencilBuilder,
                         TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
_compute_stencils_amr(CellStencilBuilder & a_cell_stencil_builder,
                      FaceStencilBuilder & a_face_stencil_builder)
{
  // Compute cell stencils for internal faces

  ENUMERATE_FACE(iface, m_internal_faces) {
      const Face& F = *iface;
    ENUMERATE_NODE(inode, F.nodes()){
        const Node& node = *inode;
          ItemVectorT<Cell> node_cells(this->m_mesh->cellFamily());
          ArcGeoSim::Mesh::amrCells(node, node_cells);
          //if(ArcGeoSim::Mesh::isActive(F))
          for (Integer i = 0 ; i < node_cells.size() ; i++){
            if(node_cells[i].isActive())
              a_cell_stencil_builder.insert(F, node_cells[i]);
          }
    }
  }

 // Cell stencils for boundary faces and face stencils for boundary
  // and near boundary faces
  switch(m_type) {
  case TypesDivKGradOLGSchemes::BC_TwoPoints:
  case TypesDivKGradOLGSchemes::BC_StandardTwoPoints:
      ENUMERATE_FACE(iface, m_boundary_faces) {
      const Face& F = *iface;
      a_cell_stencil_builder.insert(F, F.boundaryCell());
      a_face_stencil_builder.insert(F, F);
    }
    break;
default :
  ARCANE_ASSERT(0,("Only two point boundary condition in AMR"));
  }
}

/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
void IDivKGradOLGSchemes<TCellStencilBuilder, TFaceStencilBuilder,
                         TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
prepare(IMesh * mesh,
        CoefficientArrayT<Cell>* cell_coefficients,
        CoefficientArrayT<Face>* face_coefficients)
{
  ARCANE_ASSERT( (m_status & DiscreteOperatorProperty::S_INITIALIZED),
                 ("Operator not initialized when calling prepare") );

  m_mesh = mesh;

  VariableBuildInfo cell_numbering_vb(mesh,
                                      IMPLICIT_UNIQ_NAME,
                                      IVariable::PPrivate | IVariable::PTemporary);

  m_cell_numbering.reset( new VariableCellInteger(cell_numbering_vb) );

  m_cells          = mesh->allCells();
  m_faces          = mesh->allFaces();
  m_internal_faces = m_cells.innerFaceGroup();
  m_boundary_faces = m_cells.outerFaceGroup();

  m_cell_coefficients = cell_coefficients;
  m_face_coefficients = face_coefficients;

  // Form node group
  ItemGroupBuilder<Node> boundary_nodes_builder(m_boundary_faces.mesh(), IMPLICIT_NAME);
  ItemGroupBuilder<Node> internal_nodes_builder(m_internal_faces.mesh(), IMPLICIT_NAME);

  //noeuds de bord
  ENUMERATE_FACE( iface, m_boundary_faces ) {
    const Face& F = *iface;
    boundary_nodes_builder.add(F.nodes());
  }
  m_boundary_nodes = boundary_nodes_builder.buildGroup();


  // Internal nodes
  ENUMERATE_FACE(iface, m_internal_faces) {
    const Face& F = *iface;

    for(Integer inode=0; inode<F.nbNode(); inode++) {
      const Node& node = F.node(inode);
      bool isNotBoundary = true;

      for(Integer iface=0; iface<node.nbFace(); iface++) {
        const Face& face = node.face(iface);
        if (face.isSubDomainBoundary()) {
          isNotBoundary = false;
          break;
        }
      }

      if(isNotBoundary == false)
        break;
      else
        internal_nodes_builder.add(node);
    }
  }
  m_internal_nodes = internal_nodes_builder.buildGroup();

  // Compute stencils
    computeStencils();

  // Change status to prepared
  m_status |= DiscreteOperatorProperty::S_PREPARED;
}

/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
void IDivKGradOLGSchemes<TCellStencilBuilder, TFaceStencilBuilder,
                         TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
prepare(const FaceGroup& internal_faces,
        const FaceGroup& boundary_faces,
        FaceGroup& c_internal_faces,
        FaceGroup& cf_internal_faces,
        CoefficientArrayT<Cell>* cell_coefficients,
        CoefficientArrayT<Face>* face_coefficients)
{
  ARCANE_ASSERT( (m_status & DiscreteOperatorProperty::S_INITIALIZED),
                 ("Operator not initialized when calling prepare") );

  // Clear groups
  m_cells.clear();
  m_faces.clear();
  m_internal_nodes.clear();
  m_boundary_nodes.clear();
/*
  if(! (m_status & DiscreteOperatorProperty::S_PREPARED) )
    throw(Error(" Numerical service not initialized",
                E_NotInitialized));
*/
  m_mesh = internal_faces.mesh();
  m_amr =true;
  VariableBuildInfo cell_numbering_vb(internal_faces.mesh(),
                                      IMPLICIT_UNIQ_NAME,
                                      IVariable::PPrivate | IVariable::PTemporary);

  m_cell_numbering.reset( new VariableCellInteger(cell_numbering_vb) );


  m_internal_faces = internal_faces;
  m_boundary_faces = boundary_faces;

  m_cell_coefficients = cell_coefficients;
  m_face_coefficients = face_coefficients;

  // Form face and cell groups
  ItemGroupBuilder<Face> faces_builder(m_internal_faces.mesh(), m_faces_group_name);
  ItemGroupBuilder<Cell> cells_builder(m_internal_faces.mesh(), m_cells_group_name);

  ENUMERATE_FACE(iface, m_internal_faces) {
    const Face& F = *iface;
    cells_builder.add(F.cells());
    faces_builder.add(F);
  }
  ENUMERATE_FACE(iface, m_boundary_faces) {
    const Face& F = *iface;
    cells_builder.add(F.cells());
    faces_builder.add(F);
  }

  m_cells = cells_builder.buildGroup();//m_mesh->allActiveCells();//cells_builder.buildGroup();
  m_faces = faces_builder.buildGroup();//m_mesh->allActiveFaces();//faces_builder.buildGroup();

  // Form node group
  ItemGroupBuilder<Node> boundary_nodes_builder(m_boundary_faces.mesh(), IMPLICIT_NAME);
  ItemGroupBuilder<Node> internal_nodes_builder(m_internal_faces.mesh(), IMPLICIT_NAME);

  //noeuds de bord
  ENUMERATE_FACE( iface, m_boundary_faces ) {
    const Face& F = *iface;
    boundary_nodes_builder.add(F.nodes());
  }
  m_boundary_nodes = boundary_nodes_builder.buildGroup();


  // Internal nodes
  ENUMERATE_FACE(iface, m_internal_faces) {
    const Face& F = *iface;

    if(ArcGeoSim::Mesh::isActive(F))
    for(Integer inode=0; inode<F.nbNode(); inode++) {
      const Node& node = F.node(inode);
      bool isNotBoundary = true;

      for(Integer iface=0; iface<node.nbFace(); iface++) {
        const Face& face = node.face(iface);
        if (face.isSubDomainBoundary())
          {
            isNotBoundary = false;
            break;
          }
      }

      if(isNotBoundary == false)
        break;
      else
        internal_nodes_builder.add(node);
    }
  }
  m_internal_nodes = internal_nodes_builder.buildGroup();

  // Compute stencils
  if(this->m_amr)
    computeStencils_amr();
  else
  computeStencils();

  // Build face groups
  ItemGroupBuilder<Face> c_internal_faces_builder(m_internal_faces.mesh(),
                                                  c_internal_faces.name());
  ItemGroupBuilder<Face> cf_internal_faces_builder(m_internal_faces.mesh(),
                                                   cf_internal_faces.name());

  ENUMERATE_FACE(iF, m_internal_faces) {
    const Face& F = *iF;
    if (m_face_coefficients->stencilSize(F)) {
      cf_internal_faces_builder.add(F);
    }
    else
      c_internal_faces_builder.add(F);
  }

  m_c_internal_faces = c_internal_faces_builder.buildGroup();
  m_cf_internal_faces = cf_internal_faces_builder.buildGroup();

  // Change status to prepared
  m_status |= DiscreteOperatorProperty::S_PREPARED;
}

/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
void IDivKGradOLGSchemes<TCellStencilBuilder, TFaceStencilBuilder,
                         TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
finalize()
{
  ARCANE_ASSERT( (m_status & DiscreteOperatorProperty::S_PREPARED),
                 ("Operator not prepared when calling finalize") );
  m_status = DiscreteOperatorProperty::S_INITIALIZED;
}

/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
void IDivKGradOLGSchemes<TCellStencilBuilder, TFaceStencilBuilder,
                         TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
computeStencils()
{
  // Cells sharing at least one node with a face
  FaceCellGroup face_to_cell(m_internal_faces, m_cells, IK_Node);

  // Initialize boundary nodes indicator
  Integer MaxNodeId = m_faces.mesh()->nodeFamily()->maxLocalId();
  SharedArray<Byte> node_is_boundary(MaxNodeId);
  node_is_boundary.fill(0);
  ENUMERATE_NODE(inode, m_boundary_nodes){
    const Node& node = *inode;
    node_is_boundary[node.localId()] = 1;
  }

  // Create group of faces which have at least one node on the boundary
  FaceGroup near_and_boun_faces;
  ItemGroupBuilder<Face> near_and_boun_faces_builder(m_faces.mesh(), IMPLICIT_NAME);
  ENUMERATE_FACE(iface, m_boundary_faces) {
    near_and_boun_faces_builder.add(*iface);
  }
  ENUMERATE_FACE(iface, m_internal_faces) {
    const Face& F = *iface;
    ENUMERATE_NODE(inode, F.nodes()) {
      if (node_is_boundary[inode->localId()]){
        near_and_boun_faces_builder.add(F);
        break;
      }
    }
  }
  near_and_boun_faces = near_and_boun_faces_builder.buildGroup();

  // Create connectivity table between near_and_boun faces and boundary
  // faces through nodes
  FaceFaceGroup face_to_bface(near_and_boun_faces, m_boundary_faces, IK_Node);

  // Create connectivity table between boundary faces and cells
  FaceCellGroup bface_to_cell(m_boundary_faces, m_cells, IK_Node);

  // Compute stencil sizes and init containers
  {
    StencilSizesBuilderT<Cell> cell_stencil_sizes_builder(m_cell_coefficients);
    StencilSizesBuilderT<Face> face_stencil_sizes_builder(m_face_coefficients);

    _compute_stencils(face_to_cell,
                      face_to_bface,
                      bface_to_cell,
                      cell_stencil_sizes_builder,
                      face_stencil_sizes_builder);

    cell_stencil_sizes_builder.finalize();
    face_stencil_sizes_builder.finalize();
  }

  // Compute stencils
  {
    TCellStencilBuilder cell_stencil_builder(m_cell_coefficients);
    TFaceStencilBuilder face_stencil_builder(m_face_coefficients);

    _compute_stencils(face_to_cell,
                      face_to_bface,
                      bface_to_cell,
                      cell_stencil_builder,
                      face_stencil_builder);

    cell_stencil_builder.finalize();
    face_stencil_builder.finalize();
  }

  // Clear work groups
  near_and_boun_faces.clear();
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
void IDivKGradOLGSchemes<TCellStencilBuilder, TFaceStencilBuilder,
                         TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
computeStencils_amr()
{

  // Initialize boundary nodes indicator
  Integer MaxNodeId = m_faces.mesh()->nodeFamily()->maxLocalId();
  SharedArray<Byte> node_is_boundary(MaxNodeId);
  node_is_boundary.fill(0);
  ENUMERATE_NODE(inode, m_boundary_nodes){
    const Node& node = *inode;
    node_is_boundary[node.localId()] = 1;
  }

  // Create group of faces which have at least one node on the boundary
  FaceGroup near_and_boun_faces;
  ItemGroupBuilder<Face> near_and_boun_faces_builder(m_faces.mesh(), IMPLICIT_NAME);
  ENUMERATE_FACE(iface, m_boundary_faces) {
    near_and_boun_faces_builder.add(*iface);
  }
  ENUMERATE_FACE(iface, m_internal_faces) {
    const Face& F = *iface;
    ENUMERATE_NODE(inode, F.nodes()) {
      if (node_is_boundary[inode->localId()]){
        near_and_boun_faces_builder.add(F);
        break;
      }
    }
  }
  near_and_boun_faces = near_and_boun_faces_builder.buildGroup();


  // Compute stencil sizes and init containers
  {
    StencilSizesBuilderT<Cell> cell_stencil_sizes_builder(m_cell_coefficients);
    StencilSizesBuilderT<Face> face_stencil_sizes_builder(m_face_coefficients);

    _compute_stencils_amr(cell_stencil_sizes_builder,
                            face_stencil_sizes_builder);

    cell_stencil_sizes_builder.finalize();
    face_stencil_sizes_builder.finalize();
  }

  // Compute stencils
  {
    TCellStencilBuilder cell_stencil_builder(m_cell_coefficients);
    TFaceStencilBuilder face_stencil_builder(m_face_coefficients);
      _compute_stencils_amr(cell_stencil_builder,
                            face_stencil_builder);

    cell_stencil_builder.finalize();
    face_stencil_builder.finalize();
  }

  // Clear work groups
  near_and_boun_faces.clear();

}

/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
void IDivKGradOLGSchemes<TCellStencilBuilder, TFaceStencilBuilder,
                         TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
_compute_local_connectivity(const Node & node,
                            CellNum & NumLocCell,
                            FaceNumByCell & NumLocfacebyCell)
{

  // Local cell renumbering for fast access
  for(Integer icell=0; icell<node.nbCell(); ++icell) {
    const Cell & cell = node.cell(icell);
    NumLocCell[cell] = icell;
    NumLocfacebyCell.reserve(2);
  }
  // For each cell around the node, build the set of cell faces
  // sharing the node
  for(Integer iface=0; iface<node.nbFace(); ++iface) {
    const Face& face = node.face(iface);
    if(face.isSubDomainBoundary()) {
      const Cell cell0 = face.boundaryCell();
      Integer numloc_cell0  = NumLocCell[cell0];
      NumLocfacebyCell[numloc_cell0].push_back(iface);
    } else {
      const Cell cellback       = face.backCell();
      const Cell cellfront      = face.frontCell();
      Integer numloc_cellback  = NumLocCell[cellback];
      Integer numloc_cellfront = NumLocCell[cellfront];
      NumLocfacebyCell[numloc_cellback].push_back(iface);
      NumLocfacebyCell[numloc_cellfront].push_back(iface);
    }
  }
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
void IDivKGradOLGSchemes<TCellStencilBuilder, TFaceStencilBuilder,
                         TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
_compute_local_connectivity_amr(const Node & node,
                                ItemVectorT<Cell> node_cells,
                                CellNum & NumLocCell,
                                FaceNumByCell & NumLocfacebyCell)
{

  // Local cell renumbering for fast access
  //for(CellEnumerator it_cell=node_cells.enumerator();it_cell();++it_cell){
  for(Integer icell = 0 ; icell < node_cells.size() ;++icell){
    const Cell& cell = node_cells[icell];
    NumLocCell[cell] = icell;
    NumLocfacebyCell.reserve(2);
  }
  // For each cell around the node, build the set of cell faces
  // sharing the node
  for(Integer iface = 0 ; iface < node.nbFace() ;++iface){
    const Face& face = node.face(iface);
    if(face.isSubDomainBoundary()) {
      const Cell cell0 = face.boundaryCell();
      Integer numloc_cell0  = NumLocCell[cell0];
      NumLocfacebyCell[numloc_cell0].push_back(iface);
    } else {
      const Cell cellback       = face.backCell();
      const Cell cellfront      = face.frontCell();
      Integer numloc_cellback  = NumLocCell[cellback];
      Integer numloc_cellfront = NumLocCell[cellfront];
      NumLocfacebyCell[numloc_cellback].push_back(iface);
      NumLocfacebyCell[numloc_cellfront].push_back(iface);
    }
  }
}

/*----------------------------------------------------------------------------*/

#if (ARCANE_VERSION >= 12201)
template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
template<Integer N, typename Comparator>
void IDivKGradOLGSchemes<TCellStencilBuilder, TFaceStencilBuilder,
TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
_compute_cell_gradient(const Node & P,
		FaceNumByCell & NumLocfacesbyCell,
		Array2<Real3> & gradient,
		Comparator & degeneration_comparator)
		{
	for(Integer iK=0; iK<P.nbCell(); ++iK) {
		const Cell & K = P.cell(iK);

		// Build matrix B
		RealMatrix B = ZeroMatrix(N, N);

		// Local face numbers
		FaceNum & face_local = NumLocfacesbyCell[iK];
		// Number of faces of K that share the node P
		Integer nbface =  face_local.size();

		// Loop over the faces of cell K that share the node P
		for(Integer iF=0; iF<nbface; ++iF) {
			// Local face number around the node
			Integer numloc_face = face_local[iF];
			const Face & F = P.face(numloc_face);
			// Retrieve geometric properties
			const Real3 & xF = (*this->m_f_centers)[F];
			const Real3 & xK = (*this->m_c_centers)[K];
			const Real3 & nF = (*this->m_f_normals)[F];
			Real3 dFK = xF - xK;
			Real3 nKF = (math::scaMul(nF, dFK) > 0) ? nF : -nF;

			B += DiscreteOperator::vector_outer_prod<N>::eval(nKF, dFK);
		} // End loop over faces

		// Factorize matrix B
		bool apply_gradient_correction = true;
		LUSolver<Real> lu;
		try {
			lu.factor(B);
		}
		catch(const LUSolver<Real>::Error & e) {
			throw(Error("Singular matrix", E_CellGradientComputation));
			apply_gradient_correction = false;
		}

		if(!degeneration_comparator.compare(apply_gradient_correction))
			throw(Error("Inconsistent construction in gradient computation",
					E_InconsistentConstruction));

		gradient[iK][nbface]  = Real3(0.,0.,0.);

		for(Integer iF=0; iF<nbface; ++iF) {
			// Local face number around the node
			Integer numloc_face = face_local[iF];
			const Face & F = P.face(numloc_face);
			// Retrieve geometric properties
			const Real3 & xF = (*this->m_f_centers)[F];
			const Real3 & xK = (*this->m_c_centers)[K];
			const Real3 & nF = (*this->m_f_normals)[F];
			Real3 nKF = (math::scaMul(nF, xF-xK) > 0) ? nF : -nF;
			// Right-hand side
			RealVector b = DiscreteOperator::real3_to_realvector<N>::convert(nKF);

			if(apply_gradient_correction) {
				lu.overwrite_solve(b);
			}
			Real3 x0 = DiscreteOperator::realvector_to_real3<N>::convert(b);

			gradient[iK][iF]      = x0;
			gradient[iK][nbface] -= x0;
		} // End loop over faces
	} // End loop over cells
		}
#else /* ARCANE_VERSION */
template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
template<Integer N, typename Comparator>
void IDivKGradOLGSchemes<TCellStencilBuilder, TFaceStencilBuilder,
                         TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
_compute_cell_gradient(const Node & P,
                       FaceNumByCell & NumLocfacesbyCell,
                       CArray2T<Real3> & gradient,
                       Comparator & degeneration_comparator)
{
  for(Integer iK=0; iK<P.nbCell(); ++iK) {
    const Cell & K = P.cell(iK);

    // Build matrix B
    RealMatrix B = ZeroMatrix(N, N);

    // Local face numbers
    FaceNum & face_local = NumLocfacesbyCell[iK];
    // Number of faces of K that share the node P
    Integer nbface =  face_local.size();

    // Loop over the faces of cell K that share the node P
    for(Integer iF=0; iF<nbface; ++iF) {
      // Local face number around the node
      Integer numloc_face = face_local[iF];
      const Face & F = P.face(numloc_face);
      // Retrieve geometric properties
      const Real3 & xF = (*this->m_f_centers)[F];
      const Real3 & xK = (*this->m_c_centers)[K];
      const Real3 & nF = (*this->m_f_normals)[F];
      Real3 dFK = xF - xK;
      Real3 nKF = (math::scaMul(nF, dFK) > 0) ? nF : -nF;

      B += DiscreteOperator::vector_outer_prod<N>::eval(nKF, dFK);
    } // End loop over faces

    // Factorize matrix B
    bool apply_gradient_correction = true;
    LUSolver<Real> lu;
    try {
      lu.factor(B);
    }
    catch(const LUSolver<Real>::Error & e) {
      throw(Error("Singular matrix", E_CellGradientComputation));
      apply_gradient_correction = false;
    }

    if(!degeneration_comparator.compare(apply_gradient_correction))
      throw(Error("Inconsistent construction in gradient computation", 
                  E_InconsistentConstruction));

    gradient[iK][nbface]  = Real3(0.,0.,0.);

    for(Integer iF=0; iF<nbface; ++iF) {
      // Local face number around the node
      Integer numloc_face = face_local[iF];
      const Face & F = P.face(numloc_face);
      // Retrieve geometric properties
      const Real3 & xF = (*this->m_f_centers)[F];
      const Real3 & xK = (*this->m_c_centers)[K];
      const Real3 & nF = (*this->m_f_normals)[F];
      Real3 nKF = (math::scaMul(nF, xF-xK) > 0) ? nF : -nF;
      // Right-hand side
      RealVector b = DiscreteOperator::real3_to_realvector<N>::convert(nKF);

      if(apply_gradient_correction) {
        lu.overwrite_solve(b);
      }        
      Real3 x0 = DiscreteOperator::realvector_to_real3<N>::convert(b);

      gradient[iK][iF]      = x0;
      gradient[iK][nbface] -= x0;
    } // End loop over faces
  } // End loop over cells
}
#endif /* ARCANE_VERSION */

/*----------------------------------------------------------------------------*/

#if (ARCANE_VERSION >= 12201)
template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
void IDivKGradOLGSchemes<TCellStencilBuilder, TFaceStencilBuilder,
TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
_compute_weak_cell_gradient(const Node & node,
		FaceNumByCell & NumLocfacesbyCell,
		Array2<Real3> & gradient)
		{
	Integer N = m_mesh->dimension(); // Space dimension
	Integer nbcell = node.nbCell();
	gradient.fill(Real3(0., 0., 0.));

	// Loop over the cells sharing a node
	for(Integer iK = 0; iK < nbcell; iK++) {
		const Cell & K   = node.cell(iK);
		FaceNum & face_local = NumLocfacesbyCell[iK];
		Integer nbfaceK =  face_local.size();

		UniqueArray<Real> dK_sigma(nbfaceK);
		const Real3 & xK = (*this->m_c_centers)[K];

		for (Integer iF = 0 ; iF < nbfaceK ; iF++) {
			Integer numloc_face = face_local[iF];
			const Face & F = node.face(numloc_face);
			const Real3 & xF = (*this->m_f_centers)[F];
			const Real3 & nF = (*this->m_f_normals)[F];
			const Real & mF = (*this->m_f_measures)[F];
			// We divide by mF to obtain the unit normal
			dK_sigma[iF] = math::abs(math::scaMul(xF-xK,nF)/mF);
		}

		Real m_Ks = 0.;
		for (Integer iF = 0 ; iF < nbfaceK ; iF++) {
			Integer numloc_face = face_local[iF];
			const Face & F = node.face(numloc_face);
			const Real & mF = (*this->m_f_measures)[F];
			m_Ks += mF/F.nbNode()*dK_sigma[iF];
		}
		m_Ks /= N;

		Real coef = 1./m_Ks;
		for (Integer iF = 0 ; iF < nbfaceK ; iF++) {
			Integer numloc_face = face_local[iF];
			const Face & F = node.face(numloc_face);
			Real3 nF = (*this->m_f_normals)[F]/F.nbNode();
			if (K != F.backCell()) {
				nF = -nF;
			}
			Real3 comp = coef * nF;
			gradient[iK][iF]      += comp;
			gradient[iK][nbfaceK] -= comp;
		}
	}
		}
#else /* ARCANE_VERSION */
template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
void IDivKGradOLGSchemes<TCellStencilBuilder, TFaceStencilBuilder,
                         TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
_compute_weak_cell_gradient(const Node & node,
                            FaceNumByCell & NumLocfacesbyCell,
                            CArray2T<Real3> & gradient)
{
  Integer N = m_mesh->dimension(); // Space dimension
  Integer nbcell = node.nbCell();
  gradient.fill(Real3(0., 0., 0.));

  // Loop over the cells sharing a node
  for(Integer iK = 0; iK < nbcell; iK++) {
    const Cell & K   = node.cell(iK);
    FaceNum & face_local = NumLocfacesbyCell[iK];
    Integer nbfaceK =  face_local.size();

    SharedArray<Real> dK_sigma(nbfaceK);
    const Real3 & xK = this->m_c_centers[K];

    for (Integer iF = 0 ; iF < nbfaceK ; iF++) {
      Integer numloc_face = face_local[iF];
      const Face & F = node.face(numloc_face);
      const Real3 & xF = (*this->m_f_centers)[F];
      const Real3 & nF = (*this->m_f_normals)[F];
      const Real & mF = (*this->m_f_measures)[F];
      // We divide by mF to obtain the unit normal
      dK_sigma[iF] = math::abs(math::scaMul(xF-xK,nF)/mF);
    }

    Real m_Ks = 0.;
    for (Integer iF = 0 ; iF < nbfaceK ; iF++) {
      Integer numloc_face = face_local[iF];
      const Face & F = node.face(numloc_face);
      const Real & mF = (*this->m_f_measures)[F];
      m_Ks += mF/F.nbNode()*dK_sigma[iF];
    }
    m_Ks /= N;

    Real coef = 1./m_Ks;
    for (Integer iF = 0 ; iF < nbfaceK ; iF++) {
      Integer numloc_face = face_local[iF];
      const Face & F = node.face(numloc_face);
      Real3 nF = (*this->m_f_normals)[F]/F.nbNode();
      if (K != F.backCell()) {
        nF = -nF;
      }
      Real3 comp = coef * nF;
      gradient[iK][iF]      += comp;
      gradient[iK][nbfaceK] -= comp;
    }
  }
}
#endif /* ARCANE_VERSION */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#endif
