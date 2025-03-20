#ifndef MESHPROPERTY_H_
#define MESHPROPERTY_H_


BEGIN_ARCGEOSIM_NAMESPACE

BEGIN_MESH_NAMESPACE

class MeshProperty
{
public:
  /** Constructeur de la classe */
  MeshProperty() {} ;
  /** Destructeur de la classe */
  virtual ~MeshProperty() {} ;

public:
  typedef enum { 
    Top,
    Bottom
  } eLayerType ;
};

END_MESH_NAMESPACE

END_ARCGEOSIM_NAMESPACE
#endif /*MESHPROPERTY_H_*/
