#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ class StMemStat-;
#pragma link C++ class TTreeHelper-;
#pragma link C++ class THelixTrack-;
#pragma link C++ class THack-;
#pragma link C++ class TUnixTime;
#pragma link C++ class TDirIter;
//#pragma link C++ class TAssign;
//#pragma link C++ class TAssMtx;
//#pragma link C++ class TAssMtxSimple;
//#pragma link C++ class TAssMtxObj;
#pragma link C++ class TRArray+;
#pragma link C++ class TRMatrix+;
#pragma link C++ class TRSymMatrix+;
#pragma link C++ class TRVector+;

#pragma link C++ enum TRArray::ETRMatrixType;
#pragma link C++ enum TRArray::ETRMatrixCreatorsOp;

#pragma link C++ function operator-=(TRArray &, const Double_t);
#pragma link C++ function operator+=(TRArray &, const Double_t);
#pragma link C++ function operator*=(TRArray &, const Double_t);
#pragma link C++ function operator*(const TRArray &, const TRArray &);
#pragma link C++ function operator-=(TRArray &, const TRArray &);
#pragma link C++ function operator+=(TRArray &, const TRArray &);
#pragma link C++ function operator==(TRArray &, const Double_t &);
#pragma link C++ function operator==(TRArray &, const TRArray &);
#pragma link C++ function operator<<(ostream &, const TRArray &);
#pragma link C++ function operator>>(istream &, TRArray &);

#pragma link C++ function operator<<(ostream &, const TRMatrix &);
#pragma link C++ function operator<<(ostream &, const TRSymMatrix &);
#pragma link C++ function operator<<(ostream &, const TRVector &);

#pragma link C++ function operator*(const TRMatrix &, Double_t);
#pragma link C++ function operator*(Double_t,const TRMatrix &);
#pragma link C++ function operator/(const TRMatrix &, Double_t);
#pragma link C++ function operator+(const TRMatrix &, Double_t);
#pragma link C++ function operator+(Double_t,const TRMatrix &);
#pragma link C++ function operator-(const TRMatrix &, Double_t);
#pragma link C++ function operator-(Double_t,const TRMatrix &);

#pragma link C++ function operator*(const TRVector &, Double_t);
#pragma link C++ function operator*(Double_t,const TRVector &);
#pragma link C++ function operator/(const TRVector &, Double_t);
#pragma link C++ function operator+(const TRVector &, Double_t);
#pragma link C++ function operator+(Double_t,const TRVector &);
#pragma link C++ function operator-(const TRVector &, Double_t);
#pragma link C++ function operator-(Double_t,const TRVector &);

#pragma link C++ class StCheckQtEnv;
#pragma link C++ class StMultiKeyMap;
#pragma link C++ class StMultiKeyNode;
#pragma link C++ class StMultiKeyMapIter;
#pragma link C++ class StDraw3DStyle;
#pragma link C++ namespace StarRoot;
#pragma link C++ class StarRoot::StEta;
#pragma link C++ enum  EDraw3DStyle;
//IncFile=TPolynomial.h
#pragma link C++ namespace TPolynomial;
#pragma link C++ class KFParticleBase+;
#pragma link C++ class KFParticle+;
#pragma link C++ class MTrack+;
#pragma link C++ class MVertex+;
#pragma link C++ class TTreeIter-;
#pragma link C++ class TTreeIterCast-;

#pragma link C++ class TPolinom+;
#pragma link C++ class TPoliFitter+;
#pragma link C++ class TRandomVector+;
#pragma link C++ class TNumDeriv+;
#pragma link C++ class TNumDeriv1Test+;
#pragma link C++ class TNumDeriv2Test+;
#pragma link C++ class StDraw3D+;
#pragma link C++ class TF1Fitter+;
#pragma link C++ class TF1GausFitter+;
#pragma link C++ class TF1TwoGausFitter+;
#pragma link C++ class TF1GausPol2Fitter+;
#pragma link C++ class TCircle+;
#pragma link C++ class TCircleFitter+;
#pragma link C++ class THelixFitter+;
#pragma link C++ class THelixKFitter+;
#pragma link C++ class TRDiagMatrix+;
#pragma link C++ class StCloseFileOnTerminate+;
#pragma link C++ class StNotifyOnTerminate+;
#pragma link C++ class KFVertex+;
#pragma link C++ class TMDFParameters+;
#pragma link C++ class StArchInfo+;
#pragma link C++ class TCFit+;
#pragma link C++ class TCFitData+;
#pragma link C++ class TCFitV0+;
#pragma link C++ class VVertex+;
#pragma link C++ class TH1Helper+;
#pragma link C++ class TAttr+;

#endif
