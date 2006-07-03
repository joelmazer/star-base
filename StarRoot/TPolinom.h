#ifndef TPOLINOM_H
#define TPOLINOM_H
#include "TObject.h"
#include "TArrayD.h"

class TPolinom : public TObject
{
public:
  TPolinom(int npw=-1,const double *coefs=0);
  TPolinom(const TPolinom &from);
  TPolinom(double c0);
  TPolinom(double c0,double c1);
  TPolinom(double c0,double c1,double c2);
TPolinom &operator=(const TPolinom &from);
virtual ~TPolinom();
   void  Backward();
virtual void   Print(const char* chopt = "") const;
virtual void   Clear(const char *opt ="");
  double Eval (double x) const;
  double operator()(double x) const 	{return Eval(x);}
  double Evrr (double x) const;
  double Deriv(double x) const;
  void   Move(double x);
  const double *Coe() const 		{return fCoe;}
  const double *Emx()      const 	{return fEmx;}
        double  GetEmx(int i,int j) const; 		
        void    SetCoefs(int npw=0,const double *coefs=0);
        void    SetCoeff(int idx,double val)	{fCoe[idx]=val;}
//	statics
static double Eval (double x,int n,double *coe);


protected:
int  fNP;
double *fCoe;
double *fEmx;
double f2Coe[3];
ClassDef(TPolinom,0)
};


class TPoliFitter: public TPolinom
{
public:
       TPoliFitter(int np);
       TPoliFitter(const TPoliFitter &from);
virtual ~TPoliFitter(){;}
void   Add(double x, double y,double err2=1);
void   AddErr(double err2);
void   Skip(int idx);
const double *GetX(int i=0) const; 	
      double *GetX(int i=0); 		
double Fit();   
void   MakeErrs();
double Chi2() const 			{return fChi2;}
int    Ndf()  const 			{return fNdf ;}
double Wtot() const                     {return fWtot;}
const double *CoeOrt() const		{return fC   ;}
double EvalOrt(int idx,double x) const;

double EvalChi2();
void   Clear(const char *opt ="");
void   Print(const char* chopt = "") const;
void   FixY0(double y);
void   DCoeDy(int iy,double *dcoe);
static void Test(int kase=0);
static void TestCorr();
static void Dest(int kase=0);
private:
void   Prepare();

private:
TArrayD fArr;
char   fBeg[1];
int fN;
int fNuse;
double *fDat;
double *fP;
double *fC;
double fWtot;
double fChi2;
int    fNdf;
char   fEnd[1];
ClassDef(TPoliFitter,0)
};

#endif // TPOLINOM_H