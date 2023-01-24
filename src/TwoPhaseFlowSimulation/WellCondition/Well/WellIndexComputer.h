// -*- C++ -*-
#ifndef WELLCONDITIONS_CONDITION_WELL_WELLINDEXCOMPUTER_H
#define WELLCONDITIONS_CONDITION_WELL_WELLINDEXCOMPUTER_H


// gump
#include "ArcRes/XPath.h"

#include "ArcGeoSim/Appli/AppService.h"

#include "ArcGeoSim/Physics/Law2/Contribution/VariableFolderAccessor.h"

#include "ArcGeoSim/Numerics/Expressions/FunctionParser/FunctionParser.h"
#include "ArcGeoSim/Numerics/Expressions/IExpressionMng.h"
#include "ArcGeoSim/Numerics/Expressions/ExpressionBuilder/ExpressionBuilderR3vR1Core.h"

#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"

#include <arcane/IMesh.h>
#include <arcane/ItemVector.h>

#include "ArcRes/Entities/System.h"
#include "ArcGeoSim/Physics/Gump/Property.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

    class WellIndexComputer
    {
    public:

        WellIndexComputer(IMesh*  mesh);

    public:

        void compute(ArcRes::System& system,
                     Law::VariableCellFolder& domain,
                     Arcane::PartialVariableCellReal well_index,
                     Arcane::Real radius,
                     Arcane::Real skin) const;

    private:

        inline Arcane::Real _computeWellIndex(const Arcane::Real3& cell_dims,
                                              const Arcane::Real3& cell_perm,
                                              const Arcane::Real& well_radius,
                                              const Arcane::Real& well_skin) const;

//  inline Arcane::Real _computeWellIndexNew(const Arcane::Real3& cell_dims,
//                                           const Arcane::Real3& cell_perm,
//                                           const Arcane::Real& well_radius,
//                                           const Arcane::Real& well_skin) const;

        inline Arcane::Real3 _computeCellThickness(const Arcane::Cell& cell) const;

    private:

        Arcane::Real m_pi;

        const Arcane::VariableNodeReal3& m_node_coordinates;
    };

/*---------------------------------------------------------------------------*/

//Arcane::Real
//WellIndexComputer::
//_computeWellIndexNew(const Arcane::Real3& cell_dims,
//                     const Arcane::Real3& cell_perm,
//                     const Arcane::Real& well_radius,
//                     const Arcane::Real& well_skin ) const
//{
//  const Arcane::Real KX = cell_perm.x;
//  const Arcane::Real KY = cell_perm.y;
//
//  if(Arcane::math::isNearlyZero(KX) || Arcane::math::isNearlyZero(KY))
//     return 0;
//
//  const Arcane::Real DX = cell_dims.x;
//  const Arcane::Real DY = cell_dims.y;
//  const Arcane::Real H  = cell_dims.z;
//
//  const Arcane::Real N = Arcane::math::sqrt(KY/KX)*DX*DX + Arcane::math::sqrt(KX/KY)*DY*DY ;
//  const Arcane::Real D = Arcane::math::pow(KY/KX, 0.25)  + Arcane::math::pow(KX/KY, 0.25);
//  const Arcane::Real R0 = 0.28*N/D;
//  const Arcane::Real PI = ( 2 * m_pi * Arcane::math::sqrt(KX*KY) * H ) /
//                        ( Arcane::math::log(R0/well_radius) + well_skin );
//  return PI;
//}

/*---------------------------------------------------------------------------*/

    Arcane::Real
    WellIndexComputer::
    _computeWellIndex(const Arcane::Real3& cell_dims,
                      const Arcane::Real3& cell_perm,
                      const Arcane::Real& well_radius,
                      const Arcane::Real& well_skin ) const
    {
        Arcane::Real well_index = 0.;

        bool debug_info = false;  // true

        if (debug_info)
        {
            std::cout << "=== DEBUG Peaceman Compute Well Index ===" << std::endl;

            std::cout << "cell_perm.x = " << cell_perm.x  << std::endl;
            std::cout << "cell_perm.y = " << cell_perm.y  << std::endl;
            std::cout << "cell_perm.z = " << cell_perm.z  << std::endl;
            std::cout << "cell_dims.x = " << cell_dims.x  << std::endl;
            std::cout << "cell_dims.y = " << cell_dims.y  << std::endl;
            std::cout << "cell_dims.z = " << cell_dims.z  << std::endl;
            std::cout << "well_radius = " << well_radius  << std::endl;
            std::cout << "well_skin   = " << well_skin    << std::endl;
        }

        Arcane::Real eps_perm = 1.e-20;

        bool is_null_permx_or_permy = (cell_perm.x < eps_perm) || (cell_perm.y < eps_perm);

        // Null PermX or PermY Case

        if(is_null_permx_or_permy)
        {
            well_index = 0;

            // Debug Results

            if (debug_info)
            {
                std::cout << "=========================================" << std::endl;
                std::cout << "NULL PERMX OR PERMY case " << std::endl;
                std::cout << "=========================================" << std::endl;
                std::cout << "well_index  = "       << well_index   << std::endl;
                std::cout << "=========================================" << std::endl;
            }
        }

            // Standard Case

        else
        {
            const Arcane::Real kx_ky = cell_perm.x/cell_perm.y;
            const Arcane::Real ky_kx = cell_perm.y/cell_perm.x;

            Arcane::Real well_r = Arcane::math::sqrt(ky_kx)*cell_dims.x*cell_dims.x +
                                  Arcane::math::sqrt(kx_ky)*cell_dims.y*cell_dims.y;

            well_r = 0.28*Arcane::math::sqrt(well_r) /
                     (Arcane::math::pow(ky_kx, 0.25) + Arcane::math::pow(kx_ky, 0.25));

            if (well_r <= well_radius)
            {
                std::cout << "well_r = " << well_r << " well_radius = " << well_radius << std::endl;
                throw Arcane::FatalErrorException("Invalid Peaceman formula : well_r <= well_radius");
            }

            well_index = 2 * m_pi * Arcane::math::sqrt(cell_perm.x*cell_perm.y) * cell_dims.z /
                         ( Arcane::math::log(well_r/well_radius) + well_skin );

            // Debug Results

            if (debug_info)
            {
                std::cout << "=========================================" << std::endl;
                std::cout << "kx_ky       = " << kx_ky        << std::endl;
                std::cout << "ky_kx       = " << ky_kx        << std::endl;
                std::cout << "well_r      = " << well_r       << std::endl;
                std::cout << "=========================================" << std::endl;
                std::cout << "well_index  = " << "2 * pi * sqrt(cell_perm.x*cell_perm.y) * cell_dims.z / (log(well_r/well_radius) + well_skin)"<< std::endl;
                std::cout << "=========================================" << std::endl;
                std::cout << "well_index  = " << well_index   << std::endl;
                std::cout << "=========================================" << std::endl;
            }

        }

        return well_index;
    }

/*---------------------------------------------------------------------------*/

    Arcane::Real3
    WellIndexComputer::
    _computeCellThickness(const Arcane::Cell& cell) const
    {
        Arcane::Real3 min_coord, max_coord;

        min_coord = max_coord = m_node_coordinates[cell.node(0)];

        ENUMERATE_NODE(inode, cell.nodes())
        {
            const Arcane::Real3& node_coord = m_node_coordinates[inode];

            min_coord.x = std::min(min_coord.x, node_coord.x);
            max_coord.x = std::max(max_coord.x, node_coord.x);

            min_coord.y = std::min(min_coord.y, node_coord.y);
            max_coord.y = std::max(max_coord.y, node_coord.y);

            min_coord.z = std::min(min_coord.z, node_coord.z);
            max_coord.z = std::max(max_coord.z, node_coord.z);
        }

        return max_coord - min_coord;
    }



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* WELLCONDITIONS_CONDITION_WELL_WELLINDEXCOMPUTER_H */
