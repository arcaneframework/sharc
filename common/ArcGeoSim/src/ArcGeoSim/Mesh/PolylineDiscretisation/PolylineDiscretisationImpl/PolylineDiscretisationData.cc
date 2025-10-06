#include "PolylineDiscretisationData.h"


/*---------------------------------------------------------------------------*/
void PolylineDiscretisationData::clear()
{
  m_res_segment_idx.clear();
  m_res_cell_idx.clear();
  m_res_coord_inter.clear();
}

/*---------------------------------------------------------------------------*/
void PolylineDiscretisationData::add(Integer segment, Integer cell, Real coord)
{
  Integer size = m_res_cell_idx.size();

  m_res_segment_idx.add(segment);
  m_res_cell_idx.add(cell);
  m_res_coord_inter.add(coord);

  // tri des points suivant selon leur coordonnees sur le segment (MD croissant)
  if (size > 1 && cell != -1)
  {
    Integer i = 1;
    // determine la position d'insertion du nouveau point
    for (Integer k = size - 1; k > 0; k--)
    {
      // on ajoute le point apres le point d'absisse curviligne immediatement inferieur
      if (coord >= m_res_coord_inter[k] || m_res_cell_idx[k] == -1)
      {
        // si le point d'insertion trouve est un point d'entree dans une maille alors le point a inserer
        // est un point de sortie a positionner juste avant
        if ((k < size - 1) && m_res_cell_idx[k] == m_res_cell_idx[k + 1])
        {
          i = k;
        }
        else
          // sinon on suppose que le point d'insertion trouve est un point de sortie dans une maille alors le point a inserer
          // est un point d'entree a positionner juste apres
          // dans le cas contraire, on permuttera apres
        {
          i = k + 1;
        }
        break;
      }
      else if (cell == m_res_cell_idx[k] && (m_res_coord_inter[k]==m_res_coord_inter[k-1]))
      {
        // le point de sortie a deja ete trouve : l'hypothese ci dessus etait fausse pour le point k  : permutation k et k-1
        Integer tmp_int = m_res_cell_idx[k];
        m_res_cell_idx[k] =  m_res_cell_idx[k-1];
        m_res_cell_idx[k-1] = tmp_int;
      }
    }
    // insertion du nouveau point et decallage des points suivants
    if (i < size)
    {
      for (Integer j = size; j > i; j--)
      {
        m_res_segment_idx[j] = m_res_segment_idx[j-1];
        m_res_cell_idx[j] = m_res_cell_idx[j-1];
        m_res_coord_inter[j] = m_res_coord_inter[j-1];
      }
      m_res_segment_idx[i] = segment;
      m_res_cell_idx[i] = cell;
      m_res_coord_inter[i] = coord;
    }
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

