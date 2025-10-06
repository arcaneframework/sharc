// -*- C++ -*-
#ifndef DIVKGRADOSCHEME_H
#define DIVKGRADOSCHEME_H

#include <arcane/ArcaneVersion.h>
#include <boost/shared_ptr.hpp>
#include <arcane/utils/ITraceMng.h>

#include "ArcGeoSim/Numerics/DiscreteOperator/CoefficientArrayBuilder.h"
#include "ArcGeoSim/Numerics/DiscreteOperator/Comparator.h"
#include "ArcGeoSim/Numerics/DiscreteOperator/IDivKGradOLGSchemes.h"

#include "ArcGeoSim/Numerics/DiscreteOperator/OperatorImplementation.h"

#include "ArcGeoSim/Numerics/DiscreteOperator/TwoPointInterpolator.h"

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
class DivKGradOScheme
  : public IDivKGradOLGSchemes<TCellStencilBuilder, TFaceStencilBuilder,
                               TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>
{
public:
  typedef DivKGradOScheme<TCellStencilBuilder, TFaceStencilBuilder,
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
  DivKGradOScheme(ITraceMng * a_trace_mng)
    : m_trace_mng(a_trace_mng)
  {
    // Do nothing
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

  // Enforce flux conservation
#if (ARCANE_VERSION >= 12201)
  template<typename VariableTypeT, typename CellCoefficientArrayBuilder, typename DegenerationComparator>
  Integer _enforce_flux_conservation(const Node& node,
  		CellNum & NumLocCell,
  		FaceNumByCell& NumLocfacebyCell,
  		Array2<Real3>& kappa_gradient,
  		const VariableTypeT & kappa,
  		CellCoefficientArrayBuilder & cell_ca_builder,
  		DegenerationComparator & degeneration_comparator);
#else /* ARCANE_VERSION */
  template<typename VariableTypeT, typename CellCoefficientArrayBuilder, typename DegenerationComparator>
  Integer _enforce_flux_conservation(const Node& node,
  		CellNum & NumLocCell,
  		FaceNumByCell& NumLocfacebyCell,
  		CArray2T<Real3> & kappa_gradient,
  		const VariableTypeT & kappa,
  		CellCoefficientArrayBuilder & cell_ca_builder,
  		DegenerationComparator & degeneration_comparator);
#endif /* ARCANE_VERSION */

  // Compute discrete operator. N is here the space dimension
  template<Integer N,
           typename VariableTypeT, 
           typename CellCoefficientArrayBuilder, typename FaceCoefficientArrayBuilder,
           typename Degeneration1Comparator, typename Degeneration2Comparator>
  void _compute_discrete_operator(const VariableTypeT & kappa,
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
void DivKGradOScheme<TCellStencilBuilder, TFaceStencilBuilder,
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
void DivKGradOScheme<TCellStencilBuilder, TFaceStencilBuilder,
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

  Integer n_deg_1 = 0; // Number of nodes for which gradient correction is disabled
  Integer n_deg_2 = 0; // Number of nodes for which two-point method is used

  // Internal nodes
  ENUMERATE_NODE(inode, this->m_internal_nodes) {
    const Node& node = *inode;
    //Allocation du vecteur
    FaceNumByCell NumLocfacesbyCell;
    NumLocfacesbyCell.resize(node.nbCell());
    // Pour chaque maille autour du noeud, on calcule Lambda Gradient
    SharedArray2<Real3> Gradient(node.nbCell(),node.nbFace()+1);

    this->_compute_local_connectivity(node, *(this->m_cell_numbering), NumLocfacesbyCell);
    try {
      this->BaseClass::template
        _compute_cell_gradient<N, Degeneration1Comparator>
        (node, NumLocfacesbyCell, Gradient, degeneration1_comparator);
    }
    catch(typename BaseClass::Error e) {
      switch(e.code) {
      case BaseClass::E_CellGradientComputation:
        m_trace_mng->debug() << "Error in local gradient computation: "
                             << e.msg
                             << ". Gradient correction disabled";
        n_deg_1++;
        break;
      case BaseClass::E_InconsistentConstruction:
        throw(typename BaseClass::Error("Inconsistent construction", BaseClass::E_InconsistentConstruction));
        break;
      default:
        m_trace_mng->fatal() << "Fatal error in local gradient computation: "
                             << e.msg;
      }
    }

    try {
      n_deg_2 += 
        _enforce_flux_conservation<VariableTypeT, CellCoefficientArrayBuilder, Degeneration2Comparator>
        (node, *(this->m_cell_numbering), NumLocfacesbyCell, Gradient,
         kappa, 
         cell_ca_builder, degeneration2_comparator);
    }
    catch(typename BaseClass::Error e) {
      switch(e.code) {
      case BaseClass::E_InconsistentConstruction:
        throw(typename BaseClass::Error("Inconsistent construction", BaseClass::E_InconsistentConstruction));
        break;
      default:
        m_trace_mng->fatal() << "Fatal error in flux conservation: "
                             << e.msg;
      }
    }
  }

  // Boundary faces
  switch(this->m_type) {
  case TypesDivKGradOLGSchemes::BC_StandardTwoPoints : { // two-point method for boundary nodes
    ENUMERATE_NODE(inode, this->m_boundary_nodes) {
      const Node& node = *inode;
      this->BaseClass::template _compute_standard_two_point_boundary_conditions<VariableTypeT,
        CellCoefficientArrayBuilder,
        FaceCoefficientArrayBuilder>(node, kappa, cell_ca_builder, face_ca_builder);
    }
    break;
  }
  case TypesDivKGradOLGSchemes::BC_TwoPoints : { // two-point method for boundary nodes
    ENUMERATE_NODE(inode, this->m_boundary_nodes) {
      const Node& node = *inode;
      this->BaseClass::template _compute_two_point_boundary_conditions<VariableTypeT,
        CellCoefficientArrayBuilder,
        FaceCoefficientArrayBuilder>(node, kappa, cell_ca_builder, face_ca_builder);
    }
    break;
  }
  case TypesDivKGradOLGSchemes::BC_Oscheme : {   // O-method for boundary nodes

    ENUMERATE_NODE(inode, this->m_boundary_nodes) {
      const Node& node = *inode;

      //Allocation du vecteur
      FaceNumByCell NumLocfacesbyCell;
      NumLocfacesbyCell.resize(node.nbCell());

      SharedArray2<Real3> gradient(node.nbCell(),node.nbFace()+1);

      this->_compute_local_connectivity(node, *(this->m_cell_numbering), NumLocfacesbyCell);
      try {
        this->BaseClass::template
          _compute_cell_gradient<N, Degeneration1Comparator>
          (node, NumLocfacesbyCell, gradient, degeneration1_comparator) ;
      }
      catch (typename BaseClass::Error e) {
        switch(e.code) {
        case BaseClass::E_CellGradientComputation:
          m_trace_mng->debug() << "Error in local gradient computation: "
                               << e.msg
                               << ". Gradient correction disabled";
          n_deg_1++;
          break;
        case BaseClass::E_InconsistentConstruction:
          throw(typename BaseClass::Error("Inconsistent construction", BaseClass::E_InconsistentConstruction));
          break;
        default:
          m_trace_mng->fatal() << "Fatal error in flux conservation: "
                               << e.msg;
        }
      }
      try {
        this->BaseClass::template _compute_o_scheme_boundary_conditions<VariableTypeT,
          CellCoefficientArrayBuilder,
          FaceCoefficientArrayBuilder>(node, *(this->m_cell_numbering), NumLocfacesbyCell, gradient, kappa, cell_ca_builder, face_ca_builder);
      }
      catch(typename BaseClass::Error e) {
        m_trace_mng->fatal() << "Fatal error in boundary computation: "
                             << e.msg;
      }
    }
    break;
  }
  default:
    m_trace_mng->fatal() << "Unknown boundary condition type";
  }

  m_trace_mng->info() << "Gradient correction disabled in " << n_deg_1 << " nodes";
  m_trace_mng->info() << "Two-point method used in " << n_deg_2 << " nodes";

  // Enhance monotonicity by switching faces with negative diagonal
  // coefficients to two-point
  if ((this->m_check_sign)or not (this->m_dtodr<0))
    {
      Integer n_face_2pts = 0;
      ENUMERATE_FACE(iface, this->m_internal_faces)
        {
          const Face & face = *iface;

          const Cell & K = face.backCell();
          const Cell & L = face.frontCell();

          bool force_two_pts = false;

          // Retrieve the transmissivities for back and front cell
          Real tauK = cell_ca_builder.coefficient(face, K);
          Real tauL = cell_ca_builder.coefficient(face, L);

          if(tauK <= 0 || tauL >= 0)
	    force_two_pts = true;
          else
            {  
              //on recupere tous les coeffs 
             ArrayView<Integer> cell_stencil_face = this->m_cell_coefficients->stencilLocalId(face);
             ArrayView<Real> cell_coefficients_face = this->m_cell_coefficients->coefficients(face);

             //coeff hors diag negatif minimum et positif maximum
             const Integer L_lid = L.localId();
             const Integer K_lid = K.localId();
             Real min_offdiag_neg = 0;
             Real max_offdiag_pos = 0;
             for (Integer i=0;i<cell_stencil_face.size();++i)
               {
                 const Real coeff = cell_coefficients_face[i];
                 const Integer lid = cell_stencil_face[i];
                 if (not(lid == L_lid) and not(lid== K_lid) )
                   {
		     min_offdiag_neg = math::min(coeff,min_offdiag_neg);
                     max_offdiag_pos = math::max(coeff,max_offdiag_pos);
                   }
                }
             //tauK >0, offK <=0 ok, max_offdiag_pos a controler 
             if (not (this->m_dtodr * max_offdiag_pos<tauK))
               force_two_pts = true;
             //tauL <0, offL >=0 ok, min_offdiag_neg a controler
             if (not (this->m_dtodr * min_offdiag_neg>tauL))
	       force_two_pts = true;
	    }

        if(force_two_pts)
          {
	    n_face_2pts += 1;
            // Set all coefficients to zero
            cell_ca_builder.reset(face);
            face_ca_builder.reset(face);

            // Compute transmissivity
            Real tau = 0.;
            switch(this->m_type) {          
            case TypesDivKGradOLGSchemes::BC_StandardTwoPoints:
              tau = TwoPointInterpolator::standardTwoPointInternalTransmissivity(face, 
                                                                               kappa, 
                                                                               this->m_f_centers, 
                                                                               this->m_f_normals, 
                                                                               this->m_f_measures, 
                                                                               this->m_c_centers);
              break;
            default:
              tau = TwoPointInterpolator::twoPointInternalTransmissivity(face, 
                                                                       kappa, 
                                                                       this->m_f_centers, 
                                                                       this->m_f_normals, 
                                                                       this->m_f_measures, 
                                                                       this->m_c_centers);
              break;
          } // END case

        // Store flux
        cell_ca_builder.sum(face, K, tau);
        cell_ca_builder.subtract(face, L, tau);
      }
    }
    m_trace_mng->info() << "Two-point method used in  " << n_face_2pts << " out of "<<this->m_internal_faces.size() <<" faces";
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#if (ARCANE_VERSION >= 12201)
template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
template <typename VariableTypeT, typename CellCoefficientArrayBuilder, typename DegenerationComparator>
Integer DivKGradOScheme<TCellStencilBuilder, TFaceStencilBuilder,
TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
_enforce_flux_conservation(const Node& node, CellNum & NumLocCell, FaceNumByCell& NumLocfacesbyCell,
		Array2<Real3> & gradient,
		const VariableTypeT & kappa,
		CellCoefficientArrayBuilder & cell_ca_builder,
		DegenerationComparator & degeneration_comparator)
		{
	typedef typename VariableTraitsT<VariableTypeT>::DataType DiffusionType;

	Integer nbface = node.nbFace();
	Integer nbcell = node.nbCell();
	Integer N = this->m_mesh->dimension(); // Space dimension

	typename BaseClass::RealMatrix A(nbface, nbface); A.clear();
	typename BaseClass::RealMatrix B(nbface, nbcell); B.clear();

	//Multiply gradients by Lambda
	SharedArray2<Real3> kappa_gradient(node.nbCell(),node.nbFace()+1);
	for (Integer i = 0 ; i < node.nbCell() ; i++){
		const Cell & T = node.cell(i);
		const DiffusionType & kappaT = kappa[T];
		for (Integer j = 0 ; j < node.nbFace()+1; j++){
			kappa_gradient[i][j] = DiscreteOperator::tensor_vector_prod<DiffusionType>::eval(kappaT,gradient[i][j]);
		}
	}

	// Find maximum number of faces
	UInt32 max_nb_local_faces=0;
	for (Integer i=0; i<nbcell ; i++) {
		const FaceNum & face_local = NumLocfacesbyCell[i];
		if(face_local.size()>max_nb_local_faces)
			max_nb_local_faces = face_local.size();
	}
	// Initialize multi array for fluxes
	boost::multi_array<Real, 3> Fluxes(boost::extents[nbcell][nbface][max_nb_local_faces+1]);

	//Build the fluxes
	for (Integer iK = 0 ; iK < nbcell ; iK++){
		const Cell & K = node.cell(iK);
		Integer iK_loc = NumLocCell[K];
		FaceNum & face_local = NumLocfacesbyCell[iK_loc];
		Integer nbfaceK =  face_local.size();

		// Main part of the flux
		for (Integer iF = 0 ; iF < nbfaceK ; ++iF){  // Loop over the faces of cell sharing the node (rows of the matrix)
			Integer iF_loc = face_local[iF];
			const Face & F = node.face(iF_loc);
			const Real3 & nF = (*this->m_f_normals)[node.face(iF_loc)];
			for (Integer jF = 0 ; jF <= nbfaceK ; jF++)
				Fluxes[iK][iF_loc][jF]  = // We divide by the number of nodes
						-math::scaMul(kappa_gradient[iK_loc][jF], nF) / F.nbNode();
		}

		// Add the residual terms for the cells with more than 3 (in 3D) faces sharing a node
		if (nbfaceK != N){
			// Compute residuals
		  SharedArray2<Real> Resid(nbfaceK, nbfaceK+1);
			SharedArray<Real3> dK_sigma(nbfaceK);
			const Real3 xK = (*this->m_c_centers)[K];
			Resid.fill(0.);
			for (Integer iF = 0 ; iF < nbfaceK ; iF++){
				Integer iF_loc = face_local[iF];
				const Face & F = node.face(iF_loc);
				dK_sigma[iF] = ((*this->m_f_centers)[F] - xK);
			}

			for (Integer iF = 0 ; iF < nbfaceK ; iF++){
				Resid[iF][iF] =1.;
				Resid[iF][nbfaceK] = -1.;
				for (Integer jF = 0 ; jF <= nbfaceK ; jF++){
					Resid[iF][jF] -= math::scaMul(gradient[iK_loc][jF] , dK_sigma[iF]);
				}
			}

			// Compute the term involving the sum of residuals
			SharedArray<Real3> sum(nbfaceK+1);
			sum.fill(Real3(0.,0.,0.));
			for (Integer iF = 0 ; iF <= nbfaceK ; iF++)
				for (Integer jF = 0 ; jF < nbfaceK ; jF++)
					sum[iF] += Resid[jF][iF] / dK_sigma[jF].abs2() *  dK_sigma[jF];

			// Additional terms
			const DiffusionType & kappaK = kappa[K];
			Real alpha = DiscreteOperator::MinimumEigenvalue::compute(kappaK);
			Real m_Ks = 0;
			for (Integer i = 0 ; i < nbfaceK ; i++){
				Integer numloc_face = face_local[i];
				const Face & face = node.face(numloc_face);
				m_Ks += (*this->m_f_measures)[face] / face.nbNode() * dK_sigma[i].abs();
			}
			m_Ks /= N;
			Real coef = alpha * m_Ks;
			for (Integer i = 0 ; i < nbfaceK ; i++){
				Integer numloc_face = face_local[i];
				const Face & face = node.face(numloc_face);
				for (Integer j = 0 ; j <= nbfaceK ; j++)
					Fluxes[iK][numloc_face][j] -=
							coef*(Resid[i][j]/dK_sigma[i].abs2()-math::scaMul(gradient[iK_loc][i],sum[j]))/face.nbNode();
			}
		}
	}

	// Construct local matrices to enforce flux continuity
	for(Integer iF = 0 ; iF < nbface ; ++iF){
		const Face& face  = node.face(iF);
		const Cell& K  = face.backCell();
		const Cell& L  = face.frontCell();
		// Local cell numbering around one node
		Integer iK_loc   = NumLocCell[K];
		Integer iL_loc   = NumLocCell[L];
		// Local face numbering
		FaceNum & iF_loc_K = NumLocfacesbyCell[iK_loc];
		FaceNum & iF_loc_L = NumLocfacesbyCell[iL_loc];
		Integer nbfaceK =  iF_loc_K.size();
		Integer nbfaceL =  iF_loc_L.size();

		for(Integer jF = 0 ; jF < nbfaceK ; ++jF){
			A(iF, iF_loc_K[jF]) += Fluxes[iK_loc][iF][jF];
		}

		for(Integer jF = 0 ; jF < nbfaceL ; ++jF){
			A(iF, iF_loc_L[jF]) -= Fluxes[iL_loc][iF][jF];
		}

		B(iF,iK_loc) = -Fluxes[iK_loc][iF][nbfaceK];
		B(iF,iL_loc) = Fluxes[iL_loc][iF][nbfaceL];
	}

	bool switch_to_two_point = false;
	{
		typename BaseClass::PermutationMatrix P(nbface);
		Integer success = Numerics::lu_factorize(A, P, this->m_epsilon);

		if(success != 0) {
			m_trace_mng->debug() << "O-stencil leading to a singular matrix for node "
					<< node.localId()
					<< ". falling back to two-points scheme";
#ifdef ARCANE_DEBUG
			(*this->m_degenerate_nodes)[node] = 1;
#endif /* ARCANE_DEBUG */
			switch_to_two_point = true;
		} else {
			lu_substitute(A, P, B);
		}

		if(!degeneration_comparator.compare(switch_to_two_point))
			throw(typename BaseClass::Error("Inconsistent construction in flux conservation",
					BaseClass::E_InconsistentConstruction));
	}

	// Compute fluxes for each face sharing the node
	if(switch_to_two_point) {
		// Loop over the faces sharing the node
		for(Integer iF=0; iF<node.nbFace(); ++iF) {
			// Internal nodes only => F is an internal face
			const Face& F = node.face(iF);
			// Retrieve back and front cell
			const Cell & K = F.backCell();
			const Cell & L = F.frontCell();

			// Compute transmissivity
			Real tau = 0.;
			switch(this->m_type) {
			case TypesDivKGradOLGSchemes::BC_StandardTwoPoints:
				tau = TwoPointInterpolator::standardTwoPointInternalTransmissivity(F,
						kappa,
						this->m_f_centers,
						this->m_f_normals,
						this->m_f_measures,
						this->m_c_centers);
				break;
			default:
				tau = TwoPointInterpolator::twoPointInternalTransmissivity(F,
						kappa,
						this->m_f_centers,
						this->m_f_normals,
						this->m_f_measures,
						this->m_c_centers);
				break;
			} // END case

			//Reduce to sub face flux
			tau /= F.nbNode();
			// Store flux
			cell_ca_builder.sum(F, K, tau);
			cell_ca_builder.subtract(F, L, tau);
		} // END for
	} else {
		for(Integer iF = 0; iF < node.nbFace(); ++iF){
			const Face & F = node.face(iF);
			const Cell& K  = F.backCell();
			//numero locale de la maille autour du noeud
			Integer iK_loc   = NumLocCell[K];
			FaceNum & face_local = NumLocfacesbyCell[iK_loc];
			Integer nbfaceK =  face_local.size();

			// Update flux through F
			for (Integer jF = 0 ; jF < nbfaceK ; ++jF) {
				for(Integer iL = 0 ; iL < nbcell ; ++iL) {
					const Cell& L = node.cell(iL);
					// Add the contribution from L
					cell_ca_builder.sum(F, L, Fluxes[iK_loc][iF][jF]*B(face_local[jF], iL));
				}
			}
			cell_ca_builder.sum(F, K, Fluxes[iK_loc][iF][nbfaceK]);
		}
	}

	return (Integer)switch_to_two_point;
		}
#else /* ARCANE_VERSION */
template<typename TCellStencilBuilder, typename TFaceStencilBuilder,
         typename TCellCoefficientArrayBuilder, typename TFaceCoefficientArrayBuilder>
template <typename VariableTypeT, typename CellCoefficientArrayBuilder, typename DegenerationComparator>
Integer DivKGradOScheme<TCellStencilBuilder, TFaceStencilBuilder,
                        TCellCoefficientArrayBuilder, TFaceCoefficientArrayBuilder>::
_enforce_flux_conservation(const Node& node, CellNum & NumLocCell, FaceNumByCell& NumLocfacesbyCell,
                           CArray2T<Real3> & gradient, 
                           const VariableTypeT & kappa,
                           CellCoefficientArrayBuilder & cell_ca_builder,
                           DegenerationComparator & degeneration_comparator)
{
  typedef typename VariableTraitsT<VariableTypeT>::DataType DiffusionType;

  Integer nbface = node.nbFace();
  Integer nbcell = node.nbCell();
  Integer N = this->m_mesh->dimension(); // Space dimension

  typename BaseClass::RealMatrix A(nbface, nbface); A.clear();
  typename BaseClass::RealMatrix B(nbface, nbcell); B.clear();

  //Multiply gradients by Lambda
  CArray2T<Real3> kappa_gradient(node.nbCell(),node.nbFace()+1);
  for (Integer i = 0 ; i < node.nbCell() ; i++){
    const Cell & T = node.cell(i);
    const DiffusionType & kappaT = kappa[T];
    for (Integer j = 0 ; j < node.nbFace()+1; j++){
      kappa_gradient[i][j] = DiscreteOperator::tensor_vector_prod<DiffusionType>::eval(kappaT,gradient[i][j]);
    }
  }

  // Find maximum number of faces
  UInt32 max_nb_local_faces=0;
  for (Integer i=0; i<nbcell ; i++) {
    const FaceNum & face_local = NumLocfacesbyCell[i];
    if(face_local.size()>max_nb_local_faces)
      max_nb_local_faces = face_local.size();
  }
  // Initialize multi array for fluxes
  boost::multi_array<Real, 3> Fluxes(boost::extents[nbcell][nbface][max_nb_local_faces+1]);

  //Build the fluxes
  for (Integer iK = 0 ; iK < nbcell ; iK++){
    const Cell & K = node.cell(iK);
    Integer iK_loc = NumLocCell[K];
    FaceNum & face_local = NumLocfacesbyCell[iK_loc];
    Integer nbfaceK =  face_local.size();

    // Main part of the flux
    for (Integer iF = 0 ; iF < nbfaceK ; ++iF){  // Loop over the faces of cell sharing the node (rows of the matrix)
      Integer iF_loc = face_local[iF];
      const Face & F = node.face(iF_loc);
      const Real3 & nF = (*this->m_f_normals)[node.face(iF_loc)];
      for (Integer jF = 0 ; jF <= nbfaceK ; jF++)
        Fluxes[iK][iF_loc][jF]  = // We divide by the number of nodes
          -math::scaMul(kappa_gradient[iK_loc][jF], nF) / F.nbNode();
    }

    // Add the residual terms for the cells with more than 3 (in 3D) faces sharing a node
    if (nbfaceK != N){
      // Compute residuals
      CArray2T<Real> Resid(nbfaceK, nbfaceK+1);
      SharedArray<Real3> dK_sigma(nbfaceK);
      const Real3 xK = (*this->m_c_centers)[K];
      Resid.fill(0.);
      for (Integer iF = 0 ; iF < nbfaceK ; iF++){
        Integer iF_loc = face_local[iF];
        const Face & F = node.face(iF_loc);
        dK_sigma[iF] = ((*this->m_f_centers)[F] - xK);
      }

      for (Integer iF = 0 ; iF < nbfaceK ; iF++){
        Resid[iF][iF] =1.;
        Resid[iF][nbfaceK] = -1.;
        for (Integer jF = 0 ; jF <= nbfaceK ; jF++){
          Resid[iF][jF] -= math::scaMul(gradient[iK_loc][jF] , dK_sigma[iF]);
        }
      }

      // Compute the term involving the sum of residuals
      SharedArray<Real3> sum(nbfaceK+1);
      sum.fill(Real3(0.,0.,0.));
      for (Integer iF = 0 ; iF <= nbfaceK ; iF++)
        for (Integer jF = 0 ; jF < nbfaceK ; jF++)
          sum[iF] += Resid[jF][iF] / dK_sigma[jF].abs2() *  dK_sigma[jF];

      // Additional terms
      const DiffusionType & kappaK = kappa[K];
      Real alpha = DiscreteOperator::MinimumEigenvalue::compute(kappaK);
      Real m_Ks = 0;
      for (Integer i = 0 ; i < nbfaceK ; i++){
        Integer numloc_face = face_local[i];
        const Face & face = node.face(numloc_face);
        m_Ks += (*this->m_f_measures)[face] / face.nbNode() * dK_sigma[i].abs();
      }
      m_Ks /= N;
      Real coef = alpha * m_Ks;
      for (Integer i = 0 ; i < nbfaceK ; i++){
        Integer numloc_face = face_local[i];
        const Face & face = node.face(numloc_face);
        for (Integer j = 0 ; j <= nbfaceK ; j++)
          Fluxes[iK][numloc_face][j] -= 
            coef*(Resid[i][j]/dK_sigma[i].abs2()-math::scaMul(gradient[iK_loc][i],sum[j]))/face.nbNode();
      }
    }
  }

  // Construct local matrices to enforce flux continuity
  for(Integer iF = 0 ; iF < nbface ; ++iF){
    const Face& face  = node.face(iF);
    const Cell& K  = face.backCell();
    const Cell& L  = face.frontCell();
    // Local cell numbering around one node
    Integer iK_loc   = NumLocCell[K];
    Integer iL_loc   = NumLocCell[L];
    // Local face numbering 
    FaceNum & iF_loc_K = NumLocfacesbyCell[iK_loc];
    FaceNum & iF_loc_L = NumLocfacesbyCell[iL_loc];
    Integer nbfaceK =  iF_loc_K.size();
    Integer nbfaceL =  iF_loc_L.size();

    for(Integer jF = 0 ; jF < nbfaceK ; ++jF){
      A(iF, iF_loc_K[jF]) += Fluxes[iK_loc][iF][jF];
    }

    for(Integer jF = 0 ; jF < nbfaceL ; ++jF){
      A(iF, iF_loc_L[jF]) -= Fluxes[iL_loc][iF][jF];
    }

    B(iF,iK_loc) = -Fluxes[iK_loc][iF][nbfaceK];
    B(iF,iL_loc) = Fluxes[iL_loc][iF][nbfaceL];
  }

  bool switch_to_two_point = false;
  {
    typename BaseClass::PermutationMatrix P(nbface);
    Integer success = Numerics::lu_factorize(A, P, this->m_epsilon);

    if(success != 0) {
      m_trace_mng->debug() << "O-stencil leading to a singular matrix for node "
                           << node.localId()
                           << ". falling back to two-points scheme";
#ifdef ARCANE_DEBUG
      (*this->m_degenerate_nodes)[node] = 1;
#endif /* ARCANE_DEBUG */
      switch_to_two_point = true;
    } else {
      lu_substitute(A, P, B);
    }

    if(!degeneration_comparator.compare(switch_to_two_point))
      throw(typename BaseClass::Error("Inconsistent construction in flux conservation", 
                                      BaseClass::E_InconsistentConstruction));
  }

  // Compute fluxes for each face sharing the node
  if(switch_to_two_point) {
    // Loop over the faces sharing the node
    for(Integer iF=0; iF<node.nbFace(); ++iF) {
      // Internal nodes only => F is an internal face
      const Face& F = node.face(iF);
      // Retrieve back and front cell
      const Cell & K = F.backCell();
      const Cell & L = F.frontCell();

      // Compute transmissivity
      Real tau = 0.;
      switch(this->m_type) {
      case TypesDivKGradOLGSchemes::BC_StandardTwoPoints:
        tau = TwoPointInterpolator::standardTwoPointInternalTransmissivity(F, 
                                                                   kappa, 
                                                                   this->m_f_centers, 
                                                                   this->m_f_normals, 
                                                                   this->m_f_measures, 
                                                                   this->m_c_centers);
        break;
      default:
        tau = TwoPointInterpolator::twoPointInternalTransmissivity(F, 
                                                                           kappa, 
                                                                           this->m_f_centers, 
                                                                           this->m_f_normals, 
                                                                           this->m_f_measures, 
                                                                           this->m_c_centers);
        break;
      } // END case

      //Reduce to sub face flux
      tau /= F.nbNode();
      // Store flux
      cell_ca_builder.sum(F, K, tau);
      cell_ca_builder.subtract(F, L, tau);
    } // END for
  } else {
    for(Integer iF = 0; iF < node.nbFace(); ++iF){
      const Face & F = node.face(iF);
      const Cell& K  = F.backCell();
      //numero locale de la maille autour du noeud
      Integer iK_loc   = NumLocCell[K];
      FaceNum & face_local = NumLocfacesbyCell[iK_loc];
      Integer nbfaceK =  face_local.size();

      // Update flux through F
      for (Integer jF = 0 ; jF < nbfaceK ; ++jF) {
        for(Integer iL = 0 ; iL < nbcell ; ++iL) {
          const Cell& L = node.cell(iL);
          // Add the contribution from L
          cell_ca_builder.sum(F, L, Fluxes[iK_loc][iF][jF]*B(face_local[jF], iL));
        }
      }
      cell_ca_builder.sum(F, K, Fluxes[iK_loc][iF][nbfaceK]);
    }
  }

  return (Integer)switch_to_two_point;
}
#endif /* ARCANE_VERSION */

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
#endif
