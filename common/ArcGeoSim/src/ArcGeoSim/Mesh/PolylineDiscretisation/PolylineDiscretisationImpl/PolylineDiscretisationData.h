// -*- C++ -*-
#ifndef ARCGEOSIM_MESH_POLYLINEDISCRETISATION_POLYLINEDISCRETISATIONDATA_H
#define ARCGEOSIM_MESH_POLYLINEDISCRETISATION_POLYLINEDISCRETISATIONDATA_H


#include "arcane/utils/UtilsTypes.h"
#include <arcane/VariableTypes.h>

using namespace Arcane;

class PolylineDiscretisationData;

typedef UniqueArray<PolylineDiscretisationData> PolylineDiscretisationDataArray;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Class PolylineDiscretisationData
 *
 * This classes contains all informations about the well discretisation on 3D unstructured mesh.
 *
 * This is created along importing OpenFlow's PolylineDiscretization class.
 */
class PolylineDiscretisationData
{
public:

  /** Constructeur de la classe */
  PolylineDiscretisationData()
  {
  }

  /** Destructeur de la classe */
  virtual ~PolylineDiscretisationData()
  {
  }

public:

  void clear();
  void add(Integer segment, Integer cell, Real coord);

  UniqueArray<Integer> const& getResSegmentIdx() const
  {
    return m_res_segment_idx;
  }

  UniqueArray<Integer> const& getResCellIdx() const
  {
    return m_res_cell_idx;
  }

  UniqueArray<Real> const& getResCoordInter() const
  {
    return m_res_coord_inter;
  }

private:
  /** Index of the segment the point belong */
  UniqueArray<Integer> m_res_segment_idx;
  /** Index of the cell the point belong */
  UniqueArray<Integer> m_res_cell_idx;
  /** curvilinear coordinates of the point on its segment (0..1) */
  UniqueArray<Real> m_res_coord_inter;
};

#endif /* ARCGEOSIM_MESH_POLYLINEDISCRETISATION_POLYLINEDISCRETISATIONDATA_H */
