// $Id: StMultiH1F.cxx,v 1.6 2000/09/15 21:23:36 fisyak Exp $
// $Log: StMultiH1F.cxx,v $
// Revision 1.6  2000/09/15 21:23:36  fisyak
// HP does not have iostream
//
// Revision 1.5  2000/08/28 19:21:05  genevb
// Improved projection code
//
// Revision 1.4  2000/08/28 18:47:50  genevb
// Better handling of 1 y-bin case
//
// Revision 1.3  2000/08/25 22:03:39  genevb
// Fixed entries problem
//
// Revision 1.2  2000/08/25 15:46:42  genevb
// Added stats box, legend names
//
// Revision 1.1  2000/07/26 22:00:27  lansdell
// new multi-hist class for superimposing the x-projections of y-bins (of a TH2F histogram) into one TH1F histogram
//
#ifdef __HP_aCC
#include <iostream.h>
#else
#include <iostream>
#endif
#include "StMultiH1F.h"
#include "TString.h"
#include "TLegend.h"
#include "TPad.h"

ClassImp(StMultiH1F)

StMultiH1F::StMultiH1F() {}

StMultiH1F::StMultiH1F(const char *name,const char *title,Int_t nbinsx,
		       Axis_t xlow,Axis_t xup ,Int_t nbinsy) :
  TH2F(name,title,nbinsx,xlow,xup,nbinsy,-0.5,-0.5+nbinsy) {}

StMultiH1F::StMultiH1F(const char *name,const char *title,Int_t nbinsx,
		       Double_t *xbins,Int_t nbinsy) :
  TH2F(name,title,nbinsx,xbins,nbinsy,-0.5,-0.5+nbinsy) {}

void StMultiH1F::Draw(Option_t *option) {

  Int_t ybins = GetNbinsY();
  if (ybins == 1) {
    TH1F* temp0 = XProjection(GetName());
    temp0->SetStats((!TestBit(kNoStats)));
    temp0->Draw();
    return;
  }

  // overlay the y bins of the 2d histogram into a 1d histogram
  // using different line styles

  // make a legend
  TLegend *legend = new TLegend(0.80,0.84,0.98,0.98,"Legend","NDC");
  legend->SetFillColor(0);
  legend->SetFillStyle(4000);
  legend->SetMargin(0.25);

  Int_t ybin;
  Double_t maxval = -999999.;
  Int_t maxbin = -1;

  // dummy histogram pointer
  TH1F** temp = new TH1F*[ybins];

  TString n0;
  for (ybin=0; ybin<ybins; ybin++) {
    if ((ybin >= 10) || (names[ybin].IsNull())) n0 = GetName();
    else n0 = names[ybin];
    Int_t slice = ybin+1;
    temp[ybin] = XProjection(n0.Data(),slice);
    temp[ybin]->SetLineStyle(slice);
    temp[ybin]->SetStats(kFALSE);
    Double_t binmax = temp[ybin]->GetMaximum();
    if (binmax > maxval) {
      maxval = binmax;
      maxbin = ybin;
    }
    legend->AddEntry(temp[ybin],n0.Data(),"l");
  }

  // can't use the option argument in Draw() since this is called from
  // StHistUtil::DrawHists(), which defaults 2D histograms to a box plot
  temp[maxbin]->Draw();
  for (ybin=0; ybin<ybins; ybin++) {
    if (ybin != maxbin) temp[ybin]->Draw("same");
  }

  // Draw statistics for full set if stats are turned on
  if (!TestBit(kNoStats)) {
    temp[0] = XProjection(GetName());
    temp[0]->SetEntries(GetEntries());
    temp[0]->SetStats(kTRUE);
    temp[0]->Draw("boxsames");
    legend->SetX1(0.59);
    legend->SetX2(0.77);
  }

  legend->Draw();
}

TH1F* StMultiH1F::XProjection(const char* name, Int_t ybin) {
  TH1F* temp=0;
  if (ybin<0) temp = (TH1F*) ProjectionX(name);
  else temp = (TH1F*) ProjectionX(name,ybin,ybin);
  TAttLine::Copy(*temp);
  TAttFill::Copy(*temp);
  TAttMarker::Copy(*temp);
  return temp;
}