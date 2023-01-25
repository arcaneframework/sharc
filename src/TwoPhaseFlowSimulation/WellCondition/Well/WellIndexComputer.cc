#include "WellIndexComputer.h"



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

WellIndexComputer::
WellIndexComputer(IMesh*  mesh)
: m_pi(std::acos(-1.))
, m_node_coordinates(mesh->nodesCoordinates()) {}

/*---------------------------------------------------------------------------*/

void
WellIndexComputer::
compute(ArcRes::System& system, Law::VariableCellFolder& domain, Arcane::PartialVariableCellReal well_index, Arcane::Real radius, Arcane::Real skin) const
{
    auto well_cells = well_index.itemGroup();

    auto fluid = system.fluidSubSystem();

    auto Kx = Law::values<ArcRes::XPermeability>(domain,fluid);
    auto Ky = Law::values<ArcRes::YPermeability>(domain,fluid);
    auto Kz = Law::values<ArcRes::ZPermeability>(domain,fluid);

    ENUMERATE_CELL(icell, well_cells) {

        auto Dx = Kx[icell];
        auto Dy = Ky[icell];
        auto Dz = Kz[icell];

        Arcane::Real3 permeability_real3(Dx, Dy, Dz) ;

        auto cell_dims = _computeCellThickness(*icell);

        well_index[icell] = _computeWellIndex(cell_dims, permeability_real3 , radius, skin);
    }

    bool debug_info = false;  // true

    if (debug_info)
    {
        std::cout << "=========================================" << std::endl;
        std::cout << " Well Index Compute Results              " << std::endl;
        std::cout << "=========================================" << std::endl;

        ENUMERATE_CELL(icell, well_cells) {
            std::cout << "cell = " << icell->uniqueId() << " well_index = " << well_index[icell]  << std::endl;
        }

        std::cout << "=========================================" << std::endl;
    }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
