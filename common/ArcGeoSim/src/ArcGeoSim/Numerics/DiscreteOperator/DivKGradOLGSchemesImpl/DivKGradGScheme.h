// -*- C++ -*-
#ifndef DIVKGRADGSCHEME_H
#define DIVKGRADGSCHEME_H

#include <arcane/ArcaneVersion.h>
#include <arcane/utils/ITraceMng.h>
#include <arcane/IMeshSubMeshTransition.h>
#include <arcane/VariableTypedef.h>
#include <boost/shared_ptr.hpp>

#include "ArcGeoSim/Numerics/DiscreteOperator/CoefficientArrayBuilder.h"
#include "ArcGeoSim/Numerics/DiscreteOperator/Comparator.h"
#include "ArcGeoSim/Numerics/DiscreteOperator/IDivKGradOLGSchemes.h"
#include "ArcGeoSim/Numerics/DiscreteOperator/DivKGradOLGSchemesImpl/DivKGradLSchemeStencils.h"
#include "ArcGeoSim/Numerics/Utils/Algorithms/LUSolver.h"
#include "ArcGeoSim/Numerics/Utils/lapackUtils.h"

/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
class DivKGradGScheme
  : public IDivKGradOLGSchemes<TCellStencilBuilder, TFaceStencilBuilder,
                               TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>
{
public:
  typedef IDivKGradOLGSchemes<TCellStencilBuilder, TFaceStencilBuilder,
                              TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder> BaseClass;

  typedef DivKGradGScheme<TCellStencilBuilder, TFaceStencilBuilder,
                          TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder> ThisClass;

  typedef TCellStencilBuilder CellStencilBuilderType;
  typedef TFaceStencilBuilder FaceStencilBuilderType;
  typedef TCellCoefficientArrayBuilder CellCoefficientArrayBuilderType;
  typedef TFaceCoefficientArrayBuilder FaceCoefficientArrayBuilderType;

protected:
  friend struct OperatorImplementationT<OperatorImplementationTraitsT<ThisClass>::type>;

  OperatorImplementationT<OperatorImplementationTraitsT<ThisClass>::type> m_implementation;
  
  typedef OperatorImplementationT<OperatorImplementationTraitsT<ThisClass>::type> ImplementationType;
  typedef typename ImplementationType::ComparatorType ComparatorType;

private:
  typedef typename TCellCoefficientArrayBuilder::Initializer CellCoefficientArrayInitializer;
  typedef typename TFaceCoefficientArrayBuilder::Initializer FaceCoefficientArrayInitializer;

  typedef typename TCellCoefficientArrayBuilder::Inserter CellCoefficientArrayInserter;
  typedef typename TFaceCoefficientArrayBuilder::Inserter FaceCoefficientArrayInserter;

  typedef typename BaseClass::CellNum CellNum;
  typedef typename BaseClass::FaceNum FaceNum;
  typedef typename BaseClass::FaceNumByCell FaceNumByCell;

public:
  DivKGradGScheme(ITraceMng * a_trace_mng)
    : m_trace_mng(a_trace_mng)
  {
    // do nothing
  }

  //! Form discrete operator. N is the space dimension
  template<Integer N, typename VariableTypeT>
  void formDiscreteOperatorT(const VariableTypeT & kappa);

private:
  boost::shared_ptr<CellCoefficientArrayInitializer> m_cell_ca_initializer;
  boost::shared_ptr<FaceCoefficientArrayInitializer> m_face_ca_initializer;

  boost::shared_ptr<CellCoefficientArrayInserter> m_cell_ca_inserter;
  boost::shared_ptr<FaceCoefficientArrayInserter> m_face_ca_inserter;

  boost::shared_ptr<ComparatorType> m_degeneration1_comparator;
  boost::shared_ptr<ComparatorType> m_degeneration2_comparator;

  ITraceMng * m_trace_mng;

  // Compute discrete operator. N is the space dimension
  template<Integer N,
           typename VariableTypeT, typename CellCoefficientArrayBuilder, typename FaceCoefficientArrayBuilder,
           typename Degeneration1Comparator, typename Degeneration2Comparator>
  void _compute_discrete_operator(const VariableTypeT & kappa,
                                  CellCoefficientArrayBuilder & cell_ca_builder,
                                  FaceCoefficientArrayBuilder & face_ca_builder,
                                  Degeneration1Comparator & degeneration1_comparator, 
                                  Degeneration2Comparator & degeneration2_comparator);

  template<typename VariableTypeT>
  Real _compute_trans_centered_stencil(const Node& node, CellNum & NumLocCell,
                                       const VariableTypeT& kappa,
                                       L_Stencil& centered_stencil);

  Real _coercivity(Real x, Real eps = 0.1, Real n = 1.);
};

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
template<Integer N, typename VariableTypeT>
void DivKGradGScheme<TCellStencilBuilder, TFaceStencilBuilder,
                     TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
formDiscreteOperatorT(const VariableTypeT & kappa)
{

  ImplementationType::template apply<N, ThisClass, VariableTypeT>(this, kappa);
  this->m_status |= DiscreteOperatorProperty::S_FORMED;
}

/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
template<Integer N,
         typename VariableTypeT,
         typename CellCoefficientArrayBuilder, typename FaceCoefficientArrayBuilder,
         typename Degeneration1Comparator, typename Degeneration2Comparator>
void DivKGradGScheme<TCellStencilBuilder, TFaceStencilBuilder,
                     TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
_compute_discrete_operator(const VariableTypeT & kappa,
                           CellCoefficientArrayBuilder & cell_ca_builder,
                           FaceCoefficientArrayBuilder & face_ca_builder,
                           Degeneration1Comparator & degeneration1_comparator, 
                           Degeneration2Comparator & degeneration2_comparator)
{
  ARCANE_ASSERT( (this->m_status & DiscreteOperatorProperty::S_PREPARED),
                 ("Operator not prepared") );

  // Retrieve geometric properties
  this->m_f_centers  = &this->m_geometry_service->getReal3VariableProperty(this->m_faces, IGeometryProperty::PCenter);
  this->m_f_normals  = &this->m_geometry_service->getReal3VariableProperty(this->m_faces, IGeometryProperty::PNormal);
  this->m_f_measures = &this->m_geometry_service->getRealVariableProperty(this->m_faces, IGeometryProperty::PMeasure);
  this->m_c_centers  = &this->m_geometry_service->getReal3VariableProperty(this->m_cells, IGeometryProperty::PCenter);

  Integer MaxFaceId = this->m_internal_faces.mesh()->faceFamily()->maxLocalId();

  // Internal nodes
  SharedArray<Real> weight_normalize(MaxFaceId);
  SharedArray<Integer> non_boun_nodes(MaxFaceId);
  weight_normalize.fill(0.);
  non_boun_nodes.fill(0);
  VariableNodeReal3 coords = PRIMARYMESH_CAST(this->m_internal_faces.mesh())->nodesCoordinates();

  ENUMERATE_NODE(inode, this->m_internal_nodes) {
    const Node& node = *inode;
    //Allocation du vecteur
    FaceNumByCell NumLocfacesbyCell;
    NumLocfacesbyCell.resize(node.nbCell());

    this->_compute_local_connectivity(node, *(this->m_cell_numbering),NumLocfacesbyCell);

    //Calculate the number of stencils
    Integer NumStencils = 0;
    for (Integer i = 0 ; i < node.nbCell() ; i++) {
      if (NumLocfacesbyCell[i].size() == 3) {
        NumStencils ++;
      }
      else{
        NumStencils += 4;
      }
    }

    //Initialize the centered stencils
    L_Stencil **centered_stencils;
    centered_stencils = new L_Stencil*[NumStencils];
    //Compute the transmissibilities
    SharedArray<Real> weight(NumStencils);
    Integer ist = 0;
    for (Integer icell=0 ; icell<node.nbCell() ; ++icell) {
      if (NumLocfacesbyCell[icell].size() == 3) {
        centered_stencils[ist] = new L_Stencil(node, icell, *(this->m_cell_numbering), NumLocfacesbyCell);
        weight[ist] = _compute_trans_centered_stencil<VariableTypeT>(node, *(this->m_cell_numbering),  kappa, *centered_stencils[ist]);
        ist++;
      }
      else{  // Case of pyramid vertex
        for (Integer iface = 0 ; iface < 4 ; iface++) {
          centered_stencils[ist] = new L_Stencil(node, icell, iface, *(this->m_cell_numbering), NumLocfacesbyCell);
          weight[ist] = _compute_trans_centered_stencil<VariableTypeT>(node, *(this->m_cell_numbering),  kappa, *centered_stencils[ist]);
          ist++;
        }
      }
    }

    // Calculate the non-boundary nodes for each face
    for (Integer iface = 0; iface < node.nbFace(); iface++) {
      const Face & face = node.face(iface);
      non_boun_nodes[face.localId()]++;
    }

    for (Integer stencil_num=0 ; stencil_num < NumStencils ; ++stencil_num) { //loop over the stencils
      for (Integer iface = 0; iface < 3 ; iface++) {
        Integer locnbface = centered_stencils[stencil_num]->getFace(iface);
        const Face & face = node.face(locnbface);
        weight_normalize[face.localId()] += weight[stencil_num];
        for (Integer icell = 0; icell < 4 ; icell++) {
          cell_ca_builder.subtract(face,
                                   node.cell(centered_stencils[stencil_num]->getCell(icell)),
                                   centered_stencils[stencil_num]->transm[iface][icell]);
        }
      }
    }
    for (Integer ist = 0 ; ist < NumStencils; ist++)
      delete centered_stencils[ist];
    delete[] centered_stencils;
  }

  if (this->m_type != TypesDivKGradOLGSchemes::BC_Gscheme) {
    // Get the proper coefficients
    ENUMERATE_FACE(iface, this->m_internal_faces) {
      const Face & F = *iface;
      Integer nbnodes = F.nbNode();
      if (weight_normalize[F.localId()]==0.) {
        m_trace_mng->fatal()<<" No non-degenerate stencils found inside of the domain (case of non-G b.c.) for face #"<<F.localId();
      }
      if (non_boun_nodes[F.localId()])
        weight_normalize[F.localId()] = non_boun_nodes[F.localId()]/(weight_normalize[F.localId()]*nbnodes);
      else
        weight_normalize[F.localId()] = 1.;
    }

    // Normalization of transmissibilities
    ENUMERATE_FACE(iface, this->m_internal_faces) {
      const Face & F = *iface;
      ItemVectorView c_stencilF = this->m_cell_coefficients->stencil(F);
      Integer c_st_size = this->m_cell_coefficients->stencilSize(F);
      for (Integer i = 0; i < c_st_size ; i++) {
        const Cell & cell = c_stencilF[i].toCell();
        cell_ca_builder.multiply(F, cell, weight_normalize[F.localId()]);
      }
    }
  }


  // Boundary nodes
  switch(this->m_type) {
  case TypesDivKGradOLGSchemes::BC_Gscheme : {
    ENUMERATE_NODE(inode, this->m_boundary_nodes) {
      const Node& node = *inode;
      //Allocation du vecteur
      FaceNumByCell NumLocfacesbyCell;
      NumLocfacesbyCell.resize(node.nbCell());
      this->_compute_local_connectivity(node, *(this->m_cell_numbering),NumLocfacesbyCell);

      //Calculate the number of stencils
      Integer NumStencils = 0;
      for (Integer i = 0 ; i < node.nbCell() ; i++) {
        if (NumLocfacesbyCell[i].size() == 3) {
          NumStencils ++;
        }
        else{
          NumStencils += 4;
        }
      }

      //Initialize the centered stencils
      L_Stencil **centered_stencils;
      centered_stencils = new L_Stencil*[NumStencils];

      //Compute the transmissibilities
      SharedArray<Real> weight(NumStencils);
      Integer ist = 0;
      for (Integer icell=0 ; icell < node.nbCell() ; ++icell) {
        if (NumLocfacesbyCell[icell].size() == 3) {
          centered_stencils[ist] = new L_Stencil(node, icell, *(this->m_cell_numbering), NumLocfacesbyCell);
          weight[ist] = _compute_trans_centered_stencil<VariableTypeT>(node, *(this->m_cell_numbering),  kappa, *centered_stencils[ist]);
          ist++;
        }
        else{  // Case of pyramid vertex
          for (Integer iface = 0 ; iface < 4 ; iface++) {
            centered_stencils[ist] = new L_Stencil(node, icell, iface, *(this->m_cell_numbering), NumLocfacesbyCell);
            weight[ist] = _compute_trans_centered_stencil<VariableTypeT>(node, *(this->m_cell_numbering),  kappa, *centered_stencils[ist]);
            ist++;
          }
        }
      }

      //Retrieve the transmissibilities
      for (Integer stencil_num = 0 ; stencil_num < NumStencils ; ++stencil_num) { //loop over the stencils
        for (Integer iface = 0; iface < 3 ; iface++) {
          Integer locnbface = centered_stencils[stencil_num]->getFace(iface);
          const Face & face = node.face(locnbface);
          weight_normalize[face.localId()] += weight[stencil_num];
          for (Integer icell = 0; icell < 4; icell++) {
            Integer num_cell = centered_stencils[stencil_num]->getCell(icell);
            if (num_cell == -1) {  //boundary face
              face_ca_builder.subtract(face, 
                                       node.face(centered_stencils[stencil_num]->getFace(icell-1)),
                                       centered_stencils[stencil_num]->transm[iface][icell]);
            }
            else{
              cell_ca_builder.subtract(face, 
                                       node.cell(centered_stencils[stencil_num]->getCell(icell)),
                                       centered_stencils[stencil_num]->transm[iface][icell]);
            }
          }
        }
      }
      for (Integer ist = 0 ; ist < NumStencils; ist++)
        delete centered_stencils[ist];
      delete[] centered_stencils;
    }

    // Invert the weighting coefficients
    ENUMERATE_FACE(iface, this->m_faces) {
      const Face & F = *iface;
      if (weight_normalize[F.localId()]!=0.)
        weight_normalize[F.localId()] = 1./weight_normalize[F.localId()];
      else
        m_trace_mng->fatal()<<" No non-degenerate stencils found for face #"<<F.localId();
    }

    // Normalization of transmissibilities
    ENUMERATE_FACE(iface, this->m_faces) {
      const Face & F = *iface;
      ItemVectorView c_stencilF = this->m_cell_coefficients->stencil(F);
      Integer c_st_size = this->m_cell_coefficients->stencilSize(F);
      for (Integer i = 0; i < c_st_size ; i++) {
        const Cell & cell = c_stencilF[i].toCell();
        cell_ca_builder.multiply(F, cell, weight_normalize[F.localId()]);
      }
      Integer f_st_size = this->m_face_coefficients->stencilSize(F);
      if (f_st_size) {
        ItemVectorView f_stencilF = this->m_face_coefficients->stencil(F);
        for (Integer i = 0; i < f_st_size ; i++) {
          const Face & face = f_stencilF[i].toFace();
          face_ca_builder.multiply(F, face, weight_normalize[F.localId()]);
        }
      }
    }
    break;
  }
  case TypesDivKGradOLGSchemes::BC_TwoPoints : {// schema deux points pour les noeuds de bord
    ENUMERATE_NODE(inode, this->m_boundary_nodes) {
      const Node& node = *inode;
      this->BaseClass::template _compute_two_point_boundary_conditions<VariableTypeT,
        CellCoefficientArrayBuilder,
        FaceCoefficientArrayBuilder>(node, kappa, cell_ca_builder, face_ca_builder);
    }
    break;
  }
  case TypesDivKGradOLGSchemes::BC_StandardTwoPoints : {// schema deux points pour les noeuds de bord
    ENUMERATE_NODE(inode, this->m_boundary_nodes) {
      const Node& node = *inode;
      this->BaseClass::template _compute_standard_two_point_boundary_conditions<VariableTypeT,
        CellCoefficientArrayBuilder,
        FaceCoefficientArrayBuilder>(node, kappa, cell_ca_builder, face_ca_builder);
    }
    break;
  }
  case TypesDivKGradOLGSchemes::BC_Oscheme : {//  O-Schema pour les noeuds de bord
    ENUMERATE_NODE(inode, this->m_boundary_nodes) {
      const Node& node = *inode;
      //Allocation du vecteur
      FaceNumByCell NumLocfacesbyCell;
      NumLocfacesbyCell.resize(node.nbCell());
      // Pour chaque maille autour du noeud, on calcule Lambda Gradient
      SharedArray2<Real3> Gradient(node.nbCell(),node.nbFace()+1);
      this->_compute_local_connectivity(node, *(this->m_cell_numbering),NumLocfacesbyCell);
      try{
        TrueComparatorT<bool> true_comparator;
        this->BaseClass::template
          _compute_cell_gradient<3, TrueComparatorT<bool> >
          (node, NumLocfacesbyCell, Gradient, true_comparator) ;
        this->BaseClass::template _compute_o_scheme_boundary_conditions<VariableTypeT,
          CellCoefficientArrayBuilder,
          FaceCoefficientArrayBuilder>(node,
                                       *(this->m_cell_numbering), NumLocfacesbyCell, Gradient,
                                       kappa, cell_ca_builder, face_ca_builder);
      }
      catch (typename BaseClass::Error e) {
        m_trace_mng->fatal() << e.msg;
      }
    }
    break;
  }
  default:
    m_trace_mng->error() << "Unknown boundary condition type";
  }
}

/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
template<typename VariableTypeT>
Real DivKGradGScheme<TCellStencilBuilder, TFaceStencilBuilder,
                     TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
_compute_trans_centered_stencil(const Node& node, CellNum & NumLocCell,
                                const VariableTypeT & kappa,
                                L_Stencil& centered_stencil)
{
  typedef typename VariableTraitsT<VariableTypeT>::DataType DiffusionType;

  const Integer N = 3;
  LUSolver<Real>::matrix_type A(N,N);
  LUSolver<Real>::matrix_type B(N,N+1);
  A.clear();
  B.clear();

  const Cell & K = node.cell(centered_stencil.getCell(0));
  const DiffusionType& LambdaK = kappa[K];
  Real3 xK = (*this->m_c_centers)[K];

  // Generate the system for the gradient recovery on the central cell
  for (Integer i = 0 ; i < N ; i++) {
    const Face & face = node.face(centered_stencil.getFace(i));
    //internal face
    if (!face.isSubDomainBoundary()) {
      const Cell& L = node.cell(centered_stencil.getCell(i+1));
      const DiffusionType& LambdaL = kappa[L];
      Real3 xL = (*this->m_c_centers)[L];
      Real3 xKL = xL - xK;
      Real3 normalK, normalL;
      //get the unit normals
      if (face.backCell() == K) {
        normalK = (*this->m_f_normals)[face]/(*this->m_f_measures)[face];
        normalL = -normalK;
      }
      else{
        normalL = (*this->m_f_normals)[face]/(*this->m_f_measures)[face];
        normalK = -normalL;
      }
      Real3 Lam_Kn_K = DiscreteOperator::tensor_vector_prod<DiffusionType>::eval(LambdaK,normalK);
      Real3 Lam_Ln_L = DiscreteOperator::tensor_vector_prod<DiffusionType>::eval(LambdaL,normalL);
      Real d_L_sigma = math::abs(math::scaMul(xKL, normalK));
      Real k = math::scaMul(Lam_Ln_L, normalL)/d_L_sigma;
      Real3 row = k*xKL + Lam_Kn_K + Lam_Ln_L;
      A(i,0) = row.x;
      A(i,1) = row.y;
      A(i,2) = row.z;
      B(i,i+1) = k;
      B(i,0) = -k;
    }
    // boundary face
    else{
      Real3 x_sigma = (*this->m_f_centers)[face];
      Real3 row= x_sigma - xK;
      A(i,0) = row.x;
      A(i,1) = row.y;
      A(i,2) = row.z;
      B(i,i+1) = 1.;
      B(i,0) = -1.;
    }
  }

  // Find the gradient
  LUSolver<Real>::matrix_type Grad(3,4);
  LUSolver<Real> lu;
  try {
    lu.factor(A);
    Grad = lu.solve(B); //Grad=A^{-1}B
  }
  catch(LUSolver<Real>::Error e) { //Exceptional case when the matrix is singular
    m_trace_mng->fatal() << e.msg;
  }
  // Check if the matrix is very close to singular
  Real det = lu.det();
  Real Max = 0.;
  for (std::size_t i = 0 ; i < A.size1() ; i++) {
    for (std::size_t j = 0 ; j < A.size2() ; j++) {
      Max = math::max(Max,math::abs(A(i,j)));
    }
  }
  //Scale the determinant
  det /= Max;
  if (math::abs(det)<1e-8) {
    return 0.;
  }

  // Compute Lambda*gradient
  SharedArray<Real3> LambdaGradient(4);
  for (Integer i = 0 ; i < 4 ; i++) {
    Real3 column;
    column.x = Grad(0,i);
    column.y = Grad(1,i);
    column.z = Grad(2,i);
    LambdaGradient[i] = DiscreteOperator::tensor_vector_prod<DiffusionType>::eval(LambdaK,column);
  }

  for (Integer i = 0 ; i < N ; i++) {
    const Face & face = node.face(centered_stencil.getFace(i));
    Real3 normal = (*this->m_f_normals)[face];
    if (face.isSubDomainBoundary() && !face.isSubDomainBoundaryOutside())
      normal = -normal;
    for (Integer j = 0 ; j < 4 ; j++) {
      centered_stencil.transm[i][j] = math::scaMul(LambdaGradient[j], normal);
    }
  }

  // Compute the weighting coefficient
  SharedArray<Real> dKL(N);
  for (Integer i = 0; i < N ; i++) {
    Integer icell =centered_stencil.getCell(i+1);
    Integer iface = centered_stencil.getFace(i);
    const Face & face = node.face(iface);
    Real3 xL;
    if (icell == -1)
      xL = (*this->m_f_centers)[face];
    else
      xL = (*this->m_c_centers)[node.cell(icell)];
    Real3 xKL = xL - xK;
    Real3 normal = (*this->m_f_normals)[face];
    dKL[i] = math::abs(math::scaMul(xKL,normal));
  }

  SharedArray<Real> d_m(N);
  for (Integer i = 0 ; i < N ; i++)
    d_m[i] = math::sqrt(dKL[i])/(*this->m_f_measures)[node.face(centered_stencil.getFace(i))];

  // begin the version for Xsyev
  double A2[N*N];
  for (Integer i = 0 ; i < N ; i++) {
    Integer nline = i * N;
    for (Integer j = i ; j < N ; j++)
      A2[nline + j] = 0.5*(centered_stencil.transm[i][j+1]+centered_stencil.transm[j][i+1])*d_m[i]*d_m[j];
    for (Integer j = 0 ; j < i ; j++)
      A2[nline + j] = A2[j * N + i] ;
  }

  char  JOBS = 'N';
  char UPLO = 'U';
  const int dim = N;
  double Eigens[dim];
  const int lwork = 26*dim;
  double work[lwork];
  int INFO;
  XSYEV<double>::apply(JOBS, UPLO, dim, A2, dim, Eigens, work, lwork, INFO);
  // end the version for Xsyev

  Real weight = Eigens[0];
  for (Integer i = 1 ; i < dim; i++) {
    weight = math::min(weight, Eigens[i]);
  }

  weight = _coercivity(weight);

  // Copy the transmissibilities to the containers
  for (Integer i=0; i < N; i++) 
    for (Integer j=0 ; j < N+1; j++) 
      centered_stencil.transm[i][j] *= weight;
  
  return weight;
}

/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
Real DivKGradGScheme<TCellStencilBuilder, TFaceStencilBuilder,
                     TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
_coercivity(Real x,Real eps , Real n)
{
  if(x < 0)
    return (eps * eps /(eps - x));
  else
    return (pow(eps,1.0-n) * pow(x+eps,n)/n + (eps * (n-1))/n) ;  
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#endif
