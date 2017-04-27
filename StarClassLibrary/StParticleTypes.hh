/***************************************************************************
 *
 * $Id: StParticleTypes.hh,v 1.3 2014/06/25 14:19:24 jwebb Exp $
 *
 * Author: Thomas Ullrich, May 99 (based on Geant4 code, see below) 
 ***************************************************************************
 *
 * The design of the StParticleDefinition class and all concrete
 * classes derived from it is largely based on the design of the 
 * G4ParticleDefinition class from Geant4 (RD44).
 * Although the code is in large parts different (modified or rewritten)
 * and adapted to the STAR framework the basic idea stays the same.
 *
 ***************************************************************************
 *
 * $Log: StParticleTypes.hh,v $
 * Revision 1.3  2014/06/25 14:19:24  jwebb
 * Added psi prime --> e+e-
 *
 * Revision 1.2  2000/04/06 22:25:42  ullrich
 * Added phi and omega. More STAR specific Geant IDs.
 *
 * Revision 1.1  1999/05/14 18:50:01  ullrich
 * Initial Revision
 *
 **************************************************************************/
#ifndef StParticleTypes_hh
#define StParticleTypes_hh

// Bosons
#include "StarClassLibrary/StGamma.hh"
#include "StarClassLibrary/StOpticalPhoton.hh"

// Leptons
#include "StarClassLibrary/StAntiNeutrinoE.hh"
#include "StarClassLibrary/StAntiNeutrinoMu.hh"
#include "StarClassLibrary/StAntiNeutrinoTau.hh"
#include "StarClassLibrary/StElectron.hh"
#include "StarClassLibrary/StMuonMinus.hh"
#include "StarClassLibrary/StMuonPlus.hh"
#include "StarClassLibrary/StNeutrinoE.hh"
#include "StarClassLibrary/StNeutrinoMu.hh"
#include "StarClassLibrary/StNeutrinoTau.hh"
#include "StarClassLibrary/StPositron.hh"
#include "StarClassLibrary/StTauMinus.hh"
#include "StarClassLibrary/StTauPlus.hh"

// Mesons
#include "StarClassLibrary/StAntiBMesonZero.hh"
#include "StarClassLibrary/StAntiBsMesonZero.hh"
#include "StarClassLibrary/StAntiDMesonZero.hh"
#include "StarClassLibrary/StAntiKaonZero.hh"
#include "StarClassLibrary/StBMesonMinus.hh"
#include "StarClassLibrary/StBMesonPlus.hh"
#include "StarClassLibrary/StBMesonZero.hh"
#include "StarClassLibrary/StBsMesonZero.hh"
#include "StarClassLibrary/StDMesonMinus.hh"
#include "StarClassLibrary/StDMesonPlus.hh"
#include "StarClassLibrary/StDMesonZero.hh"
#include "StarClassLibrary/StDsMesonMinus.hh"
#include "StarClassLibrary/StDsMesonPlus.hh"
#include "StarClassLibrary/StEta.hh"
#include "StarClassLibrary/StEtaPrime.hh"
#include "StarClassLibrary/StJPsi.hh"
#include "StarClassLibrary/StPsi2s.hh"
#include "StarClassLibrary/StKaonMinus.hh"
#include "StarClassLibrary/StKaonPlus.hh"
#include "StarClassLibrary/StKaonZero.hh"
#include "StarClassLibrary/StKaonZeroLong.hh"
#include "StarClassLibrary/StKaonZeroShort.hh"
#include "StarClassLibrary/StOmegaMeson.hh"
#include "StarClassLibrary/StPhi.hh"
#include "StarClassLibrary/StPionMinus.hh"
#include "StarClassLibrary/StPionPlus.hh"
#include "StarClassLibrary/StPionZero.hh"
#include "StarClassLibrary/StRhoMinus.hh"
#include "StarClassLibrary/StRhoPlus.hh"
#include "StarClassLibrary/StRhoZero.hh"

// Baryons
#include "StarClassLibrary/StAntiLambda.hh"
#include "StarClassLibrary/StAntiLambdacPlus.hh"
#include "StarClassLibrary/StAntiNeutron.hh"
#include "StarClassLibrary/StAntiOmegaMinus.hh"
#include "StarClassLibrary/StAntiOmegacZero.hh"
#include "StarClassLibrary/StAntiProton.hh"
#include "StarClassLibrary/StAntiSigmaMinus.hh"
#include "StarClassLibrary/StAntiSigmaPlus.hh"
#include "StarClassLibrary/StAntiSigmaZero.hh"
#include "StarClassLibrary/StAntiSigmacPlus.hh"
#include "StarClassLibrary/StAntiSigmacPlusPlus.hh"
#include "StarClassLibrary/StAntiSigmacZero.hh"
#include "StarClassLibrary/StAntiXiMinus.hh"
#include "StarClassLibrary/StAntiXiZero.hh"
#include "StarClassLibrary/StAntiXicPlus.hh"
#include "StarClassLibrary/StAntiXicZero.hh"
#include "StarClassLibrary/StLambda.hh"
#include "StarClassLibrary/StLambdacPlus.hh"
#include "StarClassLibrary/StNeutron.hh"
#include "StarClassLibrary/StOmegaMinus.hh"
#include "StarClassLibrary/StOmegacZero.hh"
#include "StarClassLibrary/StProton.hh"
#include "StarClassLibrary/StSigmaMinus.hh"
#include "StarClassLibrary/StSigmaPlus.hh"
#include "StarClassLibrary/StSigmaZero.hh"
#include "StarClassLibrary/StSigmacPlus.hh"
#include "StarClassLibrary/StSigmacPlusPlus.hh"
#include "StarClassLibrary/StSigmacZero.hh"
#include "StarClassLibrary/StXiMinus.hh"
#include "StarClassLibrary/StXiZero.hh"
#include "StarClassLibrary/StXicPlus.hh"
#include "StarClassLibrary/StXicZero.hh"

// Ions
#include "StarClassLibrary/StAlpha.hh"
#include "StarClassLibrary/StDeuteron.hh"
#include "StarClassLibrary/StHe3.hh"
#include "StarClassLibrary/StTriton.hh"

#endif
