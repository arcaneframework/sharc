#ifndef OLBOUNDARYCONDITIONTYPES_H
#define OLBOUNDARYCONDITIONTYPES_H


struct TypesDivKGradOLGSchemes {
  enum eType {
    BC_TwoPoints,         //! Modified two point method for boundary faces to recover consistency on CPG grids
    BC_StandardTwoPoints, //! Standard two point method for boundary faces
    BC_Oscheme,           //! O method for boundary faces
    BC_Lscheme,           //! L method for boundary faces
    BC_Gscheme            //! G method for boundary faces
  };
};

#endif
