// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "ArcGeoSimPostProcessingBase.h"
/* Author : havep at Tue Dec 20 16:40:39 2011
 * Generated by createNew
 */

#include <arcane/VariableBuildInfo.h>
#include <arcane/MathUtils.h>
#include <arcane/IMeshMng.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Arcane::String ArcGeoSimPostProcessingBase::m_default_mesh_name = "default_mesh";

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ArcGeoSimPostProcessingBase::
ArcGeoSimPostProcessingBase(ISubDomain* sd, const String name)
  : m_sub_domain(sd)
  , m_has_post_processing(false)
  , m_times(VariableBuildInfo(sd, String::format("{0}ExportTimes",name)))
  , m_output_dir_created(false)
  , m_name(name)
{
//  m_post_processing_timer = new Timer(sd,String::format("{0}PostProcessorTimer",name),Timer::TimerReal);
}

/*---------------------------------------------------------------------------*/

ArcGeoSimPostProcessingBase::
~ArcGeoSimPostProcessingBase()
{
  ;
}

/*---------------------------------------------------------------------------*/
 
void 
ArcGeoSimPostProcessingBase::
init(Arcane::String output_path)
{
  m_output_path = output_path;
}

/*---------------------------------------------------------------------------*/

void 
ArcGeoSimPostProcessingBase::
saveAtTime(Real saved_time)
{
  _saveVariablesAtTime(saved_time, m_pp_variables);
}
/*---------------------------------------------------------------------------*/

void 
ArcGeoSimPostProcessingBase::
saveAtInitialTime(Real saved_time)
{
  _saveVariablesAtTime(saved_time, m_pp_init_variables);
}
/*---------------------------------------------------------------------------*/

void
ArcGeoSimPostProcessingBase::
_saveVariablesAtTime(Real saved_time, const Arcane::Array<Arcane::VariableList>& pp_variables)
{
  if (!m_has_post_processing) return;

  Integer size = m_times.size();

  IVariableMng* vm = m_sub_domain->variableMng();
  ITraceMng * traceMng = m_sub_domain->traceMng();

  // Ne sauvegarde pas si le temps actuel est le m���������me que le pr���������c���������dent
  // (Sinon ca fait planter Ensight...)
//#ifndef NO_USER_WARNING
//#ifndef WIN32
//#warning "BUG: ceci doit ���������tre un bug de post-processing suite ��������� une annulation de pas de temps"
//#endif
//#endif
  if (size!=0 && Arcane::math::isEqual(m_times[size-1],saved_time))
    return;

  m_times.resize(size+1);
  m_times[size] = saved_time;

  _checkCreateOutputDir();

  if (true /*m_do_output*/)
    {
      traceMng->info() << " ";
      traceMng->info() << "****  Sortie en cours au temps " << saved_time <<"  ******";

      // M���������morisation des ���������tats used et mise ��������� jour ���������ventuelle
      BoolSharedArray usedStates;
      for(Integer i=0;i<pp_variables.size();++i)
        for(VariableList::Enumerator j(pp_variables[i]); ++j; )
          {
            IVariable * var = *j;
            usedStates.add(var->isUsed());
            var->setUsed(true);
            if (var->computeFunction())
              {
                // var->update();
//#ifndef NO_USER_WARNING
//#ifndef WIN32
//#warning "TODO: Fixer cet appel brutal du foncteur"
//#endif
//#endif
                var->computeFunction()->execute();
              }
          }

      for(Integer ip=0,np=m_pp_writer.size();ip<np;++ip)
        {
          IPostProcessorWriter* writer = m_pp_writer[ip];
          writer->setMesh(m_pp_meshes[ip]);
          writer->setBaseDirectoryName(m_pp_base_directory_names[ip]);
          writer->setTimes(m_times);
          writer->setVariables(pp_variables[ip]);
          writer->setGroups(m_pp_groups[ip]);
          {
            Timer::Sentry ts(&m_pp_timer[ip]);
            vm->writePostProcessing(writer);
          }
        }

      // Restoration des flags used
      for(Integer i=0, index=0;i<pp_variables.size();++i)
        for(VariableList::Enumerator j(pp_variables[i]); ++j; ++index)
          (*j)->setUsed(usedStates[index]);
    }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
ArcGeoSimPostProcessingBase::
_checkCreateOutputDir()
{
  ARCANE_ASSERT((m_has_post_processing),("Illegal call when post-processing is not configured"));
  if (m_output_dir_created)
    return;
  m_output_directory = Directory(m_output_path);
  m_output_directory.createDirectory();
  m_output_directory = Directory(m_output_directory,"depouillement");
  m_output_directory.createDirectory();
  m_output_dir_created = true;
  for (Arcane::Integer i = 0; i < m_pp_meshes.size();++i)
    {
      if (m_pp_meshes[i]->name() != m_sub_domain->defaultMesh()->name())
        {
          Directory sub_dir(m_output_directory,Arcane::String::format("depouillement_{0}",m_pp_meshes[i]->name()));
          sub_dir.createDirectory();
          m_pp_base_directory_names.add(sub_dir.path());
        }
      else m_pp_base_directory_names.add(m_output_directory.path());
    }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
ArcGeoSimPostProcessingBase::
stats()
{
  if (!m_has_post_processing) return;

  ITraceMng * traceMng = m_sub_domain->traceMng();

  for(int i=0;i<m_pp_timer.size();++i)
  {
    Real total_time = m_pp_timer[i].totalTime();
    traceMng->info() << "Temps total pour les sorties de dépouillement "<< m_pp_timer[i].name() << " (seconde): " << total_time;
    Integer nb_time = m_pp_timer[i].nbActivated();
    if (nb_time!=0)
      traceMng->info() << "Temps moyen par sortie (seconde): " << total_time / nb_time
      << " (pour " << nb_time << " sorties)";
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
void
ArcGeoSimPostProcessingBase::
_addPostProcessingMesh(const Arcane::String& mesh_name)
{
  if (mesh_name == m_default_mesh_name) m_pp_meshes.add(m_sub_domain->defaultMesh());
  else
    {
      bool throw_exception = true;
      m_pp_meshes.add(m_sub_domain->meshMng()->findMeshHandle(mesh_name,throw_exception)->mesh());
    }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
