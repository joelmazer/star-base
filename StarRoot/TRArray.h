#ifndef ROOT_TRArray
#define ROOT_TRArray
#include <assert.h>
#include "Riostream.h"
#include "Rstrstream.h"
#include "Rtypes.h" 
#include "TObject.h"
#include "TArrayD.h"
#include "TMath.h"
#include "TCL.h"
#ifndef __CINT__
#define __VA_LIST__(name) \
  va_list args;     \
  va_start(args,va_(name)); \
  for (Int_t num=0; num<fN; num++) { \
    if (! num) fArray[0] = name; \
    else fArray[num] = (Double_t) va_arg(args, Double_t); \
  } \
  va_end(args); 
#endif



class TRArray : public TArrayD {
 public:
  enum ETRMatrixType {kUndefined, kVector, kRectangular, kSemiPosDefinedSymMatrix};
  enum ETRMatrixCreatorsOp { kZero, kUnit, kTransposed, kInverted, kInvertedPosDef,
			     kMult, 
			     kAxB, kAxBT, kATxB, kATxBT,
			     kAxBxAT, kATxBxA, 
			     kSxA, kAxS, kSxAT, kATxS,
			     kAxAT, kATxA,
			     kAxSxAT, kATxSxA, kRxSxR
  };

  TRArray(Int_t N=0):  TArrayD(N), fValid(kTRUE) {}
  //  TRArray(Int_t N,Double_t scalar):  TArrayD(N) {if (scalar != 0) Reset(scalar);}
#ifndef __CINT__
  TRArray(Int_t N,Double_t a0, ...);
#endif
  TRArray(Int_t N,const Double_t *Array):  TArrayD(N,Array), fValid(kTRUE) {;}
  TRArray(Int_t N,const Float_t *Array);
  TRArray(const TRArray &A,const Double_t fA, TRArray &B,const Double_t fB): TArrayD(0), fValid(kTRUE)  {
    Int_t N = A.GetSize(); assert (N == B.GetSize()); Set(N); 
    TCL::vlinco(A.GetArray(),fA,B.GetArray(),fB,fArray,N);  
  }
  TRArray(Int_t N,const Char_t *s);  
  virtual ~TRArray() {;}

  virtual Int_t GetNrows()                           const {return GetSize();} 
  virtual Int_t GetNcols()                           const {return 1;}
  virtual ETRMatrixType GetMatrixType()              const {return kUndefined;}
  virtual Bool_t IsValid()                           const {return fValid;}
  virtual void   SetValid(Bool_t Valid=kTRUE)              {fValid = Valid;}
  virtual Double_t &operator()(Int_t i)                    {return operator[](i-1);}
  friend TRArray &operator-=(TRArray &target, const Double_t &scalar) {
    for (int i=0; i<target.fN; i++) target.fArray[i] -= scalar; return target;}
  friend TRArray &operator+=(TRArray &target, const Double_t &scalar) {
    for (int i=0; i<target.fN; i++) target.fArray[i] += scalar; return target;
  }
  friend Double_t operator*(const TRArray &target, const TRArray &source) {
    assert(target.fN == source.GetSize());
    Double_t sum = 0;
    const Double_t *sArray = source.GetArray();
    for (int i=0; i<target.fN; i++) sum += target.fArray[i]*sArray[i]; return sum;
  }
  friend TRArray &operator*=(TRArray &target, const Double_t &scalar) {
    for (int i=0; i<target.fN; i++) target.fArray[i] *= scalar; return target;
  }
  friend TRArray &operator/=(TRArray &target, const Double_t &scalar) {
    for (int i=0; i<target.fN; i++) target.fArray[i] /= scalar; return target;
  }
  friend TRArray &operator-=(TRArray &target, const TRArray &A) {
    assert(target.fN == A.GetSize());
    const Double_t *fA  = A.GetArray();
    for (int i=0; i<target.fN; i++) target.fArray[i] -= fA[i]; 
    return target;
  }
  friend TRArray &operator+=(TRArray &target, const TRArray &A) {
    assert(target.fN == A.GetSize());
    const Double_t *fA  = A.GetArray();
    for (int i=0; i<target.fN; i++) target.fArray[i] += fA[i]; 
    return target;
  }
  
  friend Bool_t operator==(TRArray &target, const Double_t &scalar) {
    for (int i=0; i<target.fN; i++) if (target.fArray[i] != scalar) return kFALSE; 
    return kTRUE;
  }
  friend Bool_t operator==(TRArray &target, const TRArray &A) {
    if (target.fN != A.GetSize()) return kFALSE; 
    const Double_t *fB  = A.GetArray();
    for (int i=0; i<target.fN; i++) if (target.fArray[i] != fB[i]) return kFALSE; return kTRUE;
  }
  
  Bool_t Verify(const TRArray &A, const Double_t zeru=1.e-7, Int_t Level=1) const;
  virtual void   Print(Option_t *opt="") const;
 protected:
  Bool_t fValid;
 public:
  ClassDef(TRArray,1)  // TRArray class (double precision)
};
ostream& operator<<(ostream& s,const TRArray &target);
istream & operator>>(istream &s, TRArray &target);
#endif