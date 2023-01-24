/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "../IWellManager.h"
#include "WellManager_axl.h"
#include "WellIndexComputer.h"

#include "ArcGeoSim/Numerics/Expressions/ExpressionBuilder/ExpressionBuilderR1vR1Core.h"

// gump
#include "ArcRes/XPath.h"

#include "ArcGeoSim/Appli/AppService.h"

#include "ArcGeoSim/Physics/Law2/Contribution/VariableFolderAccessor.h"

#include "ArcGeoSim/Numerics/Expressions/FunctionParser/FunctionParser.h"
#include "ArcGeoSim/Numerics/Expressions/IExpressionMng.h"
#include "ArcGeoSim/Numerics/Expressions/ExpressionBuilder/ExpressionBuilderR3vR1Core.h"

#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"
#include "ArcGeoSim/Mesh/Geometry/ManualUpdateGeometryPolicy.h"

#include <arcane/IMesh.h>
#include <arcane/ItemVector.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class WellManagerService
  : public ArcaneWellManagerObject
  //, private PhysicalSystemAccessor
{
public:

  WellManagerService(const Arcane::ServiceBuildInfo & sbi)
      : ArcaneWellManagerObject(sbi)
      , m_initialized(false)
      , m_geometry_mng(NULL)
      , m_perforated_cells(Arcane::VariableBuildInfo(mesh(),"PerforatedCells")) {}

  ~WellManagerService()
  {
    for(Arcane::Integer i = 0; i < m_wells.size(); ++i)
      delete m_wells[i];
  }

public:

  void init(ArcRes::System& system, Law::VariableCellFolder& domain);

  void update(ArcRes::System& system);

  IWellManager::Enumerator enumerator() const
  {
    return WellEnumerator(m_wells, m_well_names, m_well_index, m_is_flow);
  }


private:

  void _fatalIfNotInitialized() const;

  void _updateLimitCondition(ArcRes::System& system);

private:

  bool m_initialized;

  IGeometryMng* m_geometry_mng;

  Arcane::UniqueArray<Law::PartialVariableCellFolder*> m_wells;
  Arcane::UniqueArray<Arcane::String> m_well_names;

  Arcane::UniqueArray<Arcane::PartialVariableCellReal*> m_well_index;

  Arcane::ByteUniqueArray m_is_flow;

  Arcane::VariableCellInteger m_perforated_cells;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
WellManagerService::
_fatalIfNotInitialized() const
{
  if(not m_initialized)
    fatal() << "Well manager service is not initialized, call init method before";
}

/*---------------------------------------------------------------------------*/

void
WellManagerService::
init(ArcRes::System& system, Law::VariableCellFolder& domain)
{
  // Init Source Manager

  if(m_initialized) return;

  ArcGeoSim::AppService<IGeometryMng> geometry_mng;
  ArcGeoSim::AppService<IExpressionMng> expression_mng;

  m_geometry_mng = geometry_mng;

  m_geometry_mng->addItemGroupProperty(allCells(),
      IGeometryProperty::PMeasure,
      IGeometryProperty::PVariable);
  m_geometry_mng->addItemGroupProperty(mesh()->allActiveCells(),
      IGeometryProperty::PMeasure,
      IGeometryProperty::PVariable);

  m_geometry_mng->addItemGroupProperty(allCells(),
      IGeometryProperty::PCenter,
      IGeometryProperty::PVariable);
  m_geometry_mng->addItemGroupProperty(mesh()->allActiveCells(),
      IGeometryProperty::PCenter,
      IGeometryProperty::PVariable);

  IGeometryPolicy* geometryPolicy = new ManualUpdateGeometryPolicy;
  m_geometry_mng->update(mesh(), geometryPolicy);
  delete geometryPolicy;

  WellIndexComputer wi(mesh());

  for(Arcane::Integer i = 0; i < options()->well.size(); ++i) {

   auto model_name = Arcane::String::format("Well_{0}",i);

   const Arcane::String cell_group_name = options()->well[i].cellGroup();
   ItemGroup group = mesh()->cellFamily()->findGroup(cell_group_name);
   if(group.itemKind() != Arcane::IK_Cell)
     fatal() << "Well model need a cell group";
   auto name = Arcane::String::format("{0}_{1}",model_name,group.name());
   auto* folder = new Law::PartialVariableCellFolder(name,mesh());

   // Init folder
   m_wells.add(folder);
   folder->setSupport(group);
   if(options()->well[i]->name.isPresent()) {
     m_well_names.add(options()->well[i]->name());
   } else {
     m_well_names.add(model_name);
   }

   //==================
   // Add Darcy Velocity
   //==================
    ENUMERATE_PHASE(iphase, system.fluidSubSystem().phases()) {
      const ArcRes::Phase & phase = *iphase;
      folder->addVariable(ArcRes::DarcyVelocity(phase));
    }

    for(Arcane::Integer j = 0; j < options()->well[i].limitCondition.size(); ++j) {
        auto p = ArcRes::XPath::scalarRealProperty(system, options()->well[i].limitCondition[j].property());
      folder->addVariable(p);
    }

    if(options()->well[i].flowRate.size() == 1) {
      m_is_flow.add(true);
      folder->addVariable(ArcRes::FlowRate(system));
      m_well_index.add(NULL);
    }

    else {
      m_is_flow.add(false);
      Arcane::String name = Arcane::String::format("WellIndex_{0}_{1}",i,group.name());
      Arcane::VariableBuildInfo vbi(mesh(),name,group.itemFamily()->name(),group.name());
      Arcane::PartialVariableCellReal* well_index = new Arcane::PartialVariableCellReal(vbi);
      wi.compute(system, domain, *well_index,options()->well[i].radius(),options()->well[i].skin() );
      m_well_index.add(well_index);
    }

  }

  IGeometryMng::Real3Variable cell_centers
    = m_geometry_mng->getReal3VariableProperty(allCells(), IGeometryProperty::PCenter) ;

  m_perforated_cells.fill(false);

  // Fill properties

  for(Arcane::Integer i = 0; i < options()->well.size(); ++i) {
    Law::PartialVariableCellFolder& folder = *(m_wells[i]);
    Arcane::CellGroup group = folder.support();

    for(Arcane::Integer j = 0; j < options()->well[i]->limitCondition.size(); ++j) {
      auto p = ArcRes::XPath::scalarRealProperty(system, options()->well[i]->limitCondition[j]->property());
      // Fill
      auto variable = Law::values(folder,p);

      // Cr�ation de l'expression
      FunctionParser parser;
      parser.init(expression_mng, NULL, traceMng());
      parser.parseString("(x,y,z;t)->" + options()->well[i]->limitCondition[j].value());
      ExpressionBuilderR3vR1Core expression(&parser);
      expression.setParameter("t",m_global_time());

      ENUMERATE_CELL(icell, group) {
        Arcane::Real3 center = cell_centers[icell];
        variable[icell] = expression.eval(center[0], center[1], center[2]);
        m_perforated_cells[icell] = true;
      }
    }
  }


  m_initialized = true;
}

/*---------------------------------------------------------------------------*/

void 
WellManagerService::
_updateLimitCondition(ArcRes::System& system)
{
  ArcGeoSim::AppService<IExpressionMng> expression_mng;

  // Update Source group after AMR

  _fatalIfNotInitialized();

  //ArcGeoSim::AppService<Geoxim::IZoneAccessor> zone_accessor;

  // Fill properties
  IGeometryMng::Real3Variable cell_centers = m_geometry_mng -> getReal3VariableProperty(allCells(), IGeometryProperty::PCenter) ;

  m_perforated_cells.fill(false);

  for(Arcane::Integer i = 0; i < options()->well.size(); ++i) {
    auto& folder = *(m_wells[i]);
    Arcane::CellGroup group = folder.support();

    for(Arcane::Integer j = 0; j < options()->well[i]->limitCondition.size(); ++j) {
      auto p = ArcRes::XPath::scalarRealProperty(system, options()->well[i]->limitCondition[j]->property());
      // Fill
      auto variable = Law::values(folder,p);

      // Cr�ation de l'expression
      FunctionParser parser;
      parser.init(expression_mng, NULL, traceMng());
      parser.parseString("(x,y,z;t)->" + options()->well[i]->limitCondition[j].value());
      ExpressionBuilderR3vR1Core expression(&parser);
      expression.setParameter("t",m_global_time());

      ENUMERATE_CELL(icell, group) {
        Arcane::Real3 center = cell_centers[icell];
        variable[icell] = expression.eval(center[0], center[1], center[2]);
        m_perforated_cells[icell] = true;
      }
    }
  }

}

/*---------------------------------------------------------------------------*/

void
WellManagerService::
update(ArcRes::System& system)
{
  // Compute Dynamic Source Properties

  _fatalIfNotInitialized();

  _updateLimitCondition(system);

  ArcGeoSim::AppService<IExpressionMng> expression_mng;

  // Update Volume of Sources
  IGeometryPolicy* geometryPolicy = new ManualUpdateGeometryPolicy;
  m_geometry_mng->update(mesh(), geometryPolicy);
  delete geometryPolicy;

  const IGeometryMng::RealVariable & cell_volume
    = m_geometry_mng->getRealVariableProperty(mesh()->allActiveCells(),IGeometryProperty::PMeasure);

  for(Arcane::Integer i = 0; i < options()->well.size(); ++i) {

    if(not m_is_flow[i]) continue;

    auto& folder = *(m_wells[i]);

    Arcane::CellGroup group = folder.support();

    Arcane::Real volume = 0;
    //const Arcane::String zone_label = options()->well[i].zone();

    ENUMERATE_CELL( cell_k, group.ownActiveCellGroup() ) {
      volume += cell_volume[cell_k];
    }

    //volume = subDomain()->parallelMng()->reduce(Arcane::Parallel::ReduceSum,volume);
    //info() << "Well " << i+1 << ": " << zone_label << " with Volume = " << volume;

    // Cr�ation de l'expression pour le d�bit
    FunctionParser parser;
    parser.init(expression_mng, NULL, traceMng());
    parser.parseString("(t)->" + options()->well[i]->flowRate[0].value());
    ExpressionBuilderR1vR1Core expression(&parser);

    auto flow = Law::values<ArcRes::FlowRate>(folder,system);

    // Remplissage
    ENUMERATE_CELL(icell, group) {
      flow[icell] = expression.eval(m_global_time()) / volume; // m3.s-1 / m3
    }
  }
}



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;
ARCANE_REGISTER_SERVICE_WELLMANAGER(WellManager,WellManagerService);
