//////////////////////////////////////////////////////////
//   This class has been automatically generated 
//     (Thu Nov 29 16:25:03 2001 by ROOT version3.02/01)
//   from TTree Bichsel/H.Bichel Calculation Summary
//   found on file: dEdx2T.root
//////////////////////////////////////////////////////////


#ifndef Bichsel_h
#define Bichsel_h
#define P03ia
#include "dEdxParameterization.h"
class Bichsel {
 private: 
  static dEdxParameterization *m_dEdxParameterization; //!
 public :
  Bichsel();
 virtual ~Bichsel() {}
  Double_t    GetMostProbableZ(Double_t log10bg, Double_t log2dx, Int_t kase=0) {
    return m_dEdxParameterization->GetMostProbableZ(log10bg,log2dx,kase);
  }
  Double_t    GetAverageZ(Double_t log10bg, Double_t log2dx, Int_t kase=0) {
    return m_dEdxParameterization->GetAverageZ(log10bg,log2dx,kase);
  }
  Double_t    GetRmsZ(Double_t log10bg, Double_t log2dx, Int_t kase=0) {
    return m_dEdxParameterization->GetRmsZ(log10bg,log2dx,kase);
  }
  Double_t    GetI70(Double_t log10bg, Double_t log2dx, Int_t kase=0);
  Double_t    GetI60(Double_t log10bg, Double_t log2dx, Int_t kase=0)  {
    return m_dEdxParameterization->GetI60(log10bg,log2dx,kase);
  }
  Double_t    GetMostProbabledEdx(Double_t log10bg, Double_t log2dx, Int_t kase = 0) {
    return m_dEdxParameterization->GetMostProbabledEdx(log10bg,log2dx,kase);
  }
  Double_t    GetdEdxWidth(Double_t log10bg, Double_t log2dx, Int_t kase=0) {
    return m_dEdxParameterization->GetdEdxWidth(log10bg,log2dx,kase);
  }
  Double_t    GetProbability(Double_t log10bg, Double_t log2dx, Double_t z, Int_t kase=0) {
    return m_dEdxParameterization->GetProbability(log10bg,log2dx,z,kase);}
  Double_t    TofCorrection(Double_t log10bg);
  ClassDef(Bichsel,0)
};
#endif
