// -*- C++ -*-
#ifndef DIVKGRADLSCHEME_H
#define DIVKGRADLSCHEME_H

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

#include "ArcGeoSim/Numerics/DiscreteOperator/OperatorImplementation.h"

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
class DivKGradLScheme
  : public IDivKGradOLGSchemes<TCellStencilBuilder, TFaceStencilBuilder,
                               TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>
{
public:
  typedef DivKGradLScheme<TCellStencilBuilder, TFaceStencilBuilder,
                          TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder> ThisClass;

  typedef IDivKGradOLGSchemes<TCellStencilBuilder, TFaceStencilBuilder,
                              TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder> BaseClass;

  typedef TCellStencilBuilder CellStencilBuilderType;
  typedef TFaceStencilBuilder FaceStencilBuilderType;
  typedef TCellCoefficientArrayBuilder CellCoefficientArrayBuilderType;
  typedef TFaceCoefficientArrayBuilder FaceCoefficientArrayBuilderType;

private:
  typedef typename TCellCoefficientArrayBuilder::Initializer CellCoefficientArrayInitializer;
  typedef typename TFaceCoefficientArrayBuilder::Initializer FaceCoefficientArrayInitializer;

  typedef typename TCellCoefficientArrayBuilder::Inserter CellCoefficientArrayInserter;
  typedef typename TFaceCoefficientArrayBuilder::Inserter FaceCoefficientArrayInserter;

  typedef typename BaseClass::CellNum CellNum;
  typedef typename BaseClass::FaceNum FaceNum;
  typedef typename BaseClass::FaceNumByCell FaceNumByCell;

public:
  DivKGradLScheme(ITraceMng * a_trace_mng)
    : m_trace_mng(a_trace_mng)
  {
    // do nothing
  }

  //! Form discrete operator. N is the space dimension
  template<Integer N, typename VariableTypeT>
  void formDiscreteOperatorT(const VariableTypeT & kappa);

protected:
  friend struct OperatorImplementationT<OperatorImplementationTraitsT<ThisClass>::type>;

  OperatorImplementationT<OperatorImplementationTraitsT<ThisClass>::type> m_implementation;

  typedef OperatorImplementationT<OperatorImplementationTraitsT<ThisClass>::type> ImplementationType;
  typedef typename ImplementationType::ComparatorType ComparatorType;

private:
  boost::shared_ptr<CellCoefficientArrayInitializer> m_cell_ca_initializer;
  boost::shared_ptr<FaceCoefficientArrayInitializer> m_face_ca_initializer;

  boost::shared_ptr<CellCoefficientArrayInserter> m_cell_ca_inserter;
  boost::shared_ptr<FaceCoefficientArrayInserter> m_face_ca_inserter;

  boost::shared_ptr<ComparatorType> m_degeneration1_comparator;
  boost::shared_ptr<ComparatorType> m_degeneration2_comparator;

  ITraceMng * m_trace_mng;

  // Compute transmissibilities for centered L stencil
  template<typename VariableTypeT>
  Byte _compute_trans_centered_stencil(const Node& node,CellNum & NumLocCell,
                                       const VariableTypeT & kappa,
                                       L_Stencil& centered_stencil);

  template<typename VariableTypeT>
  Byte _compute_trans_centered_stencil_amr(const Node& node,CellNum & NumLocCell,
                                           ItemVectorT<Cell> node_cells,
                                           const VariableTypeT & kappa,
                                           L_Stencil& centered_stencil);
  // Compute discrete operator. N is the space dimension
  template<Integer N,
           typename VariableTypeT, 
           typename CellCoefficientArrayBuilder, typename FaceCoefficientArrayBuilder,
           typename Degeneration1Comparator, typename Degeneration2Comparator>
  void _compute_discrete_operator(const VariableTypeT & kappa,
                                  CellCoefficientArrayBuilder & cell_ca_builder,
                                  FaceCoefficientArrayBuilder & face_ca_builder,
                                  Degeneration1Comparator & degeneration1_comparator,
                                  Degeneration2Comparator & degeneration2_comparator);

  // Compute discrete operator. "OLD VERSION ", N is the space dimension
  template<Integer N,
           typename VariableTypeT,
           typename CellCoefficientArrayBuilder, typename FaceCoefficientArrayBuilder,
           typename Degeneration1Comparator, typename Degeneration2Comparator>
  void _compute_discrete_operator_(const VariableTypeT & kappa,
                                   CellCoefficientArrayBuilder & cell_ca_builder,
                                   FaceCoefficientArrayBuilder & face_ca_builder,
                                   Degeneration1Comparator & degeneration1_comparator,
                                   Degeneration2Comparator & degeneration2_comparator);

  // Compute discrete operator. "AMR", N is the space dimension
  template<Integer N,
           typename VariableTypeT,
           typename CellCoefficientArrayBuilder, typename FaceCoefficientArrayBuilder,
           typename Degeneration1Comparator, typename Degeneration2Comparator>
  void _compute_discrete_operator_amr(const VariableTypeT & kappa,
                                      CellCoefficientArrayBuilder & cell_ca_builder,
                                      FaceCoefficientArrayBuilder & face_ca_builder,
                                      Degeneration1Comparator & degeneration1_comparator,
                                      Degeneration2Comparator & degeneration2_comparator);

};

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
template<Integer N, typename VariableTypeT>
void DivKGradLScheme<TCellStencilBuilder, TFaceStencilBuilder,
                     TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
formDiscreteOperatorT(const VariableTypeT & kappa)
{

  ImplementationType::template apply<N, ThisClass, VariableTypeT>(this, kappa);
  this->m_status |= DiscreteOperatorProperty::S_FORMED;
}

/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
template<typename VariableTypeT>
Byte DivKGradLScheme<TCellStencilBuilder, TFaceStencilBuilder,
                     TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
_compute_trans_centered_stencil(const Node& node, 
                                CellNum & NumLocCell,
                                const VariableTypeT & kappa,
                                L_Stencil& centered_stencil)
{
  typedef typename VariableTraitsT<VariableTypeT>::DataType DiffusionType;

  Integer N = 3;
  LUSolver<Real>::matrix_type A(N, N);
  LUSolver<Real>::matrix_type B(N, N+1);
  A.clear();
  B.clear();

  const Cell & K = node.cell(centered_stencil.getCell(0));
  const DiffusionType& LambdaK = kappa[K];
  Real3 xK = (*this->m_c_centers)[K];

  // Generate the system for the gradient recovery on the central cell
  for (Integer i = 0 ; i < N ; i++){
    const Face & face = node.face(centered_stencil.getFace(i));
    //internal face
    if (!face.isSubDomainBoundary()){
      const Cell& L = node.cell(centered_stencil.getCell(i+1));
      const DiffusionType& LambdaL = kappa[L];
      Real3 xL = (*this->m_c_centers)[L];
      Real3 xKL = xL - xK;
      Real3 normalK, normalL;
      //get the unit normals
      if (face.backCell() == K){
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
  LUSolver<Real>::matrix_type Grad(B);
  LUSolver<Real> lu;
  try
    {
      lu.factor(A);
      lu.overwrite_solve(Grad); //Grad=A^{-1}B
    }
  catch(LUSolver<Real>::Error e){//Exceptional case when the matrix is singular
    m_trace_mng->fatal() << e.msg;
    //     return 0;
  }
  // Check if the matrix is very close to singular
  Real det = lu.det();
  Real Max = 0.;
  for (std::size_t i = 0 ; i < A.size1() ; i++){
    for (std::size_t j = 0 ; j < A.size2() ; j++){
      Max = math::max(Max,math::abs(A(i,j)));
    }
  }
  //Scale the determinant
  det /= Max;
  if (math::abs(det)<1e-8){
    return 0;
  }

  // Compute Lambda*gradient
  SharedArray<Real3> LambdaGradient(4);
  for (Integer i = 0 ; i < 4 ; i++){
    Real3 column;
    column.x = Grad(0,i);
    column.y = Grad(1,i);
    column.z = Grad(2,i);
    LambdaGradient[i] = DiscreteOperator::tensor_vector_prod<DiffusionType>::eval(LambdaK,column);
  }

  for (Integer i = 0 ; i < N ; i++){
    const Face & face = node.face(centered_stencil.getFace(i));
    Real3 normal = (*this->m_f_normals)[face];
    if (face.isSubDomainBoundary() && !face.isSubDomainBoundaryOutside())
      normal = -normal;
    for (Integer j = 0 ; j < 4 ; j++){
      centered_stencil.transm[i][j] = math::scaMul(LambdaGradient[j], normal);
    }
  }
  return 1;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
template<typename VariableTypeT>
Byte DivKGradLScheme<TCellStencilBuilder, TFaceStencilBuilder,
                     TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
_compute_trans_centered_stencil_amr(const Node& node,
                                CellNum & NumLocCell,
                                ItemVectorT<Cell> node_cells,
                                const VariableTypeT & kappa,
                                L_Stencil& centered_stencil)
{
  typedef typename VariableTraitsT<VariableTypeT>::DataType DiffusionType;

  Integer N = 3;
  LUSolver<Real>::matrix_type A(N, N);
  LUSolver<Real>::matrix_type B(N, N+1);
  A.clear();
  B.clear();

  const Cell & K = node_cells[centered_stencil.getCell(0)];//node.cell(centered_stencil.getCell(0));
  const DiffusionType& LambdaK = kappa[K];
  Real3 xK = (*this->m_c_centers)[K];

  // Generate the system for the gradient recovery on the central cell
  for (Integer i = 0 ; i < N ; i++){
    const Face & face = node.face(centered_stencil.getFace(i));
    //internal face
    if (!face.isSubDomainBoundary()){
      const Cell& L = node_cells[centered_stencil.getCell(i+1)];//node.cell(centered_stencil.getCell(i+1));
      const DiffusionType& LambdaL = kappa[L];
      Real3 xL = (*this->m_c_centers)[L];
      Real3 xKL = xL - xK;
      Real3 normalK, normalL;
      //get the unit normals
  //    Real test =(*this->m_f_measures)[face];
      if (face.backCell() == K){
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
  LUSolver<Real>::matrix_type Grad(B);
  LUSolver<Real> lu;
  try
    {
      lu.factor(A);
      lu.overwrite_solve(Grad); //Grad=A^{-1}B
    }
  catch(LUSolver<Real>::Error e){//Exceptional case when the matrix is singular
    m_trace_mng->fatal() << e.msg;
    //     return 0;
  }
  // Check if the matrix is very close to singular
  Real det = lu.det();
  Real Max = 0.;
  for (std::size_t i = 0 ; i < A.size1() ; i++){
    for (std::size_t j = 0 ; j < A.size2() ; j++){
      Max = math::max(Max,math::abs(A(i,j)));
    }
  }
  //Scale the determinant
  det /= Max;
  if (math::abs(det)<1e-8){
    return 0;
  }

  // Compute Lambda*gradient
  SharedArray<Real3> LambdaGradient(4);
  for (Integer i = 0 ; i < 4 ; i++){
    Real3 column;
    column.x = Grad(0,i);
    column.y = Grad(1,i);
    column.z = Grad(2,i);
    LambdaGradient[i] = DiscreteOperator::tensor_vector_prod<DiffusionType>::eval(LambdaK,column);
  }

  for (Integer i = 0 ; i < N ; i++){
    const Face & face = node.face(centered_stencil.getFace(i));
    Real3 normal = (*this->m_f_normals)[face];
    if (face.isSubDomainBoundary() && !face.isSubDomainBoundaryOutside())
      normal = -normal;
    for (Integer j = 0 ; j < 4 ; j++){
      centered_stencil.transm[i][j] = math::scaMul(LambdaGradient[j], normal);
    }
  }
  return 1;
}

/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
template<Integer N,
         typename VariableTypeT, 
         typename CellCoefficientArrayBuilder, typename FaceCoefficientArrayBuilder,
         typename Degeneration1Comparator, typename Degeneration2Comparator>
void DivKGradLScheme<TCellStencilBuilder, TFaceStencilBuilder,
                     TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
_compute_discrete_operator(const VariableTypeT & kappa,
                           CellCoefficientArrayBuilder & cell_ca_builder,
                           FaceCoefficientArrayBuilder & face_ca_builder,
                           Degeneration1Comparator & degeneration1_comparator,
                           Degeneration2Comparator & degeneration2_comparator)
{

  if(this->m_amr)
    _compute_discrete_operator_amr< N, VariableTypeT, CellCoefficientArrayBuilder,  FaceCoefficientArrayBuilder,
                                      Degeneration1Comparator,  Degeneration2Comparator>
                                 (kappa, cell_ca_builder, face_ca_builder,
                                      degeneration1_comparator, degeneration2_comparator);
  else
  _compute_discrete_operator_< N, VariableTypeT, CellCoefficientArrayBuilder,  FaceCoefficientArrayBuilder,
                                     Degeneration1Comparator,  Degeneration2Comparator>
                              (kappa, cell_ca_builder, face_ca_builder,
                                     degeneration1_comparator, degeneration2_comparator);

}
/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
template<Integer N,
         typename VariableTypeT,
         typename CellCoefficientArrayBuilder, typename FaceCoefficientArrayBuilder,
         typename Degeneration1Comparator, typename Degeneration2Comparator>
void DivKGradLScheme<TCellStencilBuilder, TFaceStencilBuilder,
                     TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
_compute_discrete_operator_(const VariableTypeT & kappa,
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

  Integer MaxFaceId = this->m_mesh->faceFamily()->maxLocalId();

  // Internal nodes
  CellNum & NumLocCell = *(this->m_cell_numbering);

  VariableNodeReal3 coords = PRIMARYMESH_CAST(this->m_mesh)->nodesCoordinates();
  SharedArray<Integer> non_sing_stenc(MaxFaceId);
  non_sing_stenc.fill(0);
  SharedArray<Integer> non_boun_nodes(MaxFaceId);
  non_boun_nodes.fill(0);

  ENUMERATE_NODE( inode, this->m_internal_nodes ) {
    const Node& node = *inode;

    //Allocation du vecteur
    FaceNumByCell NumLocfacesbyCell;
    NumLocfacesbyCell.resize(node.nbCell());

    this->_compute_local_connectivity(node, NumLocCell, NumLocfacesbyCell);
    //Calculate the number of stencils
    Integer NumStencils = 0;
    for (Integer i = 0 ; i < node.nbCell() ; i++){
      if (NumLocfacesbyCell[i].size() == 3){
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
    SharedArray<Byte> stencil_non_degenerate(NumStencils);
    Integer ist = 0;
    for (Integer icell=0 ; icell<node.nbCell() ; ++icell){
      if (NumLocfacesbyCell[icell].size() == 3){
        centered_stencils[ist] = new L_Stencil(node, icell, NumLocCell, NumLocfacesbyCell);
        stencil_non_degenerate[ist] = _compute_trans_centered_stencil<VariableTypeT>(node, NumLocCell, kappa, *centered_stencils[ist]);
        ist++;
      }
      else{  // Case of pyramid vertex
        for (Integer iface = 0 ; iface < 4 ; iface++){
          centered_stencils[ist] = new L_Stencil(node, icell, iface, NumLocCell, NumLocfacesbyCell);
          stencil_non_degenerate[ist] = _compute_trans_centered_stencil<VariableTypeT>(node, NumLocCell,  kappa, *centered_stencils[ist]);
          ist++;
        }
      }
    }
    //Calculate the non-boundary nodes of each face (needed for scaling of the flux)
    for (Integer iface = 0; iface < node.nbFace(); iface++){
      const Face & face = node.face(iface);
      non_boun_nodes[face.localId()]++;
    }
    //Choose the best stencil for each face
    SharedArray<Integer> FaceToRow(node.nbFace());
    SharedArray<Integer> Stencil_Number(node.nbFace());
    SharedArray<Real> FaceIndicator(node.nbFace()); // the main parameter  (d1-d2) in choosing the proper stencil
    Stencil_Number.fill(NumStencils);

    for ( ist = 0 ; ist < NumStencils ; ++ist) { //loop over the stencils
      if (stencil_non_degenerate[ist]){
        for (Integer iface = 0; iface < 3 ; iface++){
          Real diff = math::abs(centered_stencils[ist]->transm[iface][0] - centered_stencils[ist]->transm[iface][iface+1]);
          Integer locnbface = centered_stencils[ist]->getFace(iface);
          if ((Stencil_Number[locnbface] == NumStencils)||(diff < FaceIndicator[locnbface])){
            Stencil_Number[locnbface] = ist;
            FaceIndicator[locnbface] = diff;
            FaceToRow[locnbface] = iface;
          }
        }
      }
    }
    // Retrieve the transmissibilities from the found stencils
    for (Integer iface = 0;  iface < node.nbFace() ; iface++){
      Integer stencil_num = Stencil_Number[iface];
      if (stencil_num != NumStencils){
        for (Integer icell = 0; icell < 4; icell++){
          cell_ca_builder.subtract(node.face(iface),
                                   node.cell(centered_stencils[stencil_num]->getCell(icell)),
                                   centered_stencils[stencil_num]->transm[FaceToRow[iface]][icell]);
        }
        non_sing_stenc[node.face(iface).localId()]++;
      }
    }
    for (Integer ist = 0 ; ist < NumStencils; ist++)
      delete centered_stencils[ist];
    delete[] centered_stencils;
  }

  if (this->m_type != TypesDivKGradOLGSchemes::BC_Lscheme){
    // Get the proper coefficients
    SharedArray<Real> weight_normalize(MaxFaceId);
    weight_normalize.fill(0.);
    ENUMERATE_FACE(iface, this->m_internal_faces){
      const Face & F = *iface;
      Integer nbnodes = F.nbNode();
      if (non_sing_stenc[F.localId()]){
        weight_normalize[F.localId()] = static_cast<Real>(non_boun_nodes[F.localId()]) / (nbnodes * non_sing_stenc[F.localId()]);
      }
      else
        weight_normalize[F.localId()] = 1.;
    }

    // Normalization of transmissibilities
    ENUMERATE_FACE(iface, this->m_internal_faces){
      const Face & F = *iface;
      ItemVectorView c_stencilF = this->m_cell_coefficients->stencil(F);
      Integer c_st_size = this->m_cell_coefficients->stencilSize(F);
      for (Integer i = 0; i < c_st_size ; i++){
        const Cell & cell = c_stencilF[i].toCell();
        //         this->m_cell_coefficients->coefficient(F, cell) *= weight_normalize[F.localId()];
        cell_ca_builder.multiply(F, cell, weight_normalize[F.localId()]);
      }
    }
  }

  // Boundary faces
  switch(this->m_type) {
  case TypesDivKGradOLGSchemes::BC_Lscheme : {
    ENUMERATE_NODE( inode, this->m_boundary_nodes ) {
      const Node& node = *inode;
      FaceNumByCell NumLocfacesbyCell;
      NumLocfacesbyCell.resize(node.nbCell());
      this->_compute_local_connectivity(node,NumLocCell,NumLocfacesbyCell);

      //Calculate the number of stencils
      Integer NumStencils = 0;
      for (Integer i = 0 ; i < node.nbCell() ; i++){
        if (NumLocfacesbyCell[i].size() == 3){
          NumStencils ++;
        }
        else{
          NumStencils += 4;
        }
      }

      //Initialize the centered stencils
      SharedArray<Byte> stencil_non_degenerate(NumStencils);
      L_Stencil **centered_stencils;
      centered_stencils = new L_Stencil*[NumStencils];

      //Compute the transmissibilities
      Integer ist = 0;
      for (Integer icell=0 ; icell<node.nbCell() ; ++icell){
        if (NumLocfacesbyCell[icell].size() == 3){
          centered_stencils[ist] = new L_Stencil(node, icell, NumLocCell, NumLocfacesbyCell);
          stencil_non_degenerate[ist] = _compute_trans_centered_stencil<VariableTypeT>(node, NumLocCell,  kappa, *centered_stencils[ist]);
          ist++;
        }
        else{  // Case of pyramid vertex
          for (Integer iface = 0 ; iface < 4 ; iface++){
            centered_stencils[ist] = new L_Stencil(node, icell, iface, NumLocCell, NumLocfacesbyCell);
            stencil_non_degenerate[ist] = _compute_trans_centered_stencil<VariableTypeT>(node, NumLocCell,  kappa, *centered_stencils[ist]);
            ist++;
          }
        }
      }

      //Choose the best stencil for each face
      SharedArray<Integer> FaceToRow(node.nbFace());
      SharedArray<Integer> Stencil_Number(node.nbFace());
      SharedArray<Real> FaceIndicator(node.nbFace()); // the main parameter  (d1-d2) in choosing the proper stencil
      Stencil_Number.fill(NumStencils);
      for ( ist = 0 ; ist < NumStencils ; ++ist) { //loop over the stencils
        if (stencil_non_degenerate[ist]){
          for (Integer iface = 0; iface < 3 ; iface++){
            Real diff = math::abs(centered_stencils[ist]->transm[iface][0] - centered_stencils[ist]->transm[iface][iface+1]);
            Integer locnbface = centered_stencils[ist]->getFace(iface);
            if ((Stencil_Number[locnbface] == NumStencils)||(diff < FaceIndicator[locnbface])){
              Stencil_Number[locnbface] = ist;
              FaceIndicator[locnbface] = diff;
              FaceToRow[locnbface] = iface;
            }
          }
        }
      }

      for (Integer iface = 0;  iface < node.nbFace() ; iface++){
        Integer stencil_num = Stencil_Number[iface];
        if (stencil_num != NumStencils){
          for (Integer icell = 0; icell < 4; icell++){
            Integer num_cell = centered_stencils[stencil_num]->getCell(icell);
            if (num_cell == -1){
              face_ca_builder.subtract(node.face(iface),
                                       node.face(centered_stencils[stencil_num]->getFace(icell-1)),
                                       centered_stencils[stencil_num]->transm[FaceToRow[iface]][icell]);
            }
            else{
              cell_ca_builder.subtract(node.face(iface),
                                       node.cell(centered_stencils[stencil_num]->getCell(icell)),
                                       centered_stencils[stencil_num]->transm[FaceToRow[iface]][icell]);
            }
          }
          non_sing_stenc[node.face(iface).localId()]++;
        }
      }
      for (Integer ist = 0 ; ist < NumStencils; ist++)
        delete centered_stencils[ist];
      delete[] centered_stencils;
    }

    // Normalization of transmissibilities
    ENUMERATE_FACE(iface, this->m_faces){
      const Face & F = *iface;
      ItemVectorView c_stencilF = this->m_cell_coefficients->stencil(F);
      Integer c_st_size = this->m_cell_coefficients->stencilSize(F);
      for (Integer i = 0; i < c_st_size ; i++){
        const Cell & cell = c_stencilF[i].toCell();
        cell_ca_builder.divide(F, cell, non_sing_stenc[F.localId()]);
      }
      Integer f_st_size = this->m_face_coefficients->stencilSize(F);
      if (f_st_size){
        ItemVectorView f_stencilF = this->m_face_coefficients->stencil(F);
        for (Integer i = 0; i < f_st_size ; i++){
          const Face & face = f_stencilF[i].toFace();
          face_ca_builder.divide(F, face, non_sing_stenc[F.localId()]);
        }
      }
    }
    break;
  }

  case TypesDivKGradOLGSchemes::BC_StandardTwoPoints : {// schema deux points pour les noeuds de bord
    ENUMERATE_NODE( inode, this->m_boundary_nodes ) {
      const Node& node = *inode;
      this->BaseClass::template _compute_standard_two_point_boundary_conditions<VariableTypeT,
        CellCoefficientArrayBuilder,
        FaceCoefficientArrayBuilder>(node, kappa, cell_ca_builder, face_ca_builder);
    }
    break;
  }
  case TypesDivKGradOLGSchemes::BC_TwoPoints : {// schema deux points pour les noeuds de bord
    ENUMERATE_NODE( inode, this->m_boundary_nodes ) {
      const Node& node = *inode;
      this->BaseClass::template _compute_two_point_boundary_conditions<VariableTypeT,
        CellCoefficientArrayBuilder,
        FaceCoefficientArrayBuilder>(node, kappa, cell_ca_builder, face_ca_builder);
    }
    break;
  }
  case TypesDivKGradOLGSchemes::BC_Oscheme : {//  O-Schema pour les noeuds de bord
    ENUMERATE_NODE( inode, this->m_boundary_nodes ) {
      const Node& node = *inode;

      //Allocation du vecteur
      FaceNumByCell NumLocfacesbyCell;
      NumLocfacesbyCell.resize(node.nbCell());

      // Pour chaque maille autour du noeud, on calcule Lambda Gradient
      SharedArray2<Real3> Gradient(node.nbCell(),node.nbFace()+1);

      this->_compute_local_connectivity(node, NumLocCell, NumLocfacesbyCell);
      try {
        TrueComparatorT<bool> true_comparator;
        this->BaseClass::template
          _compute_cell_gradient<3, TrueComparatorT<bool> >
          (node, NumLocfacesbyCell, Gradient, true_comparator) ;
        this->BaseClass::template _compute_o_scheme_boundary_conditions<VariableTypeT,
          CellCoefficientArrayBuilder,
          FaceCoefficientArrayBuilder>(node,
                                       NumLocCell, NumLocfacesbyCell, Gradient,
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

  //   this->m_boundary_nodes.clear();
  //   this->m_internal_nodes.clear();
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
template<Integer N,
         typename VariableTypeT,
         typename CellCoefficientArrayBuilder, typename FaceCoefficientArrayBuilder,
         typename Degeneration1Comparator, typename Degeneration2Comparator>
void DivKGradLScheme<TCellStencilBuilder, TFaceStencilBuilder,
                     TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
_compute_discrete_operator_amr(const VariableTypeT & kappa,
                               CellCoefficientArrayBuilder & cell_ca_builder,
                               FaceCoefficientArrayBuilder & face_ca_builder,
                               Degeneration1Comparator & degeneration1_comparator,
                               Degeneration2Comparator & degeneration2_comparator)
{
  ARCANE_ASSERT( (this->m_status & DiscreteOperatorProperty::S_PREPARED),
                 ("Operator not prepared") );


  // Retrieve geometric properties
  this->m_f_centers  = &this->m_geometry_service->getReal3VariableProperty(this->m_mesh->allActiveFaces(), IGeometryProperty::PCenter);
  this->m_f_normals  = &this->m_geometry_service->getReal3VariableProperty(this->m_mesh->allActiveFaces(), IGeometryProperty::PNormal);
  this->m_f_measures = &this->m_geometry_service->getRealVariableProperty(this->m_mesh->allActiveFaces(), IGeometryProperty::PMeasure);
  this->m_c_centers  = &this->m_geometry_service->getReal3VariableProperty(this->m_mesh->allActiveCells(), IGeometryProperty::PCenter);

  Integer MaxFaceId = this->m_mesh->faceFamily()->maxLocalId();

  // Internal nodes
  CellNum & NumLocCell = *(this->m_cell_numbering);

  VariableNodeReal3 coords = PRIMARYMESH_CAST(this->m_mesh)->nodesCoordinates();
  SharedArray<Integer> non_sing_stenc(MaxFaceId);
  non_sing_stenc.fill(0);
  SharedArray<Integer> non_boun_nodes(MaxFaceId);
  non_boun_nodes.fill(0);

  ENUMERATE_NODE( inode, this->m_internal_nodes ) {
    const Node& node = *inode;

    ItemVectorT<Cell> node_cells(this->m_mesh->cellFamily());
    ArcGeoSim::Mesh::amrCells(node, node_cells);

    //Allocation du vecteur
    FaceNumByCell NumLocfacesbyCell;
    NumLocfacesbyCell.resize(node_cells.size());//--

    this->_compute_local_connectivity_amr(node, node_cells, NumLocCell, NumLocfacesbyCell);
    //Calculate the number of stencils
    Integer NumStencils = 0;
    for (Integer i = 0 ; i < node_cells.size() ; i++){
      if (NumLocfacesbyCell[i].size() == 3){
        NumStencils ++;
      }
    }

    //Initialize the centered stencils
    L_Stencil **centered_stencils;
    centered_stencils = new L_Stencil*[NumStencils];
    //Compute the transmissibilities
    SharedArray<Byte> stencil_non_degenerate(NumStencils);
    Integer ist = 0;
    for (Integer icell=0 ; icell<node_cells.size(); ++icell){
      if (NumLocfacesbyCell[icell].size() == 3){
        centered_stencils[ist] = new L_Stencil(node, icell, node_cells, NumLocCell, NumLocfacesbyCell);
        stencil_non_degenerate[ist] = _compute_trans_centered_stencil_amr<VariableTypeT>(node, NumLocCell, node_cells, kappa, *centered_stencils[ist]);
        ist++;
      }
    }
    //Calculate the non-boundary nodes of each face (needed for scaling of the flux)
    for (Integer iface = 0; iface < node.nbFace(); iface++){
      const Face & face = node.face(iface);
      non_boun_nodes[face.localId()]++;
    }
    //Choose the best stencil for each face
    SharedArray<Integer> FaceToRow(node.nbFace());
    SharedArray<Integer> Stencil_Number(node.nbFace());
    SharedArray<Real> FaceIndicator(node.nbFace()); // the main parameter  (d1-d2) in choosing the proper stencil
    Stencil_Number.fill(NumStencils);

    for ( ist = 0 ; ist < NumStencils ; ++ist) { //loop over the stencils
      if (stencil_non_degenerate[ist]){
        for (Integer iface = 0; iface < 3 ; iface++){
          Real diff = math::abs(centered_stencils[ist]->transm[iface][0] - centered_stencils[ist]->transm[iface][iface+1]);
          Integer locnbface = centered_stencils[ist]->getFace(iface);
          if ((Stencil_Number[locnbface] == NumStencils)||(diff < FaceIndicator[locnbface])){
            Stencil_Number[locnbface] = ist;
            FaceIndicator[locnbface] = diff;
            FaceToRow[locnbface] = iface;
          }
        }
      }
    }

    // Retrieve the transmissibilities from the found stencils
    for (Integer iface = 0;  iface < node.nbFace() ; iface++){
      Integer stencil_num = Stencil_Number[iface];
      if (stencil_num != NumStencils){
        for (Integer icell = 0; icell < 4; icell++){
          if(ArcGeoSim::Mesh::isActive(node.face(iface)))
          cell_ca_builder.subtract(node.face(iface),
                                   node_cells[centered_stencils[stencil_num]->getCell(icell)],
                                   centered_stencils[stencil_num]->transm[FaceToRow[iface]][icell]);
        }
        non_sing_stenc[node.face(iface).localId()]++;
      }
    }
    for (Integer ist = 0 ; ist < NumStencils; ist++)
      delete centered_stencils[ist];
    delete[] centered_stencils;
  }
/*
  if (this->m_type != TypesDivKGradOLGSchemes::BC_Lscheme){
    // Get the proper coefficients
    Array<Real> weight_normalize(MaxFaceId);
    weight_normalize.fill(0.);
    ENUMERATE_FACE(iface, this->m_internal_faces){
      const Face & F = *iface;
      Integer nbnodes = F.nbNode();
      if (non_sing_stenc[F.localId()]){
        weight_normalize[F.localId()] = static_cast<Real>(non_boun_nodes[F.localId()]) / (nbnodes * non_sing_stenc[F.localId()]);
      }
      else
        weight_normalize[F.localId()] = 1.;
    }

    // Normalization of transmissibilities
    ENUMERATE_FACE(iface, this->m_internal_faces){
      const Face & F = *iface;
      ItemVectorView c_stencilF = this->m_cell_coefficients->stencil(F);
      Integer c_st_size = this->m_cell_coefficients->stencilSize(F);
      for (Integer i = 0; i < c_st_size ; i++){
        const Cell & cell = c_stencilF[i].toCell();
        //this->m_cell_coefficients->coefficient(F, cell) *= weight_normalize[F.localId()];
      //std::cout<<F.localId()<<" MULTIPLY"<<cell.localId()<<"\n";
      //  if(ArcGeoSim::Mesh::isActive(F))
      //    cell_ca_builder.multiply(F, cell, weight_normalize[F.localId()]);
      }
    }
  }
*/
  // Boundary faces
  switch(this->m_type) {

  case TypesDivKGradOLGSchemes::BC_StandardTwoPoints : {// schema deux points pour les noeuds de bord
    ENUMERATE_NODE( inode, this->m_boundary_nodes ) {
      const Node& node = *inode;
      //std::cout<<"BC_StandardTwoPoints S.Y."<<"\n";
      this->BaseClass::template _compute_standard_two_point_boundary_conditions<VariableTypeT,
        CellCoefficientArrayBuilder,
        FaceCoefficientArrayBuilder>(node, kappa, cell_ca_builder, face_ca_builder);
    }
    break;
  }
  case TypesDivKGradOLGSchemes::BC_TwoPoints : {// schema deux points pour les noeuds de bord
    ENUMERATE_NODE( inode, this->m_boundary_nodes ) {
      const Node& node = *inode;
      this->BaseClass::template _compute_two_point_boundary_conditions<VariableTypeT,
        CellCoefficientArrayBuilder,
        FaceCoefficientArrayBuilder>(node, kappa, cell_ca_builder, face_ca_builder);
    }
    break;
  }
  default:
    m_trace_mng->error() << "Unknown boundary condition type : only TP is accepted for AMR";

  }

  //   this->m_boundary_nodes.clear();
  //   this->m_internal_nodes.clear();
}

/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#endif
