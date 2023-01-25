//
// Created by yousefs on 05/12/2022.
//

#ifndef NUMERICS_CONDITION_WELLENUMERATOR_H
#define NUMERICS_CONDITION_WELLENUMERATOR_H

#include "TwoPhaseFlowSimulation/BoundaryCondition/FolderEnumerator.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class WellEnumerator
  : public FolderEnumerator<Law::ContainerKind::PartialVariable,
                            Law::ItemKind::Cell>
{
private:

  typedef FolderEnumerator<Law::ContainerKind::PartialVariable,
                           Law::ItemKind::Cell> Base;

public:

  WellEnumerator(const Arcane::Array<Folder*>& folders,
                 const Arcane::StringArray& names,
                 const Arcane::Array<Arcane::PartialVariableCellReal*>& well_index,
                 const Arcane::ByteArray& is_flow)
    : Base(folders, names)
    , m_folders(folders)
    , m_well_index(well_index)
    , m_is_flow(is_flow) {}

  WellEnumerator(const WellEnumerator& e)
    : Base(e)
    , m_folders(e.m_folders)
    , m_well_index(e.m_well_index)
    , m_is_flow(e.m_is_flow){}

    //! Acces aux entites
    const Folder& folder() const { return *(m_folders[index()]); }

    Folder& folder() { return *(m_folders[index()]); }


    const Arcane::PartialVariableCellReal& wellIndex() const
  {
    return *m_well_index[index()];
  }

  bool isFlow() const
  {
    return m_is_flow[index()];
  }

private:

    //! Liste des entites a enumerer
  const Arcane::Array<Folder*>& m_folders;

  const Arcane::Array<Arcane::PartialVariableCellReal*>& m_well_index;

  const Arcane::ByteArray& m_is_flow;
};


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* NUMERICS_CONDITION_WELLENUMERATOR_H */
