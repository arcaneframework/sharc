// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_ARCGEOSIM_SURFACEUTILS_CONTACTFAMILYVARIABLESYNCHRONIZER_H
#define ARCGEOSIM_ARCGEOSIM_SURFACEUTILS_CONTACTFAMILYVARIABLESYNCHRONIZER_H

#include <arcane/ArcaneTypes.h>
#include <arcane/utils/Array.h>
#include <arcane/utils/UtilsTypes.h>
#include <arcane/IVariableSynchronizer.h>
#include <arcane/IParallelMng.h>

#include <ArcGeoSim/Mesh/SurfaceUtils/ContactGroup.h>

/*!
 * Classe construite pour pouvoir utilis�e des connectivit�s distribu�es avec des contacts.
 * Seules les m�thodes de IVariableSynchronizer n�cessaire pour synchroniser les connectivit�s ont �t� impl�ment�es.
 */
class ContactFamilyVariableSynchronizer : public Arcane::IVariableSynchronizer
{
public:
  
  ContactFamilyVariableSynchronizer(Arcane::IParallelMng* pm, ContactGroup cg)
    : m_parallelMng(pm)
    , m_contactGroup(cg) {}
  
  virtual ~ContactFamilyVariableSynchronizer() {}

  virtual void compute() ;

  virtual Arcane::Int32ConstArrayView communicatingRanks() {
    return m_communicating_ranks ;
  }

  virtual Arcane::Int32ConstArrayView sharedItems(Arcane::Int32 index) {
    return m_sync_list[index].m_share_ids ;
  }

  virtual Arcane::Int32ConstArrayView ghostItems(Arcane::Int32 index) {
    return m_sync_list[index].m_ghost_ids;
  }

  virtual Arcane::IParallelMng* parallelMng() {
    return m_parallelMng;
  }

  virtual const Arcane::ItemGroup& itemGroup() {
    _notImplemented("itemGroup"); return m_empty_group;
  }

  virtual void changeLocalIds(Int32ConstArrayView old_to_new_ids) {
    _notImplemented("changeLocalIds");
  }

  virtual void synchronize(Arcane::IVariable* var) {
    _notImplemented("synchronize");
  }

  virtual void synchronize(Arcane::VariableCollection vars) {
    _notImplemented("synchronize");
  }

#if (ARCANE_VERSION >= 31303)
    void synchronize(Arcane::IVariable* var, Int32ConstArrayView local_ids) override {
        ARCANE_UNUSED(var);
        ARCANE_UNUSED(local_ids);
        _notImplemented("synchronize");
    }

    void synchronize(Arcane::VariableCollection vars, Int32ConstArrayView local_ids) override {
        ARCANE_UNUSED(vars);
        ARCANE_UNUSED(local_ids);
        _notImplemented("synchronize");
    }
#endif

  virtual void synchronizeData(Arcane::IData* data) {
    _notImplemented("synchronizeData");
  }

#if (ARCANE_VERSION >= 20405)
  virtual Arcane::EventObservable<const Arcane::VariableSynchronizerEventArgs&>& onSynchronized() {
    _notImplemented("onSynchronized");
    return m_on_synchronized ;
  }
#endif /* ARCANE_VERSION */

private:

void _notImplemented(const Arcane::String& method_name) const {throw Arcane::NotImplementedException(Arcane::String::format("ContactFamilyVariableSynchronizer does not cover the whole IVariableSynchronizer scope. In method {0}",
                                                                                                                            method_name));}

  class VariableSyncInfo
  {
  public:
    
    VariableSyncInfo() : m_target_rank(Arcane::NULL_SUB_DOMAIN_ID), m_is_send_first(false) {}
    VariableSyncInfo(Arcane::Int32ConstArrayView share_ids, Arcane::Int32ConstArrayView ghost_ids,
                     Arcane::Int32 rank, bool is_send_first)
      : m_share_ids(share_ids), m_ghost_ids(ghost_ids),
	m_target_rank(rank), m_is_send_first(is_send_first) {}
    ~VariableSyncInfo() {}
    
  public:
    
    Arcane::Int32 targetRank() const { return m_target_rank; }
    
  public:
    
    //! localIds() des entités à envoyer au processeur #m_rank
    Arcane::Int32SharedArray m_share_ids;
    //! localIds() des entités à réceptionner du processeur #m_rank
    Arcane::Int32SharedArray m_ghost_ids;
    //! Rang du processeur cible
    Arcane::Integer m_target_rank;
    //!< \a true si on envoie avant de recevoir
    bool m_is_send_first;
  } ;
  
  class RankInfo 
  {
  public:
    RankInfo() : m_rank(Arcane::A_NULL_RANK) {}
    RankInfo(Arcane::Int32 rank)
      : m_rank(rank) {}
  public:
    Arcane::Int32 rank() const { return m_rank; }
    void setRank(Arcane::Int32 rank) { m_rank = rank; }
    bool operator<(const RankInfo& ar) const
    {
      return m_rank < ar.m_rank;
    }
  private:
    Arcane::Int32 m_rank;
  };

  class GhostRankInfo : public RankInfo
  {
  public:
    GhostRankInfo() : m_nb_item(0) {}
    GhostRankInfo(Arcane::Int32 rank)
      : RankInfo(rank), m_nb_item(0) {}
    GhostRankInfo(Arcane::Int32 rank, Arcane::Integer nb_item)
      : RankInfo(rank), m_nb_item(nb_item) {}
  public:
    void setInfos(Arcane::Int32 rank, Arcane::Int32SharedArray local_ids)
    {
      setRank(rank);
      m_nb_item = local_ids.size();
      m_local_ids = local_ids;
    }
    Arcane::Int32ConstArrayView localIds() const { return m_local_ids; }
    Arcane::Integer nbItem() const { return m_nb_item; }
    void resize() { m_unique_ids.resize(m_nb_item); }
    Arcane::Int64ArrayView uniqueIds() { return m_unique_ids; }

  private:
    Arcane::Integer m_nb_item;
    Arcane::Int32SharedArray m_local_ids;
    Arcane::Int64SharedArray m_unique_ids;
  };

  class ShareRankInfo : public RankInfo
  {
  public:
    ShareRankInfo() : m_nb_item(0) {}
    ShareRankInfo(Arcane::Int32 rank, Arcane::Integer nb_item)
      : RankInfo(rank), m_nb_item(nb_item) {}
    ShareRankInfo(Arcane::Int32 rank)
      : RankInfo(rank), m_nb_item(0) {}
  public:
    void setInfos(Arcane::Int32 rank, Arcane::Int32SharedArray local_ids)
    {
      setRank(rank);
      m_nb_item = local_ids.size();
      m_local_ids = local_ids;
    }
    Arcane::Int32ConstArrayView localIds() const { return m_local_ids; }
    void setLocalIds(Arcane::Int32SharedArray v) { m_local_ids = v; }
    Arcane::Integer nbItem() const { return m_nb_item; }
    void resize() { m_unique_ids.resize(m_nb_item); }
    Arcane::Int64ArrayView uniqueIds() { return m_unique_ids; }
  private:
    Arcane::Integer m_nb_item;
    Arcane::Int32SharedArray m_local_ids;
    Arcane::Int64SharedArray m_unique_ids;
  };

  Arcane::Int64 _form_unique_id(const ContactInternal & contact) const ;
  void _uniqueIdToLocalId(Arcane::Int32ArrayView localIds, Arcane::Int64ConstArrayView uniqueIds) const ;
  void _checkValid(Arcane::ArrayView<GhostRankInfo> ghost_rank_info, Arcane::ArrayView<ShareRankInfo> share_rank_info) ;
  Arcane::IParallelMng* m_parallelMng ;
  ContactGroup m_contactGroup ;
  Arcane::SharedArray<VariableSyncInfo> m_sync_list ;
  Arcane::Int32SharedArray m_communicating_ranks ;
  Arcane::ItemGroup m_empty_group; // to avoid warning. Todo replace by ARCANE_NO_RETURN when moving to gcc > 4.7
#if (ARCANE_VERSION >= 20405)
  Arcane::EventObservable<const Arcane::VariableSynchronizerEventArgs&> m_on_synchronized ;
#endif /* ARCANE_VERSION */ 
};

#endif /* ARCGEOSIM_ARCGEOSIM_SURFACEUTILS_CONTACTFAMILYVARIABLESYNCHRONIZER_H */
