// $Id: StMultiH1F.h,v 1.2 2000/08/25 15:46:42 genevb Exp $
// $Log: StMultiH1F.h,v $
// Revision 1.2  2000/08/25 15:46:42  genevb
// Added stats box, legend names
//
// Revision 1.1  2000/07/26 22:00:28  lansdell
// new multi-hist class for superimposing the x-projections of y-bins (of a TH2F histogram) into one TH1F histogram
//

#ifndef ClassStMultiH1F
#define ClassStMultiH1F

#include "TH2.h"
class TString;

class StMultiH1F : public TH2F {
 public:
  StMultiH1F();
  StMultiH1F(const char *name,const char *title,Int_t nbinsx,Axis_t xlow,
	     Axis_t xup,Int_t nbinsy);
  StMultiH1F(const char *name,const char *title,Int_t nbinsx,Double_t *xbins,
	     Int_t nbinsy);
  virtual ~StMultiH1F() { delete names; }
  virtual        void Draw(Option_t *option="");
  virtual        void SetNames(Int_t   ybin, const char* name)
                              { names[ybin] = name; }
  virtual        void SetNames(Float_t ybin, const char* name)
                              { SetNames((Int_t) ybin, name); }
  virtual const char* GetNames(Int_t   ybin) const
                              { return names[ybin].Data(); }
  virtual const char* GetNames(Float_t ybin) const
                              { return GetNames((Int_t) ybin); }
  // Overload the Rebin() function to allow naming of y bins with TH2F pointer
  virtual        TH1* Rebin(Int_t ngroup, const char* newname)
                              { SetNames(ngroup, newname); return 0; }
 protected:
  TString names[10];
  ClassDef(StMultiH1F,1)
};

#endif
