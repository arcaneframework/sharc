/* Author : gratienj
 * Preconditioner created by combining separate preconditioners for individual
 * fields or groups of fields. See the users manual section "Solving Block Matrices"
 * for more details in PETSc 3.3 documentation : 
 * http://www.mcs.anl.gov/petsc/petsc-current/docs/manual.pdf
 */
#include "ArcGeoSim/Numerics/LinearAlgebra2/PETScImpl/PETScPrecomp.h"
#include "ArcGeoSim/Numerics/LinearSolver/PETScSolverImpl/IPETScPC.h"
#include "ArcGeoSim/Numerics/LinearSolver/PETScSolverImpl/IPETScKSP.h"
#include "ArcGeoSim/Numerics/LinearAlgebra2/IIndexManager.h"
#include "ArcGeoSim/Numerics/LinearSolver/PETScSolverImpl/FieldSplit/IFieldSplitType.h"

#include <set>
#include <map>

using namespace Arcane;
using namespace ArcGeoSim::Numerics;
#include "PETScPrecConfigFieldSplit_axl.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

//BEGIN_LINEARALGEBRA2SERVICE_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class PETScPrecConfigFieldSplitService :
  public ArcanePETScPrecConfigFieldSplitObject
{
public:
  /** Constructeur de la classe */
  PETScPrecConfigFieldSplitService(const Arcane::ServiceBuildInfo & sbi) :
    ArcanePETScPrecConfigFieldSplitObject(sbi)
    {
      ;
    }
  
  /** Destructeur de la classe */
  virtual ~PETScPrecConfigFieldSplitService() {}
  
public:
  //! Initialisation
  void configure(PC & pc, const Alien::IIndexManager * indexManager);

  //! Indicateur de support de r��solution parall��le
  bool hasParallelSupport() const;

  //! Check need of KSPSetUp before calling this PC configure
  bool needPrematureKSPSetUp() const;

public:
  //! Acces to TraceMng
  ITraceMng * traceMng() const { return BasicService::traceMng();  }

private :
  Integer initializeFields(const Alien::IIndexManager * indexManager);
  SharedArray<PC>  m_subpc ;

private:
  //! option for field split preconditioner
  static const String m_default_block_tag;
  SharedArray<String> m_field_tags;
  SharedArray<IS>     m_field_petsc_indices;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

//END_LINEARALGEBRA2SERVICE_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "petscmat.h"
#include "petscksp.h"

#include <arcane/utils/StringBuilder.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

//BEGIN_LINEARALGEBRA2SERVICE_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

const String PETScPrecConfigFieldSplitService::m_default_block_tag = "default";

/*---------------------------------------------------------------------------*/

Integer
PETScPrecConfigFieldSplitService::
initializeFields(const Alien::IIndexManager * index_manager)
{
  m_field_tags.clear();
  m_field_petsc_indices.clear();
  
  if(index_manager == NULL)
    fatal() << "FieldSplit preconditioner requires index-manager for block definition";

  Integer nerror = 0 ;

  const String block_tag = options()->blockTag();
  
  std::set<String> tag_set;
  for(Integer i=0;i<options()->block.size();++i)
    {
      String tag = options()->block[i].tag();
      if (tag == m_default_block_tag)
        fatal() << "block-tag 'default' is a reserved keyword";
      std::pair<std::set<String>::const_iterator, bool> inserter = tag_set.insert(tag);
      if (inserter.second)
        m_field_tags.add(tag) ;
      else
        fatal() << "Duplicate block-tag found : " << tag;
    }
  
  std::map<String, Alien::IIndexManager::Entry> tagged_entries;
  for(Alien::IIndexManager::EntryEnumerator i = index_manager->enumerateEntry(); i.hasNext(); ++i)
    {
      if (options()->verbose())
        info() << "Entry '" << i->getName() << "' has " << i->getOwnIndexes().size() << " indices";
      tagged_entries[i->getName()] = *i;
    }

  Integer nbField = m_field_tags.size() ;
  SharedArray<Integer> field_sizes(nbField, 0);
  SharedArray<SharedArray<Alien::IIndexManager::Entry> >  field_associations(nbField);

  for(Integer iField=0;iField<nbField;++iField)
    {
      for(Alien::IIndexManager::EntryEnumerator i = index_manager->enumerateEntry(); i.hasNext(); ++i)
        {
          if((*i).tagValue(block_tag) == m_field_tags[iField])
            {
              if (tagged_entries[i->getName()].null())
                {
                  error() << "Entry '" << i->getName() << "' is already tagged";
                  ++nerror;
                }
              else
                {
                  if (options()->verbose())
                    info() << "block-tag '" << m_field_tags[iField] << "' will tag entry '" << i->getName() << "'";
                  tagged_entries[i->getName()] = Alien::IIndexManager::Entry();
                  ConstArrayView<Integer> indices = i->getOwnIndexes();
                  field_sizes[iField] += indices.size() ;
                  field_associations[iField].add(*i);
                }
            }
        }
      if (field_sizes[iField] == 0)
        {
          error() << "No entry found for block-tag '" << m_field_tags[iField] << "'";
          ++nerror;
        }
    }

  // Actuellement les Entry sans tag sont des erreurs sauf s'il existe un default-block solver.
  bool has_default_block = (options()->defaultBlock.size() > 0);
  Integer default_field_size = 0;
  SharedArray<Alien::IIndexManager::Entry> default_block_field_association;
  for(std::map<String, Alien::IIndexManager::Entry>::const_iterator i=tagged_entries.begin(); i != tagged_entries.end(); ++i)
    {
      if (not i->second.null())
        {
          if (has_default_block)
            {
              ConstArrayView<Integer> indices = i->second.getOwnIndexes();
              default_field_size += indices.size();
              if (options()->verbose())
                info() << "Entry '" << i->first << "' has been tagged as default block";
              default_block_field_association.add(i->second);
            }
          else
            {
              error() << "Entry '" << i->first << "' has not been tagged";
              ++nerror;
            }
        }
    }

  if (has_default_block and default_field_size > 0)
    {
      m_field_tags.add(m_default_block_tag);
      nbField++;
      field_sizes.add(default_field_size);
      field_associations.add(default_block_field_association);
    }
  else
    {
      has_default_block = false;
    }

  if (nerror != 0) return nerror;

  m_field_petsc_indices.resize(nbField);
  SharedArray<Integer> current_field_indices ;
  for(Integer iField=0;iField<nbField;++iField)
    {
      current_field_indices.resize(0);
      current_field_indices.reserve(field_sizes[iField]) ;
      if (options()->verbose())
        info() << "block-tag '" <<m_field_tags[iField] << "' has " << field_sizes[iField] << " indices";
      const Array<Alien::IIndexManager::Entry> & entries = field_associations[iField];
      for(Integer j=0;j<entries.size();++j)
        {
          ConstArrayView<Integer> indices = entries[j].getOwnIndexes();
          current_field_indices.addRange(indices);
        }
      std::sort(current_field_indices.begin(), current_field_indices.end()); // PETSc requires index ordering
#if ((PETSC_VERSION_MAJOR <= 3 && PETSC_VERSION_MINOR < 3) || (PETSC_VERSION_MAJOR < 3))
      checkError("Create IndexSet", ISCreateGeneral(PETSC_COMM_WORLD, current_field_indices.size(), current_field_indices.unguardedBasePointer(),&m_field_petsc_indices[iField] ));
#else /* PETSC_VERSION */
#warning "TODO OPTIM: using other copy mode may be more efficient"
      checkError("Create IndexSet", ISCreateGeneral(PETSC_COMM_WORLD, current_field_indices.size(), current_field_indices.unguardedBasePointer(),PETSC_COPY_VALUES, &m_field_petsc_indices[iField] ));
#endif /* PETSC_VERSION */
    }

  return nerror ;
}

/*---------------------------------------------------------------------------*/

void 
PETScPrecConfigFieldSplitService::
configure(PC & pc, const Alien::IIndexManager * indexManager)
{
  checkError("Set preconditioner",PCSetType(pc,PCFIELDSPLIT));
  checkError("Build FieldSplit IndexSet",initializeFields(indexManager));

  const Integer nbFields = m_field_petsc_indices.size();
  ARCANE_ASSERT((not m_field_petsc_indices.empty()),("Unexpected empty PETSc IS for FieldSplit"));

  for(Integer i=0;i<nbFields;++i)
    {
#if ((PETSC_VERSION_MAJOR <= 3 && PETSC_VERSION_MINOR < 3) || (PETSC_VERSION_MAJOR < 3))
      checkError("Set PetscIS",PCFieldSplitSetIS(pc, m_field_petsc_indices[i]));
#else /* PETSC_VERSION */
      checkError("Set PetscIS",PCFieldSplitSetIS(pc, m_field_tags[i].localstr(),m_field_petsc_indices[i]));
#endif /* PETSC_VERSION */
    }

  // Configure type of FieldSplit decomposition
  checkError("Set FieldSplit type",options()->type()->configure(pc, nbFields));
  PCSetUp(pc) ;

  Integer nbf;
  KSP* subksp ;
  checkError("Get FieldSplit Sub KSP",PCFieldSplitGetSubKSP(pc,&nbf,&subksp));
  if(nbf!=nbFields)
    fatal()<<"Inconsistent number of split : user=" << nbFields << " PETSc=" << nbf;

  for(Integer i=0;i<nbFields;++i)
    {
      if (m_field_tags[i] == m_default_block_tag)
        options()->defaultBlock[0]->solver()->configure(subksp[i], NULL);
      else        
        options()->block[i]->solver()->configure(subksp[i], NULL);
    }
}

/*---------------------------------------------------------------------------*/

bool
PETScPrecConfigFieldSplitService::
hasParallelSupport() const
{
  bool has_parallel_support = true;
  
  for(Integer i=0;i<options()->block.size();++i)
    {
      has_parallel_support &= options()->block[i]->solver()->hasParallelSupport();
    }

  if (options()->defaultBlock.size() > 0)
    {
      has_parallel_support &= options()->defaultBlock[0]->solver()->hasParallelSupport();
    }

  return has_parallel_support;
}

/*---------------------------------------------------------------------------*/

bool
PETScPrecConfigFieldSplitService::
needPrematureKSPSetUp() const
{
  return true;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_SERVICE_PETSCPRECCONFIGFIELDSPLIT(FieldSplit,PETScPrecConfigFieldSplitService);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

//END_LINEARALGEBRA2SERVICE_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

