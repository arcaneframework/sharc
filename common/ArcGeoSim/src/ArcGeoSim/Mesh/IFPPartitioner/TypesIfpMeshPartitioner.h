/*
 * TypesIfpMeshPartitioner.h
 *
 *  Created on: Nov 3, 2009
 *      Author: gratienj
 */

#ifndef TYPESIFPMESHPARTITIONER_H_
#define TYPESIFPMESHPARTITIONER_H_


struct TypesIfpMeshPartitioner
{
  typedef enum
  {
    X,
    Y,
    OneD,
    TwoD,
    TwoDMetis,
    ThreeDMetis
   } eCutType;
};

#endif /* TYPESIFPMESHPARTITIONER_H_ */
