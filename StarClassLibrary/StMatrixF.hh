/***************************************************************************
 *
 * $Id: StMatrixF.hh,v 1.10 2005/07/06 18:49:56 fisyak Exp $
 * $Log: StMatrixF.hh,v $
 * Revision 1.10  2005/07/06 18:49:56  fisyak
 * Replace StHelixD, StLorentzVectorD,StLorentzVectorF,StMatrixD,StMatrixF,StPhysicalHelixD,StThreeVectorD,StThreeVectorF by templated version
 *

****************************************************************************/
#ifndef ST_MATRIX_F_HH
#define ST_MATRIX_F_HH
#include "St_base/Stiostream.h"
#include "StarClassLibrary/StThreeVectorF.hh"
#include "StarClassLibrary/StLorentzVectorF.hh"
#include "StarClassLibrary/StThreeVectorD.hh"
#include "StarClassLibrary/StLorentzVectorD.hh"
#include "StarClassLibrary/StMatrix.hh"
typedef  StMatrix<float> StMatrixF;
#endif
