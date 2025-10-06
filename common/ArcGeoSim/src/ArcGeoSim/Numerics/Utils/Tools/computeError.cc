#include "computeError.h"
#include <arcane/IMeshSubMeshTransition.h>
#include "ArcGeoSim/Numerics/BCs/BoundaryConditionTypes.h"

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Real computeLpErrorNorm(const VariableCellReal& uh,
                        const CellGroup&        cells,
                        const IGeometryMng::Real3GroupMap& c_centers,
                        const IGeometryMng::RealGroupMap& c_measures,
                        IFunctionR4vR1*              u,
                        Real                    p,
                        Real                    t) {
  
  Real errp = 0.;
  ENUMERATE_CELL(iT, cells) {
    const Cell& T = *iT;
    const Real3& C_T = c_centers[T];
    Real meas_T = c_measures[T];
    Real uh_T = uh[T];
    Real u_T = u->eval(C_T.x,C_T.y,C_T.z, t);

    errp = errp + meas_T * pow(math::abs(u_T - uh_T), p);
  }
  Real err = pow(errp, 1/p);
  return err;
}

/*----------------------------------------------------------------------------*/

Real computeLpErrorNormPowerp(const VariableCellReal& uh,
			      const CellGroup&        cells,
			      const IGeometryMng::Real3GroupMap& c_centers,
			      const IGeometryMng::RealGroupMap& c_measures,
			      IFunctionR4vR1*              u,
			      Real                    p,
			      Real                    t) {
  
  Real errp = 0.;
  ENUMERATE_CELL(iT, cells) {
    const Cell& T = *iT;
    const Real3& C_T = c_centers[T];
    Real meas_T = c_measures[T];
    Real uh_T = uh[T];
    Real u_T = u->eval(C_T.x,C_T.y,C_T.z, t);

    errp = errp + meas_T * pow(math::abs(u_T - uh_T), p);
  }

  return errp;
}

/*----------------------------------------------------------------------------*/

Real2 computeFluxErrors(const FaceGroup&        faces,
                          const CellGroup& cells,
                          const FaceGroup& boundary_faces,
                          const VariableCellReal& uh,
                          const  PartialVariableFaceReal& uhb,
                          const IGeometryMng::RealGroupMap& f_measures,
                          const IGeometryMng::Real3GroupMap& f_centers,
                          const IGeometryMng::Real3GroupMap& f_normals,
                          const IGeometryMng::RealGroupMap& c_measures,
                          const VariableCellReal3x3& k,
                          IMesh* mesh,
                          IBoundaryConditionMng* m_bc_mng,
                          boost::shared_ptr<CoefficientArrayT<Cell> > m_cell_coefficients,
                          boost::shared_ptr<CoefficientArrayT<Face> >  m_face_coefficients,
                          IFunctionR4vR1*  grad_x,
                          IFunctionR4vR1*  grad_y,
                          IFunctionR4vR1*  grad_z,
                          Real                    t)
{
  Real2 FluxErrors;
  Real erFl2 = 0.;
  Real erinf = 0.;

  // variables for the normalization of the flux
  Real normInf = 0.;
  Real normL2 = 0.;

  // Get the boundary face unknowns
  Integer MaxFaceId = boundary_faces.mesh()->faceFamily()->maxLocalId();
  SharedArray<Real> u_boun(MaxFaceId);
  const FaceGroup& non_dirichlet_faces = uhb.itemGroup();

  VariableBuildInfo vb(mesh , "BoundaryValues", IVariable::PNoRestore);

  VariableFaceReal u_b(vb);
  ENUMERATE_FACE(iface, non_dirichlet_faces){
    const Face & face = *iface;
    u_b[face] = uhb[iface];
  }
  ENUMERATE_FACE(iface, boundary_faces.own()){
    const Face & F = *iface;
    IBoundaryCondition* bc_F = m_bc_mng->getBoundaryCondition(F);
    if (bc_F->getType() == BoundaryConditionTypes::Dirichlet ){
      const Real3& C_F = f_centers[F];
      u_b[F] = bc_F->eval(C_F, t);
    }
  }
  u_b.synchronize();

  SharedArray<Real> local_error(MaxFaceId);
  SharedArray<Real> local_norm(MaxFaceId);

  ENUMERATE_FACE(iface, faces){
    const Face & F = *iface;
// Calculate the flux density approximation
    Integer c_st_size = m_cell_coefficients->stencilSize(F);
    ItemVectorView c_stencilF = m_cell_coefficients->stencil(F);
    ArrayView<Real> c_coeffF = m_cell_coefficients->coefficients(F);
    Real flux_approx = 0.;

    for (Integer i = 0 ; i < c_st_size ; i++){
      const Cell& cell = c_stencilF[i].toCell();
      flux_approx += c_coeffF[i] * uh[cell];
    }
    Integer f_st_size = m_face_coefficients->stencilSize(F);
    if (f_st_size){
      ArrayView<Real> f_coeffF = m_face_coefficients->coefficients(F);
      ItemVectorView  f_stencilF = m_face_coefficients->stencil(F);
      for (Integer i = 0 ; i < f_st_size ; i++)
        flux_approx += f_coeffF[i] * u_b[f_stencilF[i].toFace() ];
    }
    flux_approx /= f_measures[F]; // get the flux density
// Calculate the exact flux density
    Real3 normal = f_normals[F]/f_measures[F];
    Real3x3 Lambda;
    if (F.isSubDomainBoundary()){
      const Cell & cell = F.boundaryCell();
      Lambda = k[cell];
      if (!F.isSubDomainBoundaryOutside())
        normal = -normal;
    }
    else{
      const Cell & cell = F.backCell();
      Lambda = k[cell];
    }
    Real3 kn = math::prodTensVec(Lambda, normal);
    Real3 grad;
    grad.x = FaceAverage(mesh, F, grad_x);
    grad.y = FaceAverage(mesh, F, grad_y);
    grad.z = FaceAverage(mesh, F, grad_z);
    Real flux_exact = -math::scaMul(grad,kn);
// Calculate the local error
    local_error[F.localId()] = flux_exact-flux_approx;
// Normalization data
    local_norm[F.localId()] = math::abs(flux_exact);
  }

  ENUMERATE_FACE(iface, faces.own()){
    const Face & F = *iface;
    erinf = math::max(erinf , math::abs(local_error[F.localId()]));
    normInf = math::max(normInf , local_norm[F.localId()]);
  }

  ENUMERATE_CELL(icell,cells.own()){
    const Cell& cell = *icell;
    Real cell_error2 = 0.;
    Real cell_norm2 = 0.;
    ENUMERATE_FACE(iface,cell.faces()){
      const Face & face = *iface;
      cell_error2 += local_error[face.localId()] * local_error[face.localId()];
      cell_norm2 += local_norm[face.localId()] * local_norm[face.localId()];
    }
    cell_error2 *= c_measures[cell];
    cell_norm2 *= c_measures[cell];
    erFl2 += cell_error2;
    normL2 += cell_norm2;
  }

  Real erinf_glob = cells.mesh()->parallelMng()->reduce(Parallel::ReduceMax, erinf);
  Real erFl2_glob = cells.mesh()->parallelMng()->reduce(Parallel::ReduceSum, erFl2);
  Real normInf_glob = cells.mesh()->parallelMng()->reduce(Parallel::ReduceMax, normInf);
  Real normL2_glob = cells.mesh()->parallelMng()->reduce(Parallel::ReduceSum, normL2);

  FluxErrors.x = erinf_glob/normInf_glob;
  FluxErrors.y = math::sqrt(erFl2_glob/normL2_glob); // here the L2 error is squared

  return FluxErrors;
}

Real FaceAverage(IMesh* mesh, const Face& face,
                              IFunctionR4vR1* u, Real t)
{
  SharedArray<Real3> x(3);
  VariableNodeReal3 coords = PRIMARYMESH_CAST(mesh)->nodesCoordinates();
  for (Integer i = 0 ; i < 3 ; i++)
    x[i] = coords[face.node(i)];

  SharedArray2<Real> a(3,3); //baricentric points
  SharedArray<Real> w(4); //weights of the quadrature
  SharedArray<Real3> xG(13); //Gauss integration points (1+3+3+6)

  w[0] = -0.149570044467670;
  w[1] = 0.175615257433204;
  w[2] = 0.053347235608839;
  w[3] = 0.077113760890257;

  a[0][0] = 0.479308067841923;
  a[0][1] = 0.260345966079038;
  a[0][2] = a[1][1];

  a[1][0] = 0.869739794195568;
  a[1][1] = 0.065130102902216;
  a[1][2] = a[2][1];

  a[2][0] = 0.638444188569809;
  a[2][1] = 0.312865496004875;
  a[2][2] = 0.048690315425316;

// compute the Gauss points
  xG[0] = 0.;
  for (Integer i = 0 ; i < 3; i++)
    xG[0] += x[i];
  xG[0] *= 0.3333333333333333333333;

  Real3 x10 = x[1] - x[0];
  Real3 x20 = x[2] - x[0];

  for (Integer i = 0; i < 3; i++)
    xG[i+1] = x[0] + x10 * a[0][i] + x20 * a[0][(i+1)%3] ;

  for (Integer i = 0; i < 3; i++)
    xG[i+4] = x[0] + x10 * a[1][i] + x20 * a[1][(i+1)%3] ;

  for (Integer i = 0; i < 3; i++){
    xG[i+7] = x[0] + x10 * a[2][i] + x20 * a[2][(i+1)%3] ;
    xG[i+10] = x[0] + x10 * a[2][(i+1)%3] + x20 * a[2][i] ;
  }

  Real average = 0.;
  average += w[0] * u->eval(xG[0].x,xG[0].y,xG[0].z, t);
  Real sum = 0.;
  for (Integer i = 1 ; i <= 3; i++ )
    sum += u->eval(xG[i].x,xG[i].y,xG[i].z,t);
  average += w[1] * sum;
  sum = 0.;
  for (Integer i = 4 ; i <= 6; i++ )
    sum += u->eval(xG[i].x,xG[i].y,xG[i].z,t);
  average += w[2] * sum;
  sum = 0.;
  for (Integer i = 7 ; i <= 12; i++ )
    sum += u->eval(xG[i].x,xG[i].y,xG[i].z,t);
  average += w[3] * sum;
  return average;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
