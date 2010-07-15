#include <stdlib.h>
#include <math.h>
#include "TError.h"
#include "TArrayD.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TSystem.h"
#include "TMath.h"
#if ROOT_VERSION_CODE < 331013
#include "TCL.h"
#else
#include "TCernLib.h"
#endif
#include "TRandom.h"
#include "TRandom2.h"
#include "THelixTrack.h"
#include "StMatrixD.hh"
#include "TComplex.h"
#include "TH1.h"
#include <cassert>

// Complex numbers
const TComplex Im(0,1);
//_____________________________________________________________________________
inline static double dot(const TComplex &a,const TComplex &b)
{return a.Re()*b.Re()+a.Im()*b.Im();}
//_____________________________________________________________________________
inline static TComplex expOne(TComplex x)
{
  double a = TComplex::Abs(x);
  if (a<0.01) {
    return 1.+x*((1/2.) + x*((1/6.)+ x*(1/24.)));
  } else {
    return (TComplex::Exp(x)-1.)/x;
  }
}
//_____________________________________________________________________________
inline static TComplex expOneD(TComplex x)
{
  double a = TComplex::Abs(x);
  if (a<0.01) {
    return (1/2. + x*((1/6.)+ x*((1/24.)+x*(1/120.))));
  } else {
    return (TComplex::Exp(x)-1.-x)/(x*x);
  }
}

//______________________________________________________________________________
void TCEmx_t::Set(const double *err)  	
{ if (err) {memcpy(this,err,sizeof(*this));} else {Clear();}}

//______________________________________________________________________________
void TCEmx_t::Move(const double F[3][3])
{
  assert(mHH);
  double oErr[6];
  memcpy(oErr,Arr(),sizeof(oErr));
  TCL::trasat(F[0],oErr,Arr(),3,3); 
}
//______________________________________________________________________________
void TCEmx_t::Backward()
{
  mHA*=-1; mAC*=-1; 
}

//______________________________________________________________________________
void THEmx_t::Set(const double *errxy,const double *errz)
{
  Clear();
  memcpy(&mHH,errxy,sizeof(mHH)*6);
  mZZ = errz[0]; mZL =errz[1]; mLL =errz[2];
}
//______________________________________________________________________________
void THEmx_t::Set(const double *err)  	
{ if (err) {memcpy(this,err,sizeof(*this));} else {Clear();}}
//_____________________________________________________________________________
void THEmx_t::Backward()
{
  mHA*=-1; mAC*=-1; mHZ*=-1; mCZ*=-1; mAL*=-1; mZL*=-1;
}
//______________________________________________________________________________
void THEmx_t::Move(const double F[5][5])
{
  assert(mHH);
  double oErr[15];
  memcpy(oErr,Arr(),sizeof(oErr));
  TCL::trasat(F[0],oErr,Arr(),5,5); 
}

const double Zero = 1.e-6;
static TComplex sgCX1,sgCX2,sgCD1,sgCD2,sgImTet,sgCOne,sgCf1;
static  int  SqEqu(double *, double *);
#if 0
//_____________________________________________________________________________
static int myEqu(double *s, int na, double *b,int nb)
{
  StMatrixD mtx(na,na);
  double *m = &mtx(1,1);
  TCL::trupck(s,m,na);
  size_t ierr=0;
  mtx.invert(ierr);
  if (ierr) return ierr;
  for (int ib=0;ib<nb;ib++) {
    TCL::vmatl(m,b+ib*na,s,na,na);
    memcpy(b+ib*na,s,na*sizeof(*b));
  }
  TCL::trpck(m,s,na);
  return 0;  
}
//_____________________________________________________________________________
static void Eigen2(const double err[3], double lam[2], double eig[2][2])
{

  double spur = err[0]+err[2];
  double det  = err[0]*err[2]-err[1]*err[1];
  double dis  = spur*spur-4*det;
  if (dis<0) dis = 0;
  dis = sqrt(dis);
  lam[0] = 0.5*(spur+dis);
  lam[1] = 0.5*(spur-dis);
  eig[0][0] = 1; eig[0][1]=0;
  if (dis>1e-6*spur) {// eigenvalues are different
    if (fabs(err[0]-lam[0])>fabs(err[2]-lam[0])) {
     eig[0][1] = 1; eig[0][0]= -err[1]/(err[0]-lam[0]);
    } else {
     eig[0][0] = 1; eig[0][1]= -err[1]/(err[2]-lam[0]);
    }
    double tmp = sqrt(eig[0][0]*eig[0][0]+eig[0][1]*eig[0][1]);
    eig[0][0]/=tmp; eig[0][1]/=tmp;
  }
  eig[1][0]=-eig[0][1];  eig[1][1]= eig[0][0];
}
//_____________________________________________________________________________
static TComplex MyFactor(double rho,double drho,double s)
{
// Integral exp(i*Phi)*dL where Phi = rho*L + 0.5*drho*L**2
// Let it is equal  exp(i*Phi)*A(L) + const
// then dA/dL + i*(rho+drho*L)*A = 1
// Solve this equation for Taylor representation of A(L)
// static int Iter=0;
  TComplex arr[3],add;
  TComplex Sum; //
  Sum = 0.0;
  arr[0] = 1.; arr[1] = 0.;
  drho = drho/rho;
  double ss = s;
  for (int j=2;1;j++) {
    arr[2] = -TComplex(0,1)*rho*(arr[1]+drho*arr[0])/double(j);
    ss *=s; add = ss*arr[2]; Sum += add;
    if (1e-12*Sum.Rho2() > add.Rho2()) break;
//    printf(" Iter=%d %d %g\n",Iter++,j-1,TComplex::Abs(add));
    arr[0]=arr[1]; arr[1]=arr[2]; 
  }
  return Sum;
}
#endif //0

ClassImp(THelixTrack)
//_____________________________________________________________________________
THelixTrack::THelixTrack(const double *xyz,const double *dir,double rho
		        ,double drho)
{
//	Made from GEANT3 ghelix by V.Perevoztchikov
//
//    ******************************************************************
//    *                                                                *
//    *  Performs the tracking of one step in a magnetic field         *
//    *  The trajectory is assumed to be a helix in a constant field   *
//    *  taken at the mid point of the step.                           *
//    *  Parameters:                                                   *
//    *   input                                                        *
//    *     STEP =arc length of the step asked                         *
//    *     VECT =input vector (position,direction cos and momentum)   *
//    *     CHARGE=  electric charge of the particle                   *
//    *   output                                                       *
//    *     VOUT = same as VECT after completion of the step           *
//    *                                                                *
//    *    ==>Called by : <USER>, GUSWIM                               *
//    *       Author    M.Hansroul  *********                          *
//    *       Modified  S.Egli, S.V.Levonian                           *
//    *       Modified  V.Perevoztchikov
//    *                                                                *
//    ******************************************************************
//
  fEmx=0;
  Set(xyz,dir,rho,drho);
}
//_____________________________________________________________________________
THelixTrack &THelixTrack::operator=(const THelixTrack &from)
{
  THEmx_t *save = fEmx;
  memcpy(fBeg,from.fBeg,fEnd-fBeg);
  fEmx=save;
  if (from.fEmx) SetEmx(from.fEmx->Arr());
  return *this;
}
//_____________________________________________________________________________
THelixTrack::THelixTrack(const THelixTrack &from)
{
  fEmx=0;
  *this = from;
}
//_____________________________________________________________________________
THelixTrack::THelixTrack(const THelixTrack *fr)
{
  fEmx=0;
  Set(fr->fX,fr->fP,fr->fRho);
}
//_____________________________________________________________________________
THelixTrack::~THelixTrack()
{ delete fEmx;fEmx=0;}
//_____________________________________________________________________________
THelixTrack::THelixTrack()
{
  memset(fBeg,0,fEnd-fBeg);
}
//_____________________________________________________________________________
void THelixTrack::Set(const double *xyz,const double *dir,double rho
		     ,double drho)
{
  fX[0] = xyz[0]; fX[1] = xyz[1]; fX[2] = xyz[2];
  fP[0] = dir[0]; fP[1] = dir[1]; fP[2] = dir[2];
  fRho = rho; fDRho=drho;
  Build();
}
//_____________________________________________________________________________
void THelixTrack::SetEmx(const double*  err2xy,const double*  err2sz)
{
  if (!fEmx) fEmx = new THEmx_t;
  fEmx->Set(err2xy,err2sz);
}
//_____________________________________________________________________________
void THelixTrack::SetEmx(const double*  err)
{
  if (!fEmx) fEmx = new THEmx_t;
  fEmx->Set(err);
}
//_____________________________________________________________________________
void THelixTrack::StiEmx(double err[21]) const
{
enum {kXX
     ,kYX,kYY                       
     ,kZX,kZY,kZZ                 
     ,kEX,kEY,kEZ,kEE           
     ,kPX,kPY,kPZ,kPE,kPP     
     ,kTX,kTY,kTZ,kTE,kTP,kTT
     ,kLN
     };
   memset(err,0,sizeof(err[0])*kLN);
   double cosCA = fP[0]/fCosL;
   err[kYY] = fEmx->mHH/(cosCA*cosCA);
   err[kZY] = fEmx->mHZ/(cosCA);
   err[kZZ] = fEmx->mZZ;
   err[kEY] = fEmx->mHA/cosCA;
   err[kEZ] = fEmx->mAZ;
   err[kEE] = fEmx->mAA;
   err[kPY] = fEmx->mHC/cosCA;
   err[kPZ] = fEmx->mCZ;
   err[kPE] = fEmx->mAC;
   err[kPP] = fEmx->mCC;
   err[kTY] = fEmx->mHL/(cosCA*fCosL*fCosL);
   err[kTZ] = fEmx->mZL/(      fCosL*fCosL);
   err[kTE] = fEmx->mAL/(      fCosL*fCosL);
   err[kTP] = fEmx->mCL/(      fCosL*fCosL);
   err[kTT] = fEmx->mLL/(      fCosL*fCosL*fCosL*fCosL);
}
//_____________________________________________________________________________
void THelixTrack::Set(double rho,double drho)
{
   fRho = rho; fDRho=drho; 
}
//_____________________________________________________________________________
void THelixTrack::Backward()
{

  double d[3];
  for (int i=0;i<3;i++) { d[i]=-fP[i];}
  Set(fX,d,-fRho,-fDRho); 
  if(fEmx) fEmx->Backward();
}
//_____________________________________________________________________________
void THelixTrack::GetSpot(const double axis[3][3],double emx[3]) const
{
/// THelixTrack::GetSpot(double axis[3][3],emx[3]) const
/// axis[0,1]  - vectors in plane. 
/// axis[2]    - normal vector of plane
/// emx[3] error matrix of coordinates according vectors in plane.

//   transformation matrix from "helix" coordinate to global
   double my[3][3] = {{-fP[1]/fCosL, 0,fP[0]}
                     ,{ fP[0]/fCosL, 0,fP[1]}
                     ,{           0, 1,fP[2]}};

   double T[3][3],tmp[3][3],g[6],t[2][2];
   TCL::mxmpy (axis[0],my[0],T[0],3,3,3);
//   	now account that matrix axis may be non orthogonal
   TCL::traat(axis[0],g,3,3);
   if (fabs(g[0]-1)+fabs(g[1])+fabs(g[2]-1)
      +fabs(g[3])+fabs(g[4])+fabs(g[5]-1)>1e-10) {//non orthogonal case
     TCL::trsinv(g,g,3);
     memcpy(tmp[0],T[0],sizeof(T));
     TCL::trsa  (g,tmp[0],T[0],3,3);
   }
   TCL::vlinco(T[0],1.,T[2],-T[0][2]/T[2][2],t[0],2);
   TCL::vlinco(T[1],1.,T[2],-T[1][2]/T[2][2],t[1],2);
   double myerr[3]={fEmx->mHH,fEmx->mHZ,fEmx->mZZ};
   TCL::trasat(t[0],myerr,emx,2,2);
   return;
}
//_____________________________________________________________________________
void THelixTrack::Build()
{

  double tmp;
    
  tmp = fP[0]*fP[0]+ fP[1]*fP[1]+ fP[2]*fP[2];
  if (fabs(tmp-1.) > 1.e-12) {
    tmp = ::sqrt(tmp); fP[0] /=tmp; fP[1] /=tmp; fP[2] /=tmp; }
    
  fCosL = ::sqrt(fP[0]*fP[0]+fP[1]*fP[1]);
}
//______________________________________________________________________________
void THelixTrack::MakeMtx(double step,double F[5][5])
{
//  H,A,C,Z,L
  enum {kH=0,kA,kC,kZ,kL};

  double S = step*fCosL;
  memset(F[0],0,sizeof(F[0][0])*5*5);

  F[kH][kH]   = sgCf1.Re()+1.;
  double dSdH = sgCf1.Im();

  F[kH][kA]   = S*sgCOne.Re();
  double dSdA = S*sgCOne.Im();

  TComplex llCOneD = S*S*expOneD(-sgImTet);
  F[kH][kC]   = llCOneD.Re();
  double dSdC = llCOneD.Im();

  F[kA][kH] =  -dSdH*fRho;
  F[kA][kA] = 1-dSdA*fRho;
  F[kA][kC] = S+dSdC*fRho;
  F[kC][kC] = 1;

  double tanL = fP[2]/fCosL;

  F[kZ][kH] = -dSdH*tanL;
  F[kZ][kA] = -dSdA*tanL;
  F[kZ][kC] =  dSdC*tanL;
  F[kZ][kZ] = 1;
  F[kZ][kL] = S/(fCosL*fCosL);
  F[kL][kL] = 1;
}
//______________________________________________________________________________
void THelixTrack::TestMtx() 
{
  enum {kH=0,kA,kC,kZ,kL};
const static char* T="HACZL";
  double Dir[4][3],X[4][3]={{0}},Rho[2],step,F[5][5],Del,Dif;
  double maxEps = 0;  
  step = 20;
  int nErr=0;
  int iR = 10+ gRandom->Rndm()*100;
  int iAlf=30+ gRandom->Rndm()*100;
  int iLam=10+ gRandom->Rndm()*60;
//??iLam=0;
    Rho[0] = 1./iR;
    double alf = iAlf/180.*M_PI;
    double lam = iLam/180.*M_PI;
    Dir[0][0] = cos(lam)*cos(iAlf/180.*M_PI);
    Dir[0][1] = cos(lam)*sin(iAlf/180.*M_PI);
    Dir[0][2] = sin(lam);
    THelixTrack tc(X[0],Dir[0],Rho[0]);
    tc.Eval(step,X[1],Dir[1]);
    tc.MakeMtx(step,F);
    printf("TestMtx: Angle=%d Lam=%d \tRad=%d \n",iAlf,iLam,iR);

    for (int iHAR=0;iHAR<5;iHAR++) {
      memcpy(X[2]  ,X[0]  ,sizeof(X[0][0])  *3);
      memcpy(Dir[2],Dir[0],sizeof(Dir[0][0])*3);
      Del = 0;
      Rho[1]=Rho[0];
      switch (iHAR) {
	case kH: { 
	  Del = 0.001*iR;
          X[2][0] += -Dir[0][1]*Del/cos(lam);
          X[2][1] +=  Dir[0][0]*Del/cos(lam);
          break;}
	  
	case kA: {
	  Del = M_PI/180*0.01;
          Dir[2][0] = cos(lam)*cos(alf+Del);
          Dir[2][1] = cos(lam)*sin(alf+Del);
          Dir[2][2] = sin(lam);
          break;}

	case kC: {
          Del = Rho[0]*0.005;
          Rho[1] = Rho[0]+Del;
          break;}
	case kZ: {
          Del = 0.02;
          X[2][2] += Del;
          break;}
	case kL: {
          Del = M_PI/180*0.1;
          Dir[2][0] = cos(lam+Del)*cos(alf);
          Dir[2][1] = cos(lam+Del)*sin(alf);
          Dir[2][2] = sin(lam+Del);
          break;}
        }//end switch
      
        THelixTrack tcc(X[2],Dir[2],Rho[1]);
        double myStep = tcc.Path(X[1][0],X[1][1]);
        tcc.Eval(myStep,X[3],Dir[3]);

        for (int jHAR=0;jHAR<5; jHAR++) {
          if (jHAR==kC) continue;
          if (jHAR==kL) continue;
	  switch(jHAR) {
	  case kH: {
	    Dif = (X[3][0]-X[1][0])*(-Dir[1][1])
	        + (X[3][1]-X[1][1])*( Dir[1][0]);
            Dif/=cos(lam);
            break;}
	  case kA: {
	    Dif = atan2(Dir[3][1],Dir[3][0])
	         -atan2(Dir[1][1],Dir[1][0]); 
            if (Dif>  M_PI) Dif-=2*M_PI;
            if (Dif< -M_PI) Dif+=2*M_PI;
            break;}
	  case kZ: {
	    Dif = X[3][2]-X[1][2];
            break;}
          }
          double est = Dif/Del;
	  double eps = fabs(est-F[jHAR][iHAR])*2
	             /(fabs(est)+fabs(F[jHAR][iHAR]+1e-6));
          if (eps>maxEps) maxEps=eps;
          if (eps < 1e-2) continue;
          nErr++;
          printf(" m%c%c \t%g \t%g \t%g\n",
	         T[jHAR],T[iHAR],F[jHAR][iHAR],est,eps);
	
    } }  
    printf("TestMtx: %d errors maxEps=%g\n",nErr,maxEps);

}

//_____________________________________________________________________________
double THelixTrack::Move(double step) 
{
  double xyz[3],dir[3],rho,F[5][5];
  Eval(step,xyz,dir,rho);
  Set(xyz,dir,rho,fDRho);
  if (fEmx && fEmx->mHH>0) {
    MakeMtx(step,F);
    fEmx->Move(F);
  } 
  return step;
}
//_____________________________________________________________________________
double THelixTrack::Move(double step,double F[5][5]) 
{
  double xyz[3],dir[3],rho;
  Eval(step,xyz,dir,rho);
  Set(xyz,dir,rho,fDRho);
  MakeMtx(step,F);
  if (fEmx && fEmx->mHH>0) fEmx->Move(F); 
  return step;
}

//_____________________________________________________________________________
double THelixTrack::Step(double stmax,const  double *surf, int nsurf,
                         double *xyz, double *dir, int nearest) const
{
  int i;
  double s[10]={0,0,0,0,0,0,0,0,0,0},tmp=0;
  memcpy(s,surf,nsurf*sizeof(surf[0]));
  
  for(i=1;i<nsurf;i++) if (fabs(s[i])>tmp) tmp = fabs(s[i]);
  if(fabs(tmp-1.)>0.1) {for(i=0;i<nsurf;i++) s[i]/=tmp;}
  double stmin = (nearest)? -stmax:0;
//  if (!s[3] && !s[6] && !s[8] && !s[9] && fabs(s[4]-s[5])<1.e-12) 
//         return StepHZ(s,nsurf,xyz,dir,nearest);
//   else  return Step(stmin,stmax,s,nsurf,xyz,dir,nearest);
  return Step(stmin,stmax,s,nsurf,xyz,dir,nearest);
}


//_____________________________________________________________________________
double THelixTrack::Step(double stmin,double stmax, const double *s, int nsurf,
                         double *xyz, double *dir, int nearest) const
{
  int ix,jx,nx,ip,jp;
  double poly[4][3],tri[3],sol[2],cos1t,f1,f2,step,ss;
  const double *sp[4][4] = {{s+0,s+1,s+2,s+3}, {s+1,s+4,s+7,s+9}, 
                            {s+2,s+7,s+5,s+8}, {s+3,s+9,s+8,s+6}}; 

  double myMax = 1./(fabs(fRho*fCosL)+1.e-10);
  THelixTrack th(fX,fP,fRho);
  cos1t = 0.5*fRho*fCosL;
  double totStep=0;
  while (2005) {
    double hXp[3]={-th.fP[1],th.fP[0],0};
    poly[0][0]=1.;poly[0][1]=0.;poly[0][2]=0.;
    tri[0]=tri[1]=tri[2]=0;
    for(ix=1;ix<4;ix++) {
      poly[ix][0] =th.fX  [ix-1]; 
      poly[ix][1] =th.fP  [ix-1]; 
      poly[ix][2] =hXp[ix-1]*cos1t;
    }

    nx = (nsurf<=4) ? 1:4;
    for(ix=0;ix<nx;ix++) {
      for(jx=ix;jx<4;jx++) {  
	ss = *sp[ix][jx]; if(!ss) 	continue;
	for (ip=0;ip<3;ip++) {
          f1 = poly[ix][ip]; if(!f1) 	continue;
          f1 *=ss;
          for (jp=0;jp+ip<3;jp++) {
            f2 = poly[jx][jp]; if(!f2) 	continue;
            tri[ip+jp] += f1*f2;
    } } } }

    int nsol = SqEqu(tri,sol);
    step = 1.e+12;
    if (nsol<0) 	return step;

    if (nearest && nsol>1) {
      if(fabs(sol[0])>fabs(sol[1])) sol[0]=sol[1];
      nsol = 1;
    }
    if (nsol) step = sol[0];
    if (step < stmin && nsol > 1) step = sol[1];
    if (step < stmin || step > stmax) 	{
      nsol = 0; 
      if (step>0) {step = stmax; stmin+=myMax/2;}
      else        {step = stmin; stmax-=myMax/2;}}

    if (!nsol && fabs(step) < 0.1*myMax) return 1.e+12;
    if (fabs(step)>myMax) {step = (step<0)? -myMax:myMax; nsol=0;}

    double x[3],d[3];
    th.Step(step,x,d);
    if (nsol) {//test it
      ss = s[0]+s[1]*x[0]+s[2]*x[1]+s[3]*x[2];
      if (nsurf > 4) ss += s[4]*x[0]*x[0]+s[5]*x[1]*x[1]+s[6]*x[2]*x[2];
      if (nsurf > 7) ss += s[7]*x[0]*x[1]+s[8]*x[1]*x[2]+s[9]*x[2]*x[0];
      if (fabs(ss)<1.e-7) {
	if (xyz) memcpy(xyz,x,sizeof(*xyz)*3);
	if (dir) memcpy(dir,d,sizeof(*dir)*3);
	return totStep+step;
    } }

    stmax -=step; stmin -=step;
    if (stmin>=stmax) return 1.e+12;
    totStep+=step;
    th.Move(step);
  }

}

//_____________________________________________________________________________
double THelixTrack::StepHZ(const double *su, int nsurf, 
                           double *xyz, double *dir,int nearest) const
{
   double tri[3] = {0,0,0};
   double f0,fc,fs,R,tet,tet0,tet1,tet2,costet,su45=0,fcs;
   

   R = 1./fRho/fCosL;
//		X
   f0 = fX[0] - fP[1]*R;
   fc = fP[1]*R;
   fs = fP[0]*R;

   tri[0] = su[0] + su[1]*f0;
   tri[1] = su[1]*fc;
   tri[2] = su[1]*fs;
   if (nsurf >4) {
     su45 = 0.5*(su[4]+su[5]);
     fcs  = fc*fc + fs*fs;
     tri[0] += su45*f0*f0 + su45*fcs; 
     tri[1] += su45*2*f0*fc;
     tri[2] += su45*2*f0*fs;
   }
//		Y
   f0 =  fX[1] + fP[0]*R;
   fc = -fP[0]*R;
   fs =  fP[1]*R;

   tri[0] += su[2]*f0;
   tri[1] += su[2]*fc;
   tri[2] += su[2]*fs;

   if (nsurf >4) {
     tri[1] += su45*2*f0*fc;
     tri[2] += su45*2*f0*fs;
   }
   costet = -tri[0]/::sqrt(tri[1]*tri[1]+tri[2]*tri[2]);
   if(fabs(costet)>1.) return 1.e+12;
   tet0 = atan2(tri[2],tri[1]);
   tet  = acos(costet);
   tet1 =  tet + tet0;
   tet2 = -tet + tet0;

   if (tet1 > 2*M_PI) tet1 -= 2*M_PI;
   if (tet2 > 2*M_PI) tet2 -= 2*M_PI;
   if (nearest) { 	//Select the neares solution
     if (fabs(tet1)>fabs(tet1-2*M_PI)) tet1 -=2*M_PI;
     if (fabs(tet1)>fabs(tet1+2*M_PI)) tet1 +=2*M_PI;
     if (fabs(tet2)>fabs(tet2-2*M_PI)) tet2 -=2*M_PI;
     if (fabs(tet2)>fabs(tet2+2*M_PI)) tet2 +=2*M_PI;
     if (fabs(tet1)>fabs(tet2)       ) tet1  =tet2;
     return Step(tet1*R,xyz,dir);
   } else {		//forward seqrch 
     double s1 = tet1*R;
     if (s1<=0) s1 += 2*M_PI*fabs(R);
     double s2 = tet2*R;
     if (s2<=0) s2 += 2*M_PI*fabs(R);
     if (s1>s2) s1=s2;
     return Step(s1,xyz,dir);
   }

}
//_____________________________________________________________________________
double THelixTrack::Path(const THelixTrack &th,double *s2) const
{
   double SxyMe,SxyHe,SMe,SHe,dSMe=0,dSHe=0;
   TCircle tcMe,tcHe;
   Fill(tcMe);th.Fill(tcHe);
   SxyMe = tcMe.Path(tcHe,&SxyHe);
   if (SxyMe>1e33) return 3e33;
   THelixTrack thMe(fX,fP,fRho),thHe(th.fX,th.fP,th.fRho);

   SMe = SxyMe/thMe.GetCos(); thMe.Move(SMe);
   SHe = SxyHe/thHe.GetCos(); thHe.Move(SHe);
   for (int ix=0;ix<3;ix++) {
     if (fabs(thMe.Pos()[ix]-thHe.Pos()[ix])>100)return 3e33;}
   for (int iter=0;iter<10; iter++) {
     dSMe = thMe.Path(thHe.Pos());
     if (dSMe>1e33) return 3e33;
     SMe+=dSMe; thMe.Move(dSMe);
     dSHe = thHe.Path(thHe.Pos());
     if (dSHe>1e33) return 3e33;
     SHe+=dSHe; thHe.Move(dSHe);
     if(fabs(dSHe)+fabs(dSMe)<1e-5) break;
   }
   if(fabs(dSHe)+fabs(dSMe)> 1e-5)  return 3e33;
   if(s2) *s2=SHe;
   return SMe;
}
//_____________________________________________________________________________
double THelixTrack::PathX(const THelixTrack &th,double *s2, double *dst, double *xyz) const
{
  double ss1,ss2,dd,ss1Best,ss2Best,ddBest=1e33;
  double xx[9];
  int jkBest=-1;
  for (int jk=0;jk<4;jk++) {
    THelixTrack th1(this),th2(&th);
    if (jk&1) th1.Backward();
    if (jk&2) th2.Backward();
    ss1 = th1.Path(th2,&ss2);
    if (ss1>=1e33) continue;
    if (ss2>=1e33) continue;
    th1.Eval(ss1,xx+0);
    th2.Eval(ss2,xx+3);
    TCL::vsub(xx,xx+3,xx+6,3);
    dd = TCL::vdot(xx+6,xx+6,3);
    if (dd > ddBest) continue;
    ddBest = dd; jkBest=jk; ss1Best = ss1; ss2Best = ss2;
    if (xyz) TCL::ucopy(xx,xyz,6);
  }
  if (jkBest<0) { if(s2) *s2=3e33; return 3e33; }
  if (jkBest&1) ss1Best = -ss1Best;
  if (jkBest&2) ss2Best = -ss2Best;
  if (s2 ) *s2  = ss2Best;
  if (dst) *dst = ddBest;
  return ss1Best;
}
//_____________________________________________________________________________
double THelixTrack::Path(double x,double y) const
{
   double ar[6]={fX[0],fX[1],0,fP[0]/fCosL,fP[1]/fCosL,0};
   THelixTrack ht(ar,ar+3,fRho);
   ar[0]=x;ar[1]=y;
   double s= ht.Path(ar)/fCosL;
   return s;
}
//_____________________________________________________________________________
double THelixTrack::Step(const double *point,double *xyz, double *dir) const
{

    static int nCount=0; nCount++;
    TComplex cpnt(point[0]-fX[0],point[1]-fX[1]);
    TComplex cdir(fP[0],fP[1]); cdir /=TComplex::Abs(cdir);
    double step[3]={0,0,0};
//		Z estimated step 

    int zStep=0;
    if (fabs(fP[2]) > 0.01){ //Z approximation
      zStep = 1;
      step[1] = (point[2]-fX[2])/fP[2];
    }
//angle approximation
//		R estimated step
    {
      cpnt /= cdir;
      if (fabs(cpnt.Re()*fRho) < 0.01) {
        step[2]=cpnt.Re();
      } else {
        double rho = fRho;
        for (int i=0;i<2;i++) {
          TComplex ctst = (1.+TComplex(0,1)*rho*cpnt);
	  ctst /=TComplex::Abs(ctst);
	  ctst = TComplex::Log(ctst);
	  step[2]= ctst.Im()/rho;
          if (!fDRho) break;
	  rho = fRho+ 0.5*fDRho*step[2];
        }
      }
      step[2]/=fCosL;
    }

    if (zStep) {
      double p = GetPeriod();
      int nperd = (int)((step[1]-step[2])/p);
      if (step[2]+nperd*p>step[1]) nperd--;
      if (fabs(step[2]-step[1]+(nperd+0)*p)
         >fabs(step[2]-step[1]+(nperd+1)*p)) nperd++;
      step[2]+=(nperd)*p;
    }
    step[0] = step[2];

    double ds = step[1]-step[2];
    if (zStep && fabs(ds)>1.e-5) {
      double dz = ds*fP[2];
      step[0] += dz*dz/ds;
    }


    double xnear[6],ss=0;  double* pnear=xnear+3;
//		iterations
    double dstep = 1.e+10,oldStep=dstep,dztep;
    double lMax = step[0]+0.25*GetPeriod();
    double lMin = step[0]-0.25*GetPeriod();

    if (zStep) {
      lMax = (step[1]>step[2])? step[1]:step[2];
      lMin = (step[1]>step[2])? step[2]:step[1];}
    int iter=99,icut=1;
    THelixTrack local(this);
    local.Move(step[0]);
    lMax-=step[0];lMin-=step[0];
    local.Step(0.,xnear,pnear);
    for (; iter; iter--)
    { 
      double diff = (icut)? lMax-lMin: fabs(dstep);
      if (diff < 0.1) {
        if (diff < 1.e-6) 	break;
        double tmp = 0;
        for (int i=0;i<3;i++) {tmp += fabs(point[i]-xnear[i]);}
        if (diff < tmp*1.e-4) 	break;
        if (tmp < 1.e-6) 	break;
      }
      
      local.Step(ss,xnear,pnear);
      dstep = 0; icut = 0;
      for (int i=0;i<3;i++) {dstep += pnear[i]*(point[i]-xnear[i]);}
      if(dstep<0) {
        lMax = ss; dztep = -0.5*(lMax-lMin);
	if (dstep<dztep || fabs(dstep)>0.7*oldStep) {icut=1;dstep = dztep;}
      } else {
        lMin = ss; dztep =  0.5*(lMax-lMin);
	if (dstep>dztep || fabs(dstep)>0.7*oldStep) {icut=1;dstep = dztep;}
      }
      ss += dstep; 
      oldStep=fabs(dstep);
    }
//    printf("ITERS=%d dSTEP=%g \n",iter,dstep);
    if (!iter){ printf("*** Problem in THElixTrack::Step(vtx) ***\n");
                printf("double vtx[3]={%g,%g,%g};",point[0],point[1],point[2]);
                Print();}
    assert(iter);
    step[0]+=ss;
    return (xyz) ? Step(step[0],xyz,dir) : step[0];
}
//_____________________________________________________________________________
double THelixTrack::Dca(const double *point,double *dcaErr) const
{
   double x[3],T[3][3],emx[3];
   double s = Path(point,x,T[2]);
   for (int i=0;i<3;i++) {T[0][i]=point[i]-x[i];}
   double dca = sqrt(T[0][0]*T[0][0]+T[0][1]*T[0][1]+T[0][2]*T[0][2]);
   if (!dcaErr) return dca;

   for (int i=0;i<3;i++) {T[0][i]/=dca;}
   T[1][0]=T[0][1]*T[2][2]-T[2][1]*T[0][2];
   T[1][1]=T[0][2]*T[2][0]-T[2][2]*T[0][0];
   T[1][2]=T[0][0]*T[2][1]-T[2][0]*T[0][1];
   
   THelixTrack th(this);
   th.Move(s);
   th.GetSpot(T,emx);
   *dcaErr=emx[0];
   return dca;
}
//_____________________________________________________________________________
double THelixTrack::Dca(double x,double y,double *dcaErr) const
{
  double dir[3]={fP[0],fP[1],0};
  THelixTrack hlx(fX,dir,fRho);
  if (fEmx) hlx.SetEmx(fEmx->Arr());
  double vtx[3]={x,y,fX[2]};
  return hlx.Dca(vtx,dcaErr);
}


//_____________________________________________________________________________
double THelixTrack::Dca(const double point[3]
                       ,double &dcaXY,double &dcaZ,double dcaEmx[3],int kind) const
/// Full 3d dca evaluation
/// point[3] - x,y,z of vertex
/// dcaXY - dca in xy plane
/// dcaZ  - dca in Z direction
/// dcaEmx[3] - err(dcaXY*dcaXY),err(dcaXY*dcaZ),err(dcaZ*dcaZ)
/// kind - 3=3d dca,2=2d dca
/// return distance to dca point
{
   double dif[3];
   double s = 0;
   assert(kind==2 || kind==3);
   if (kind==3) s = Path(point);
   else         s = Path(point[0],point[1]);

   THelixTrack th(this);
   th.Move(s);
   const double *x=th.Pos();
   const double *d=th.Dir();

   for (int i=0;i<3;i++) {dif[i]=x[i]-point[i];}
   double nor = th.GetCos();
   double T[3][3]={{-d[1]/nor, d[0]/nor,    0}
                  ,{        0,        0,    1}
		  ,{ d[0]/nor, d[1]/nor,    0}};

   dcaXY = T[0][0]*dif[0]+T[0][1]*dif[1];
   dcaZ  = dif[2];
   THEmx_t *emx =th.Emx();
   dcaEmx[0] = emx->mHH;
   dcaEmx[1] = 0;
//	cos(Lambda) **4 to account that we are in the nearest point
   dcaEmx[2] = emx->mZZ*pow(th.GetCos(),4);
   return s;
}


//_____________________________________________________________________________
double THelixTrack::GetPeriod() const
{
   double per = (fabs(fRho) > 1.e-10) ? fabs(2.*M_PI/fRho):1.e+10;
   return per/fCosL;
}
//______________________________________________________________________________
void THelixTrack::Rot(double angle)
{
  Rot(cos(angle),sin(angle));
}
//______________________________________________________________________________
void THelixTrack::Rot(double cosa,double sina)
{
  TComplex CX(fX[0],fX[1]),CP(fP[0],fP[1]);
  TComplex A (cosa,sina);
  CX *=A; fX[0] = CX.Re(); fX[1]=CX.Im();
  CP *=A;
  fP[0] = CP.Re(); fP[1]=CP.Im();
}
//_____________________________________________________________________________
void THelixTrack::Streamer(TBuffer &){}
//_____________________________________________________________________________
void THelixTrack::Print(Option_t *) const
{
  printf("\n THelixTrack::this = %p\n",(void*)this);
  printf(" THelixTrack::fX[3] = { %f , %f ,%f }\n",fX[0],fX[1],fX[2]);
  printf(" THelixTrack::fP[3] = { %f , %f ,%f }\n",fP[0],fP[1],fP[2]);
  printf(" THelixTrack::fRho  =   %f \n\n",fRho);

  printf("double xyz[3] = {%g,%g,%g};\n" ,fX[0],fX[1],fX[2]); 
  printf("double dir[3] = {%g,%g,%g};\n" ,fP[0],fP[1],fP[2]); 
  printf("double Rho = %g;\n" ,fRho); 
  printf("THelixTrack *ht = new THelixTrack(xyz,dir,Rho);\n");
  
}
//_____________________________________________________________________________
int SqEqu(double *cba, double *sol)
{
//	
//	made from fortran routine GVPSQR (Geant320)
/*
************************************************************************
*                                                                      *
*     SUBROUTINE GVPSQR (CBA,SOL,NSOL)             870924  VP          *
*                                                                      *
*       SOLVE  QUADRATIC  EQUATION                                     *
*                                                                      *
*   ARGUMENTS:                                                         *
*       CBA     Array of coeff's A0 + A1*x + A2*x**2                   *
*       SOL     Solutions                                              *
*       NSOL    Number of solutions :                                  *
*               if zero - SOL[0]= extremum                             *
*               if -ve  - No solution at all                           *
*                                                                      *
************************************************************************
*/
  const double zero2=1.e-12;
  double swap,a,b,c,amx,dis,bdis;
  int nsol;
/*--------------------------------------------------------------------*/

  a = cba[2]; b = cba[1]*0.5; c = cba[0];
  if (b < 0.) {a = -a; b = -b; c = -c;}
  amx = fabs(a); if (amx<b) amx = b; if (amx<fabs(c)) amx = fabs(c);
  if (amx <= 0.) return -1;
  a = a/amx; b = b/amx; c = c/amx;

  dis = b*b - a*c;
  nsol = 1;
  if (fabs(dis) <= zero2)  dis = 0;
  if (dis < 0.) { nsol = 0; dis  = 0.;}

  dis = ::sqrt(dis); bdis = b + dis;
  if (fabs(c) > 1.e+10*bdis)	return -1;
  sol[0] = 0.;
  if (fabs(bdis) <= 0.)      	return nsol;
  sol[0] = (-c/bdis);		
  if (dis <= 0.)            	return nsol;
  if (bdis >= 1.e+10*fabs(a))   return nsol;    
  nsol   = 2; sol[1] = (-bdis/a);
  if (sol[0] > sol[1]) { swap = sol[0]; sol[0] = sol[1]; sol[1] = swap;}
  return nsol;
}
//_____________________________________________________________________________
double THelixTrack::Eval(double step, double *xyz, double *dir,double &rho) const
{
   Eval(step,xyz,dir);
   rho = fRho +(step*fCosL)*fDRho;
   return step;
}
//_____________________________________________________________________________
double THelixTrack::Eval(double step, double *xyz, double *dir) const
{
  if (!step) {
    if (xyz) memcpy(xyz,fX,sizeof(fX));
    if (dir) memcpy(dir,fP,sizeof(fP));
    return 0.;
  }

  double ztep = step*fCosL;
  double teta = ztep*(fRho+0.5*ztep*fDRho);

  sgCX1   = TComplex(fX[0]  ,fX[1]);
  sgCD1   = TComplex(fP[0],fP[1])/fCosL;
  sgImTet = TComplex(0,teta);
  sgCOne  = expOne(sgImTet);			//(exp(I*Rho*L)-1)/(I*Rho*L)
  sgCf1   = sgImTet*sgCOne;
  sgCD2   = sgCD1*sgCf1+sgCD1; 			// exp(I*Fi0+I*Rho*L)
  sgCX2   = sgCD1*sgCOne*ztep;			// exp(I*Fi0)*(exp(I*Rho*L)-1)/(I*Rho)

  if (xyz) {
    xyz[0] = sgCX2.Re()+sgCX1.Re();
    xyz[1] = sgCX2.Im()+sgCX1.Im();
    xyz[2] = fX[2]+fP[2]*step;
  }
  if (dir) {    
    sgCD2/= TComplex::Abs(sgCD2);
    dir[0] = sgCD2.Re()*fCosL;
    dir[1] = sgCD2.Im()*fCosL;
    dir[2] = fP[2];
  }
  return step;
}
//_____________________________________________________________________________
void THelixTrack::Fill(TCircle &circ) const
{
  circ.fX[0]=fX[0];
  circ.fX[1]=fX[1];
  circ.fD[0]=fP[0]/fCosL;
  circ.fD[1]=fP[1]/fCosL;
  circ.fRho=fRho;
  if (fEmx) circ.SetEmx(fEmx->Arr());
}
//_____________________________________________________________________________
void THelixTrack::Test1()
{
double surf[4] = {-11.32212856152224, 0.50109792630239824, -0.86539108263698283, 0.00078459561521909921};
double xyz[3] = {-0.0206564,-0.0153429,0.0285582};
double dir[3] = {0.450295,-0.596426,-0.664463};
double Rho = 0.00678696;
THelixTrack TH(xyz,dir,Rho);

double s = TH.Step(100000,surf,4);
printf("s=%g = 15.3589\n",s);
}
//_____________________________________________________________________________
void THelixTrack::Test2()
{
double diff[3];

double xyz[3] = {-60.0301,1.51445,-1.57283};
double dir[3] = {-0.849461,0.526419,0.0360391};
double Rho = 0.00363571;
THelixTrack ht(xyz,dir,Rho);

double MyHit[3]= { -177.673, 41.305, 2.90798};
double MyClo[3];

printf("%s= %g %g %g\n","MyHit",MyHit[0],MyHit[1],MyHit[2]);
double s = ht.Step(MyHit,MyClo);
ht.Step(s,MyClo);
TCL::vsub(MyClo,MyHit,diff,3);
double MyDist = sqrt(TCL::vdot(diff,diff,3));
printf("%s= %g %g %g\n","MyClo ",MyClo[0],MyClo[1],MyClo[2]);
printf("MustBe= -177.661 41.4145 2.94559\n");

printf("%s= %g %g %g\n","MyDif ",diff[0],diff[1],diff[2]);
printf("MustBe= 0.0122709 0.109539 0.0376077\n");
printf("%s=%g\n","MyS   ",s);
printf("MustBe=125.375\n");
printf("%s= %g\n","MyDist",MyDist);
printf("MustBe= 0.116463\n");
}
//_____________________________________________________________________________
void THelixTrack::Test3()
{
double xyz[3] = {100,200,300};
double dir[3] = {-0.224845,-0.491295,-0.841471};
double Rho = 0.02;
double sur[8]={-120,1,0,0,0,0,0};
THelixTrack *ht = new THelixTrack(xyz,dir,Rho);
double newX[3],newD[3];
ht->Backward();
double s = ht->Step(1000.,sur,4,newX,newD);
printf("Result: s=%g newX=(%g %g %g) newD=(%g %g %g)\n"
      ,s,newX[0],newX[1],newX[2],newD[0],newD[1],newD[2]);
      
printf("MustBe: s=56.1931 newX=(120 222.222 347.285) newD=(0.464979 0.275174 0.841471)\n\n");

sur[6]=1e-6;
       s = ht->Step(1000.,sur,7,newX,newD);
printf("Result: s=%g newX=(%g %g %g) newD=(%g %g %g)\n"
      ,s,newX[0],newX[1],newX[2],newD[0],newD[1],newD[2]);
printf("MustBe: s=55.9338 newX=(119.88 222.151 347.067) newD=(0.464206 0.276476 0.841471)\n\n");
}
//_____________________________________________________________________________
void THelixTrack::Test4()
{
double surf[7] = {-100, 0, 0, 0, 1,1,0};
double xyz[3] = {-0.0206564,-0.0153429,0.0285582};
double dir[3] = {0.450295,-0.596426,-0.664463};
double Rho = 0.00678696;
double xnew[3];
THelixTrack TH(xyz,dir,Rho);

double s = TH.Step(100000,surf,7,xnew);
double dif = xnew[0]*xnew[0]+xnew[1]*xnew[1]-100;
printf("s=%g dif=%g\n",s,dif);

}
//_____________________________________________________________________________
void THelixTrack::Test5()
{
  double pars[4][2][7] = {
   {{0,0,0, 1,1,1, -0.001},{0,0, 0, -1,1,-1,0.002}},
   {{0,0,1, 1,1,1, -0.001},{0,0,-1, -1,1,-1,0.002}},
   {{0,0,1, 1,1,1, -0.001},{0,0,-1,  1,1,-1,0.002}},
  };
  for (int ip=0;ip<3;ip++) {
    THelixTrack th1(pars[ip][0],pars[ip][0]+3,pars[ip][0][6]); 
    THelixTrack th2(pars[ip][1],pars[ip][1]+3,pars[ip][1][8]); 
    th1.Move(-50);
    th2.Move(-50);
    double s2;
    double s1 = th1.Path(th2,&s2);
    th1.Move(s1);
    th2.Move(s2);
    double diff[3];
    TCL::vsub(th1.Pos(),th2.Pos(),diff,3);
    double dist = sqrt(TCL::vdot(diff,diff,3));
    printf("s1=%g s2=%g dist=%g\n",s1,s2,dist);

  }  
}
// //_____________________________________________________________________________
// void THelixTrack::TestErr()
// {
//    double hpar[7];
//    for (int i=0;i<7;i++) { hpar[i] = (gRandom->Rndm()-0.5)*100;
//    hpar[6] = 1./hpar[6];
//    
// }   

//______________________________________________________________________________
void THelixTrack::Show(double len, const THelixTrack *other) const
{
static TCanvas *myCanvas = 0;
  int kolor[2]={kRed,kBlue};

  TGraph  *ciGraph[2][2] = {{0}};
  TGraph  *ptGraph[2][2] = {{0}};
  TGraph  *szGraph[2]    = {0};
  
  double  x[100],y[100],z[100],l[100],xyz[3];
  double  X[4],Y[4],Z[4],L[4];
  const THelixTrack *th[]={this,other};
  int nH = (other)? 2:1;
  for (int ih=0;ih<nH;ih++) {
    double rho = fabs(th[ih]->GetRho());
    double step = 0.01*(1./(rho+1e-10));
   
    if (step>fabs(len)*0.10) step=fabs(len)*0.1;
    if (step<fabs(len)*0.01) step=fabs(len)*0.01;


    int nPts = (int)(fabs(len)/step);
    step = fabs(len)/nPts;
    if (len<0) {len = -len; step = -step;}
    for (int ipt=0; ipt<nPts; ipt++) {
      double s = ipt*step;
      th[ih]->Eval(s,xyz); 
      l[ipt]=s; x[ipt]=xyz[0]; y[ipt]=xyz[1], z[ipt]=xyz[2];
    }
    ciGraph[ih][0]  = new TGraph(nPts  , x, y);
    ciGraph[ih][1]  = new TGraph(nPts  , l, z);
    ciGraph[ih][0]->SetLineColor(kolor[ih]);
    ciGraph[ih][1]->SetLineColor(kolor[ih]);
    ptGraph[ih][0]  = new TGraph(   1  , x, y);
    ptGraph[ih][1]  = new TGraph(   1  , l, z);
    ptGraph[ih][0]->SetMarkerColor(kolor[ih]);
    ptGraph[ih][1]->SetMarkerColor(kolor[ih]);

    X[ih*2+0]=x[0]; X[ih*2+1]=x[nPts-1];
    Y[ih*2+0]=y[0]; Y[ih*2+1]=y[nPts-1];
    Z[ih*2+0]=z[0]; Z[ih*2+1]=z[nPts-1];
    L[ih*2+0]=l[0]; L[ih*2+1]=l[nPts-1];
  }

  szGraph[0]  = new TGraph(nH*2  , X, Y);
  szGraph[1]  = new TGraph(nH*2  , L, Z);
// 
  myCanvas = new TCanvas("THelixTrack_Show","",600,800);
  myCanvas->Divide(1,2);
  for (int ipad=0;ipad<2;ipad++) {
    myCanvas->cd(ipad+1); 
    szGraph[ipad]->Draw("AP");
    for (int ih = 0;ih<nH;ih++) {
      ptGraph[ih][ipad]->Draw("same *");
      ciGraph[ih][ipad]->Draw("same CP");
  } }


  myCanvas->Modified();
  myCanvas->Update();
  while(!gSystem->ProcessEvents()){}; 

}

//______________________________________________________________________________
//______________________________________________________________________________
//______________________________________________________________________________
//______________________________________________________________________________
ClassImp(TCircle)
//______________________________________________________________________________
TCircle::TCircle()
{
  Set(0,0,0.);
  fEmx = 0;
}
//______________________________________________________________________________
TCircle::~TCircle()
{delete fEmx;fEmx=0;}

//______________________________________________________________________________
TCircle::TCircle(const double *x,const double *d,double rho)
{
  Set(x,d,rho);
  fEmx = 0;
}
//______________________________________________________________________________
void TCircle::Set(const double *x,const double *d,double rho)
{
  fX[0]=0; fX[1]=0; fD[0]=0; fD[1]=0;
  if (x) {fX[0]=x[0];fX[1]=x[1];}
  if (d) {
    fD[0]=d[0];fD[1]=d[1];
    double n = sqrt(fD[0]*fD[0]+fD[1]*fD[1]);
    fD[0]/=n; fD[1]/=n;
  }
  fRho = rho;
}
//______________________________________________________________________________
TCircle::TCircle(const TCircle& fr)
{
  fEmx = 0;
  *this = fr;
}
//______________________________________________________________________________
TCircle::TCircle(const TCircle* fr)
{
  fEmx = 0;
  Set(fr->fX,fr->fD,fr->fRho);
}
//______________________________________________________________________________
TCircle &TCircle::operator=(const TCircle& fr)
{
  Set(fr.fX,fr.fD,fr.fRho);
  if (fr.fEmx) SetEmx(fr.fEmx->Arr());
  return *this;
}

//______________________________________________________________________________
void TCircle::Clear(const char *)   
{
 if (fEmx) fEmx->Clear();
 memset(fX,0,(char*)(&fEmx)-(char*)fX);
}


//______________________________________________________________________________
void TCircle::SetEmx(const double *err)   
{ 
  if(!fEmx) fEmx = new TCEmx_t;
  fEmx->Set(err);
}
//______________________________________________________________________________
void TCircle::Nor(double *norVec) const  
{
// direction from center of circle

  norVec[0] =  fD[1];    norVec[1] = -fD[0];
  if (fRho>=0) return;
  norVec[0] = -norVec[0];norVec[1] = -norVec[1];
}
//______________________________________________________________________________
void TCircle::Print(const char* txt) const
{
  if (!txt) txt="";
  printf("TCircle(%s): x,y=%g %g dir=%g %g curv=%g\n",txt,fX[0],fX[1],fD[0],fD[1],fRho);
  if (!fEmx) return;
  printf("Errs: %g\n"          ,fEmx->mHH); 
  printf("    : %g \t%g\n"     ,fEmx->mHA,fEmx->mAA); 
  printf("    : %g \t%g \t%g\n",fEmx->mHC,fEmx->mAC,fEmx->mCC); 
}
//______________________________________________________________________________
double TCircle::Path(const double pnt[2]) const
{
  TComplex CX1(pnt[0]-fX[0],pnt[1]-fX[1]);
  TComplex CP(fD[0],fD[1]);
  TComplex CXP = TComplex(0,1)*CX1/CP;
  TComplex CXPRho = CXP*fRho;
  double s;
  if (TComplex::Abs(CXPRho)>0.001) {
    s = TComplex::Log(1.+CXPRho).Im()/fRho;
  } else {
    s = (CXP*(1.-CXPRho*(0.5-CXPRho*(1/3.-CXPRho*0.25)))).Im();
  }
//   Check
//   double x[2],d[2];
//   Eval(s,x,d) ;
//   assert(fabs((pnt[0]-x[0])*d[0]+(pnt[1]-x[1])*d[1])<1e-6);
   return s;
}
//______________________________________________________________________________
double TCircle::Path(const double *pnt,const double *exy) const
{
  double x[2],d[2];
  double s = Path(pnt);
  Eval(s,x,d);
  double k = (x[0]-pnt[0])*(d[1]) + (x[1]-pnt[1])*(-d[0]);
  double t =((d[1]*d[1]-d[0]*d[0])*exy[1]-d[1]*d[0]*(exy[2]-exy[0]))
           /( d[0]*d[0]*exy[2] -2*d[1]*d[0]*exy[1]+d[1]*d[1]*exy[0]);

  return s+k*t;
}
//_____________________________________________________________________________
double TCircle::Path(const TCircle &hlx,double *S2) const
{
  double s,s1=3e33,s2=3e33;
  const static TComplex Im(0.,1.);
  const TCircle *one = this;
  const TCircle *two = &hlx;
  if (fabs(fRho) > fabs(hlx.fRho)) { one=two; two=this; }
  double Rho1 = one->Rho();
  double Rho2 = two->Rho();
  int kase = 0;
  if (fabs(Rho1) > 1e-4) kase+=1;
  if (fabs(Rho2) > 1e-4) kase+=2;
  
  int nSol = 0;
  TComplex CX1(one->Pos()[0],one->Pos()[1]);
  TComplex CX2(two->Pos()[0],two->Pos()[1]);
  TComplex CP1(one->Dir()[0],one->Dir()[1]);
  TComplex CP2(two->Dir()[0],two->Dir()[1]);
  TComplex CdX = CX2-CX1;
  double L[2];
  switch(kase) {
    case 2:;
    case 3: {
      if (kase==3) {
	TComplex A = CP1/CP2*(Rho2/Rho1);
	TComplex B = 1.-CdX/CP2*(Im*Rho2) - CP1/CP2*(Rho2/Rho1);
	double a = A.Rho();
	double b = B.Rho();
	double alfa = A.Theta();
	double beta = B.Theta();
	double myCos = (1.-(a*a+b*b))/(2.*a*b);
	double myAng = 0;
             if (myCos>= 1.)	{nSol=1; myAng = 0.		;}
	else if (myCos<=-1.) 	{nSol=1; myAng = M_PI		;}
	else      		{nSol=2; myAng = acos(myCos)	;}
	s  = ( myAng -(alfa-beta))/Rho1;
        if (s<0) s+= 2.*M_PI/fabs(Rho1);
        s1 = s;
	if (nSol==2) {
	  s =(-myAng -(alfa-beta))/Rho1;
          if (s< 0) s+= 2.*M_PI/fabs(Rho1);
          if (s<s1) s1 = s;
	}
      } else {
	TComplex A = CP1/CP2*(Im*Rho2);
	TComplex B = 1.-CdX/CP2*(Im*Rho2);
	double cba[3]={B.Rho2()-1., 2*(A.Re()*B.Re()+A.Im()*B.Im()), A.Rho2()};
	nSol = SqEqu(cba, L);
	if (nSol< 0) break;
	if (nSol==0) nSol=1;
        if (L[0]>0) s1=L[0];
        if (nSol>1 && L[1]>0 && L[1] <s1) s1 = L[1];
      }

      break;
    }// end case 3
    case 0: {
      if (fabs((CdX/CP1).Im())>fabs((CP2/CP1).Im())*1e6) break;
      nSol = 1;
      s =  (CdX/CP2).Im()/(CP1/CP2).Im();
      if (s<0) break;
      s1 = s;
      break;   
    }//end case 0
    default: assert(0);
  }
  if (nSol) {
    TCircle tc1(*one),tc2(*two);
    double xy[2];
    tc1.Eval(s1,xy);   
    s = tc2.Path(xy);
    if (s<0 && kase) s += 2.*M_PI/fabs(Rho2);
    if (s>0 ) s2 = s;
  }


  if (one != this) {s=s1;s1=s2;s2=s;}
  if (S2) *S2=s2;
  return s1;
}
//_____________________________________________________________________________
void TCircle::Test4() 
{
  double pars[4][2][5] = {
   {{0,0,1,0.,-0.001},{0,0.0,1,0,0.002}},
   {{0,0,1,0.,-0.001},{0,0.1,1,0,0.002}},
   {{0,0,1,0.,-0.001},{0,0.0,1,1,1e-8 }},
   {{0,0,1,0.,-1e-8 },{0,0.0,1,1,1e-8 }}};
  for (int ip=0;ip<4;ip++) {
    TCircle tc1(pars[ip][0],pars[ip][0]+2,pars[ip][0][4]); 
    TCircle tc2(pars[ip][1],pars[ip][1]+2,pars[ip][1][4]); 
    tc1.Move(-20);
    tc2.Move(-20);
    double s2;
    double s1 = tc1.Path(tc2,&s2);
    printf("s1=%g s2=%g \n",s1,s2);
  }  
}   
//______________________________________________________________________________
double TCircle::Eval(double step,double *X, double *D) const
{
  
  sgCX1		=TComplex(fX[0],fX[1]);
  sgCD1		=TComplex(fD[0],fD[1]);		//  exp(I*Fi0)
  sgImTet	=TComplex(0,step*fRho);		//  I*Rho*L
  sgCOne        =expOne(sgImTet);		// (Exp(I*Rho*L)-1)/(I*Rho*L)
  sgCf1 	=sgImTet*sgCOne;		// (Exp(I*Rho*L)-1)
  
  sgCD2 = sgCD1*sgCf1+sgCD1; 			// exp(I*Fi0+I*Rho*L)
  sgCX2 = sgCD1*sgCOne*step;			// exp(I*Fi0)*(exp(I*Rho*L)-1)/(I*Rho)
  if (X) {
    X[0] = sgCX2.Re()+sgCX1.Re();
    X[1] = sgCX2.Im()+sgCX1.Im();
  }
  if (D) {
    sgCD2/= TComplex::Abs(sgCD2);
    D[0] = sgCD2.Re();
    D[1] = sgCD2.Im();
  }
  return step;
}
//______________________________________________________________________________
double TCircle::Move(double step)
{
  Eval(step,fX,fD);
  if (fEmx && fEmx->mHH>0) MoveErrs(step);
  if (fabs(fD[0])>1) fD[0]=(fD[0]<0)? -1:1;
  if (fabs(fD[1])>1) fD[1]=(fD[1]<0)? -1:1;
  return step;
}
//______________________________________________________________________________
void TCircle::MakeMtx(double S,double F[3][3])
{
  enum {kH=0,kA,kC};
  memset(F[0],0,sizeof(F[0][0])*3*3);
  F[kH][kH]   = sgCf1.Re()+1.;
  double dSdH = sgCf1.Im();

  F[kH][kA]   = S*sgCOne.Re();
  double dSdA = S*sgCOne.Im();
  TComplex llCOneD = S*S*expOneD(-sgImTet);
  F[kH][kC]   = llCOneD.Re();
  double dSdC = llCOneD.Im();

  F[kA][kH] =  -dSdH*fRho;
  F[kA][kA] = 1-dSdA*fRho;
  F[kA][kC] = S+dSdC*fRho;
  F[kC][kC] = 1;
}
//______________________________________________________________________________
void TCircle::MoveErrs(double l)
{
  double F[3][3];
  MakeMtx(l,F);
  fEmx->Move(F);
}
//______________________________________________________________________________
void TCircle::Rot(double angle)
{
  Rot(cos(angle),sin(angle));
}
//______________________________________________________________________________
void TCircle::Rot(double cosa,double sina)
{
  TComplex CX(fX[0],fX[1]),CP(fD[0],fD[1]);
  TComplex A (cosa,sina);
  CX *=A; fX[0] = CX.Re(); fX[1]=CX.Im();
  CP *=A; CP/=TComplex::Abs(CP);
  fD[0] = CP.Re(); fD[1]=CP.Im();
}
//______________________________________________________________________________
void TCircle::Backward()
{
  fRho=-fRho;fD[0]=-fD[0];fD[1]=-fD[1];
  if(fEmx) fEmx->Backward();
}

//______________________________________________________________________________
void TCircle::Test2() 
{
// double xyz[4][3]= {{-39.530250549316406, -165.19537353515625, 184.05630493164062}
//                   ,{-37.718906402587891, -167.19537353515625, 186.41175842285156}
// 		  ,{-35.468486785888672, -169.19537353515625, 189.05546569824219}
//                   ,{-33.657142639160156, -171.19537353515625, 191.347900390625}};
// double x[4],y[4];
// for (int i=0;i<4;i++) { x[i]=xyz[i][0];  y[i]=xyz[i][1]; }
// 
// 
// 
// TCircle TC;
// double qa0 = TC.Approx(4,xyz[0],3);
// double qa1 = TC.Resid (4,xyz[0],3);
// printf("Approx qa0 = %g qa1=%g\n",qa0,qa1);
// TC.Print();
// 

}
//______________________________________________________________________________
void TCircle::Test3() 
{
// enum {nPnts=4};
// double xyz[nPnts][3] = 
// {{80.815544128417969, 159.77731323242188, 129.11553955078125}
// ,{82.239913940429688, 161.25840759277344, 131.24034118652344}
// ,{84.462181091308594, 162.28025817871094, 133.59538269042969}
// ,{86.321846008300781, 163.51133728027344, 135.19621276855469}};
// 
// double err[nPnts][4] = 
// {{0.0010703595155359307, 0.00061836299089800776, 0.00035723771589107141,0.0032088035791992191}
// ,{0.0010505530116463389, 0.00060692047199979574, 0.00035062719848397145,0.0031350950603759769}
// ,{0.0010286003088986414, 0.00059423806134026682, 0.00034330037672605356,0.0030533996126220703}
// ,{0.0010136781863030494, 0.00058561716272119912, 0.00033831985920934062,0.0029978674575439454}};
// 
// 
// double res;
// TCircle circ;
// res=circ.Approx(nPnts,xyz[0],3);
// printf("res = %g \n",res);
// circ.Print();
// res=circ.Resid (nPnts,xyz[0],3);
// printf("res = %g \n",res);
// circ.Print();
// 
// circ.Show(nPnts,xyz[0],3);
// res = circ.Fit(nPnts,xyz[0],3,err[0],4);
// printf("res = %g \n",res);
// circ.Print();
// circ.Show(nPnts,xyz[0],3);

}
//______________________________________________________________________________
void TCircle::TestMtx() 
{
  double Dir[8],X[8]={0},Rho[2],step,F[3][3],Del[3],Dif[3]={0};
  double maxEps = 0;  
  step = 20;
  int nErr=0;
  for (int iR = 10;iR<100;iR+=10) {
    Rho[0] = 1./iR;
    for (int iAlf=30;iAlf<=30;iAlf+=10){  
      Dir[0] = cos(iAlf/180.*M_PI);
      Dir[1] = sin(iAlf/180.*M_PI);
      TCircle tc(X,Dir,Rho[0]);
      tc.Eval(step,X+2,Dir+2);
      tc.MakeMtx(step,F);

      for (int iHAR=0;iHAR<3;iHAR++) {
        memcpy(X  +4,X  ,sizeof(X[0]  )*2);
        memcpy(Dir+4,Dir,sizeof(Dir[0])*2);
        memset(Del,0,sizeof(Del));
        Rho[1]=Rho[0];
        switch (iHAR) {
	case 0: { 
	  Del[0] = 0.001*iR;
          X[4+0] = X[0]-Dir[1]*Del[0];
          X[4+1] = X[1]+Dir[0]*Del[0];
          break;}
	  
	case 1: {
	  Del[1] = M_PI/180*0.1;
          Dir[4+0] = cos(iAlf/180.*M_PI+Del[1]);
          Dir[4+1] = sin(iAlf/180.*M_PI+Del[1]);
          break;}

	case 2: {
          Del[2] = Rho[0]*0.005;
          Rho[1] = Rho[0]+Del[2];
          break;}
        }//end switch
      
        TCircle tcc(X+4,Dir+4,Rho[1]);
        double myStep = tcc.Path(X+2);
        tcc.Eval(myStep,X+6,Dir+6);
        for (int jHAR=0;jHAR<2; jHAR++) {
	  switch(jHAR) {
	  case 0: {
	    Dif[0] = (X[6+0]-X[2+0])*(-Dir[2+1])
	           + (X[6+1]-X[2+1])*( Dir[2+0]);
            break;}
	  case 1: {
	    Dif[1] = (atan2(Dir[6+1],Dir[6+0])
	           -  atan2(Dir[2+1],Dir[2+0])); 
            if (Dif[1]>  M_PI) Dif[1]-=2*M_PI;
            if (Dif[1]< -M_PI) Dif[1]+=2*M_PI;
            break;}
          }
          double est = Dif[jHAR]/Del[iHAR];
	  double eps = fabs(est-F[jHAR][iHAR])*2
	             /(fabs(est)+fabs(F[jHAR][iHAR]+1e-12));
          if (eps>maxEps) maxEps=eps;
          if (eps < 1e-2) continue;
          nErr++;
          printf(" Mtx[%d][%d] %g %g Angle=%d Rad=%d Len=%g\n",
	         jHAR,iHAR,F[jHAR][iHAR],est,
		 iAlf,iR,myStep);
	
    } } } } 
    printf("TestMtx: %d errors maxEps=%g\n",nErr,maxEps);

}


//______________________________________________________________________________
//______________________________________________________________________________
TCircleFitter::TCircleFitter()
{
  Clear();
  SetEmx();
}
//______________________________________________________________________________
void TCircleFitter::Clear(const char*)
{
   fArr.Reset();
   memset(fBeg,0,fEnd-fBeg+1);
   TCircle::Clear();
}
//______________________________________________________________________________
TCircleFitterAux* TCircleFitter::GetAux(int i) const
{
  return (TCircleFitterAux*)(fArr.GetArray()+i*TCircleFitterAux::dSize());
}
//______________________________________________________________________________
const double* TCircleFitter::GetX(int i) const
{
  return &(fAux[i].x);
}
//______________________________________________________________________________
double* TCircleFitter::GetX(int i) 
{
  return &(fAux[i].x);
}
//______________________________________________________________________________
void  TCircleFitter::Add(double x,double y,const double *errs) 
{
  fNuse =++fN;
  int n = fN*TCircleFitterAux::dSize();
  if (fArr.GetSize()<n) {fArr.Set(n*2);fAux=0;}
  if (!fAux) fAux = GetAux(0);
  TCircleFitterAux *aux = fAux+fN-1;
  aux->x = x; aux->y=y; aux->exy[0]=1; aux->exy[2]=1; aux->ezz=1;aux->wt=1;
  if (errs) AddErr(errs);
}
//______________________________________________________________________________
void  TCircleFitter::Add(double x,double y,double z) 
{
  fNuse =++fN;
  int n = fN*TCircleFitterAux::dSize();
  if (fArr.GetSize()<n) {fArr.Set(n*2);fAux=0;}
  if (!fAux) fAux = GetAux(0);
  TCircleFitterAux *aux = fAux+fN-1;
  aux->x = x; aux->y=y; aux->z=z;
  aux->exy[0]=1; aux->exy[1]=0; aux->exy[2]=1;aux->ezz=1;aux->wt=1;
}
//______________________________________________________________________________
void  TCircleFitter::AddErr(const double *errs,double ezz) 
{
  TCircleFitterAux *aux = fAux+fN-1;
  assert(errs[0]>=0);
  assert(errs[2]>=0);
  double spur = errs[0]+errs[2];
  assert(spur>0);
  double *e = aux->exy;
  memcpy(e,errs,sizeof(aux->exy));

  if (e[0]<0 && e[0]>-1e-5*spur) {e[0]=0;e[1]=0;}
  if (e[2]<0 && e[2]>-1e-5*spur) {e[2]=0;e[1]=0;}
  assert(e[1]*e[1]<=1.01*e[0]*e[2]);


  aux->ezz = ezz;
}
//______________________________________________________________________________
void  TCircleFitter::AddZ(double z,double ez) 
{
// Must be called immediatelly after Add(...)
  fAux[fN-1].z  =z;
  fAux[fN-1].ezz=ez;
}
//______________________________________________________________________________
double TCircleFitter::Fit() 
{
static const int nAVERs = &fRr-&fXx;
static int nCall=0; nCall++;
    int i;
    double xx, yy, xx2, yy2;
    double f, g, h, p, q, t, g0, g02, a=0, b=0, c=0, d=0;
    double xroot, ff, fp;
    double dx, dy, nx,ny, xnom,wt,hord,tmp,radius2,radiuc2;
    fKase = fCase;
    if (fNuse < 3) return 3e33;
    TCircleFitterAux *aux = GetAux(0);
    dx = aux[fN-1].x - aux[0].x;
    dy = aux[fN-1].y - aux[0].y;
    hord = sqrt(dx*dx+dy*dy);
    fCos = dx/hord;
    fSin = dy/hord;
    int withErr = aux[0].exy[0]+aux[0].exy[2]>0;
    fNor[0] = -fSin,fNor[1] = fCos;
    int nter= (withErr)? 2:1;
    for (int iter=0;iter<nter;iter++) {
      fWtot = 0;
      memset(&fXgravity,0,sizeof(double)*(nAVERs+2));
      for (i=0; i<fN; i++) {
        if (aux[i].wt<0) continue;
        wt = 1;
	if (withErr) {
          if (iter) {
            fNor[0] = fXCenter - aux[i].x;
            fNor[1] = fYCenter - aux[i].y;
            tmp = sqrt(fNor[0]*fNor[0]+fNor[1]*fNor[1]);
            fNor[0]/=tmp; fNor[1]/=tmp; 
	  } 
          const double *exy = aux[i].exy;
          wt = (fNor[0]*fNor[0]*exy[0]
	       +fNor[0]*fNor[1]*exy[1]*2
	       +fNor[1]*fNor[1]*exy[2]);
//          assert(wt>0.);
          if (wt<1e-8) wt = 1e-8;
          wt = 1/wt;
        }
        aux[i].wt = wt;
        fWtot += wt;
	fXgravity += aux[i].x *wt;
	fYgravity += aux[i].y *wt;
      }
      fXgravity /= fWtot;
      fYgravity /= fWtot;

      for (i=0; i<fN; i++) {
	  dx  = aux[i].x-fXgravity;
	  dy  = aux[i].y-fYgravity;
	  xx  =  dx*fCos + dy*fSin;
	  yy  = -dx*fSin + dy*fCos;
	  xx2 = xx*xx;
	  yy2 = yy*yy;
          wt  = aux[i].wt;
	  fXx    += xx2 		*wt;
	  fYy    += yy2 		*wt;
	  fXy    += xx*yy 		*wt;
	  fXrr   += xx*(xx2+yy2) 	*wt;
	  fYrr   += yy*(xx2+yy2) 	*wt;
	  fRrrr += (xx2+yy2)*(xx2+yy2) 	*wt;
      }
      double *dd = &fXx;
      for (i=0;i<nAVERs;i++) {dd[i]/=fWtot;}
      fRr = fXx+fYy;

      if (fNuse <= 3) fKase=1;
    if (!fKase) fKase =(fYy < fXx *(0.5)*(0.5)/210)? 1:2;
//      if (!fKase) fKase =(fYy < fXx *(0.5)*(0.5)/10)? 1:2;
SWIT:
      switch(fKase) {
        case 1:	{	//Try 1st method

//  		Variables v0=1, v1=2*x, v2 =-rr == -(x*x+y*y)
//  		Orthogonal functions of these variables:
//  		Nor0 = fPol[0]
//  		Nor1 = fPol[1]+ v1*fPol[2]
//  		Nor2 = fPol[3]+ v1*fPol[4]+ v2*fPol[5] 
    
	  double myCof[3];    
	  fPol[0] = 1;
	  fPol[1] = 0;    fPol[2] = 1./(2*sqrt(fXx));
	  fPol[3] = fRr;  fPol[4] = fXrr/(2*fXx);   fPol[5] = 1.;
	  double tmp = sqrt(fPol[3]*fPol[3]
                	   +fPol[4]*fPol[4]*(4*fXx  )
	        	   +fPol[5]*fPol[5]*(fRrrr  )
                	   +fPol[3]*fPol[5]*(-fRr   ) *2
                	   +fPol[4]*fPol[5]*(-2*fXrr) *2);
	  fPol[3]/=tmp;fPol[4]/=tmp;fPol[5]/=tmp;
	  myCof[0] =   0;
	  myCof[1] = - (fPol[2]*(4*fXy));
	  myCof[2] = - (fPol[4]*(4*fXy) + fPol[5]*(-2*fYrr));
	  c = myCof[0]*fPol[0]+myCof[1]*fPol[1]+myCof[2]*fPol[3];
	  a =                  myCof[1]*fPol[2]+myCof[2]*fPol[4];
	  b =                                   myCof[2]*fPol[5];
          fYd = (fabs(b)>1e-6) ? 1./b : 1e6;
          fXd = a*fYd;
        }// end case 1
        break;

        case 2:	{	//Try 2nd method(Ososkov/Chernov)

	  f = (3.*fXx+fYy);
	  g = (fXx+3.*fYy);
	  h = 2*fXy;
	  p = fXrr;
	  q = fYrr;
	  t = fRrrr;
	  g0 = (fXx+fYy);
	  g02 = g0*g0;
	  a = -4.0;
	  b = (f*g-t-h*h)/g02;
	  c = (t*(f+g)-2.*(p*p+q*q))/(g02*g0);
	  d = (t*(h*h-f*g)+2.*(p*p*g+q*q*f)-4.*p*q*h)/(g02*g02);
	  xroot = 1.0;
	  for (i=0; i<5; i++) {
	      ff = (((xroot+a)*xroot+b)*xroot+c)*xroot+d;
	      fp = ((4.*xroot+3.*a)*xroot+2.*b)*xroot+c;
	      xroot -= ff/fp;
	  }
	  fG1 = xroot*g0;
	  xnom = (g-fG1)*(f-fG1)-h*h;

//	  assert(xnom>3e-33);
          if (xnom<1e-20) { fKase=1; goto SWIT;}

	  fXd = ( p*(g-fG1)-q*h      )/xnom;
	  fYd = (-p*h      +q*(f-fG1))/xnom;
        }//end case 2
        break;
        
	default: assert(0);
      } //end switch
      fXCenter = fXd*fCos-fYd*fSin + fXgravity;
      fYCenter = fXd*fSin+fYd*fCos + fYgravity;
    }// end iters
    
//	Update TCircle
    switch (fKase) {    
      case 1:  {//Big R approx
	fCorrR = sqrt(1+a*a+c*b );
	fCorrB = sqrt(1+a*a     );
	fRho = fabs(b)/fCorrR;
	int sgB = (b<0)? -1:1;
	ny = sgB/sqrt(1+a*a);
	nx = a*ny;
	fH = -c*sgB/(fCorrR+fCorrB);
	fChi2 = (4*a*fXy +4*fYy- 2*b*fYrr)/4;
	fChi2 /= (fCorrR*fCorrR);
      } 
      break;
      case 2:  {//Ososkov
	radiuc2  = fXd*fXd+fYd*fYd;
	radius2  = radiuc2+fG1;
	double radius = ::sqrt(radius2);
	double radiuc = ::sqrt(radiuc2);
	fRho  = 1./radius;
	fH = -fG1/(radius+radiuc);
	nx = fXd/radiuc;
	ny = fYd/radiuc;
	fChi2 = (fG1-fRr)/2;
      }
      break;
      default: assert(0);
    }
    fNdf = fNuse-3;
    if (fNdf>0) fChi2 *= fWtot/fNdf;
    fA=a;fB=b;fC=c;
    fX[0] = nx*fH; fX[1] = ny*fH;
// 	let we are moving left --> right
//    	remember to change sign of correlation related to H if fRho<0
    fD[0] = ny; fD[1] =-nx;  
//
    Rot(fCos,fSin);
    fX[0] +=  fXgravity;
    fX[1] +=  fYgravity;
    tmp = fD[0]*(aux[0].x-fX[0])+fD[1]*(aux[0].y-fX[1]);
//	remember to change corrs related to rho and h
    fBack = 0;
    if (tmp>0) {fD[0]*=-1;fD[1]*=-1;fRho*=-1;fBack=1;}
    return fChi2;
}
//______________________________________________________________________________
void TCircleFitter::MakeErrs() 
{
   fEmx->Clear();
   double F[3][3]; memset(F[0],0,sizeof(F));
   double myFact = 1.;
   switch (fKase) {
     case 1: { //For BigYC  fit
       fCov[0] = fPol[2]*fPol[2]+ fPol[4]*fPol[4];
       fCov[1] = fPol[4]*fPol[5];
       fCov[2] = fPol[5]*fPol[5];
       fCov[3] = fPol[1]*fPol[2]+ fPol[3]*fPol[4];
       fCov[4] = fPol[3]*fPol[5];
       fCov[5] = fPol[0]*fPol[0]+ fPol[1]*fPol[1]+fPol[3]*fPol[3];
       for (int i=0;i<6;i++) {fCov[i]*=4;}
       int sgB = (fB<0)? -1:1;
       double corrRB = fCorrR+fCorrB;
       double corrR3 = fCorrR*fCorrR*fCorrR;
 //      fH = -c*sgB/(fCorrR+fCorrB);

       F[0][0] =      sgB*fA*fC/(corrRB*fCorrB*fCorrR);		//dH/da
       F[0][1] =  0.5*sgB*fC*fC/(corrRB*corrRB*fCorrR);		//dH/db
       F[0][2] =  0.5*sgB*fC*fB/(corrRB*corrRB*fCorrR)		//dH/dc
               -      sgB      /(corrRB              );
       F[1][0] =  -1/(fCorrB*fCorrB);				//dFi/da
       F[2][0] =  -   sgB*fA*fB/(corrR3);			//d(aRho)/da
       F[2][1] =  -0.5*sgB*fC*fB/(corrR3)+sgB/fCorrR;		//d(aRho)/db
       F[2][2] =  -0.5*sgB*fB*fB/(corrR3);			//d(aRho)/dc
       myFact  = (fCorrR*fCorrR);
       break;
     }
      case 2:    { //For Ososkov/Chernov fit
// <F> = (C-<(x**2 +y**2)>)/(2*R)
// <dF/dA*dF/dA> = (<x*x>*R - <F>*A*A)/R**3
// <dF/dB*dF/dA> = (<x*y>*R - <F>*A*B)/R**3
// <dF/dB*dF/dB> = (<y*y>*R - <F>*B*B)/R**3
// <dF/dC*dF/dA> = -<F>*A  /(2*R**3)
// <dF/dC*dF/dB> = -<F>*B  /(2*R**3)
// <dF/dC*dF/dC> =  (R-<F>)/(4*R**3)

	double aRho = fabs(fRho);
	double aRho2 = aRho*aRho;
	double Fm   = (fG1 - (fXx+fYy))*aRho/2;
	fCov[0]  = (fXx - Fm*aRho*fXd*fXd)*aRho2;	//  <dF/dA * dF/dA>
	fCov[1]  = (fXy - Fm*aRho*fXd*fYd)*aRho2;  	//  <dF/dA * dF/dB>
	fCov[2]  = (fYy - Fm*aRho*fYd*fYd)*aRho2;	//  <dF/dB * dF/dB>
	double aRho3 = aRho*aRho2;
	fCov[3]  = -0.5*(Fm*fXd    )*aRho3;		//  <dF/dC * dF/dA>
	fCov[4]  = -0.5*(Fm*fYd    )*aRho3;		//  <dF/dC * dF/dB>
	fCov[5]  =  0.25*(1-Fm*aRho)*aRho2;		//  <dF/dC * dF/dC>

//   h = Rx - R
	double xyRho2= 1./(fXd*fXd+fYd*fYd);
	double xyRho = sqrt(xyRho2);
	double tmp = fG1*aRho2*xyRho2/(aRho+xyRho);

	F[0][0] =  fXd*tmp;		// dH /dXd
	F[0][1] =  fYd*tmp;		// dH /dYd
	F[0][2] = -0.5*aRho;		// dH /dG1
	F[1][0] = -fYd*xyRho2;		// dFi/dXd
	F[1][1] =  fXd*xyRho2;		// dFi/dYd
	F[1][2] = 0;			// dFi/dG1
	F[2][0] = -fXd*aRho3;		// dRho/dXd 
	F[2][1] = -fYd*aRho3;		// dRho/dYd 
	F[2][2] = -0.5*aRho3;		// dRho/dG1 

	TCL::trsinv(fCov,fCov       ,3);
        break;
      }
      default: assert(0);
   } // end switch
   TCL::trasat(F[0],fCov,fEmx->Arr(),3,3); 
   TCL::vscale(fEmx->Arr(),myFact/fWtot,fEmx->Arr(),6);
   if (fBack) {fEmx->mHA*=-1;fEmx->mAC*=-1;}
}
//______________________________________________________________________________
double TCircleFitter::EvalChi2() 
{
  if (!fNuse) return 0;
  TCircle M(this);
  double sum = 0,wtot=0,wt;
  TCircleFitterAux *aux = GetAux(0);
  const double *p = M.Pos();
  for (int i=0;i<fN;i++) {
    if (aux[i].wt<0) continue;
    double s = M.Path(&(aux[i].x));
    M.Move(s);
    wt = aux[i].wt;
    sum+= (pow(p[0]-aux[i].x,2)+pow(p[1]-aux[i].y,2))*wt;
    wtot +=wt;
  }
  if (fNdf) sum /= fNdf;
  fChi2 = sum;
  return sum;
}
//_____________________________________________________________________________
double TCircleFitter::FixAt(const double vals[5],int flag) 
{
///  void TCircleFitter::FixAt(const double vals[4],double curv,int flag) 
///  fix circle at specific x,y;Psi;Curv
///  vals[0,1]	- x,y
///  vals[2]    - reserved for Z, not used here
///  vals[3]   	- Psi
///  vals[4]	- Curvature
///  flag	- +1=xy fix,+2=Psi fix,+4 =curv fix

   assert(fEmx);
   assert(flag);
   double g[6]={1,0,1,0,0,1},c[6]={1,0,1,0,0,1}
         ,e[6],adj[3]={0},amda[3],dlt[2];
   int sel[3] ={!!(flag&1), !!(flag&2), !!(flag&4)};
   int nFix=0;
   if (sel[0]) {  	// h corr
     nFix++;
     dlt[0] = vals[0]-fX[0]; dlt[1] = vals[1]-fX[1];
     adj[0] = -dlt[0]*fD[1]+dlt[1]*fD[0];
   }
   if (sel[1]) {	// angle corr
     nFix++;
     adj[1] = vals[3]-atan2(fD[1],fD[0]);
     if (adj[1]< -M_PI) adj[1] += 2*M_PI;
     if (adj[1]>  M_PI) adj[1] -= 2*M_PI;
   }
   if (sel[2]) {	//curv corr
     nFix++;
     adj[2] = vals[4]-fRho;
   }
//	calculate add to Chisq
   for (int i=0,li=0;i< 3;li+=++i) {
     for (int j=0   ;j<=i;j++    ) {
       if (!(sel[i]&sel[j])) continue;
       c[li+j] = (*fEmx)[li+j];     } }
   double addXi2=0;
   TCL::trsinv(c        ,c   ,3  );
   TCL::trasat(adj,c,&addXi2,1,3); 

   TCL::trsinv(fEmx->Arr(),e,3);
   for (int i=0,li=0;i< 3;li+=++i) {
     for (int j=0   ;j<=i;j++    ) {
       if (!(sel[i]|sel[j])) continue;
       e[li+j] = (i==j);
       if (!(sel[i]&sel[j])) continue;
       g[li+j] = (*fEmx)[li+j];
   } }
   TCL::trsinv(g        ,g   ,3  );
   TCL::trsa  (g   ,adj ,amda,3,1);
   TCL::trsa  (fEmx->Arr(),amda,adj   ,3,1);
   TCL::trsinv(e          ,fEmx->Arr(),3  );

   for (int i=0,li=0;i< 3;li+=++i) {if (sel[i]) (*fEmx)[li+i]=0;}
//     	update x,y
   fX[0] += -adj[0]*fD[1];
   fX[1] +=  adj[0]*fD[0];
//  	update direction
//    double S = adj[1]*(1-adj[1]*adj[1]/6);
//    double C = 1-adj[1]*adj[1]/2;
//   double S = sin(adj[1]);
   double S = sin(adj[1]);
   double C = cos(adj[1]);
   double d0 = fD[0];
   fD[0] = d0*C-fD[1]*S;
   fD[1] = d0*S+fD[1]*C;
//  	update curvature
   fRho += adj[2];
   fNdf+=nFix;
   fChi2 += (addXi2-fChi2*nFix)/fNdf;
   return fChi2;
}
//_____________________________________________________________________________
void TCircleFitter::Skip(int idx) 
{
  fAux[idx].exy[0] = -1;
  SetNdf(fNdf-1);	//compensate increasing it inside FixAt(double*)
}
//_____________________________________________________________________________
void TCircleFitter::SetNdf(int ndf) 
{
  fChi2*=fNdf; if (ndf) fChi2/=ndf; fNdf=ndf;
}
//______________________________________________________________________________
void TCircleFitter::Print(const char* txt) const
{
  if (!txt) txt="";
  printf("TCircleFitter::NPoints = %d method=%d",fN,fKase);
  if (fChi2) printf(" Chi2 = %g",fChi2);
  printf("\n");
  TCircle::Print();

  int iP = (strchr(txt,'P') || strchr(txt,'p'));
  int iE = (strchr(txt,'E') || strchr(txt,'e'));
  int iF = (strchr(txt,'F') || strchr(txt,'f'));
  int iZ = (strchr(txt,'Z') || strchr(txt,'z'));if(iZ){};
  TCircleFitterAux *aux = GetAux(0);
  if (iP) { //Print points
    for (int ip=0;ip<fN;ip++) {
      printf("%3d - X: %g\tY:%g \tZ:%g",ip,aux[ip].x,aux[ip].y,aux[ip].z);
      if (iE)  
      printf(" \tExy: %g %g %g \tEz:%g "
            ,aux[ip].exy[0],aux[ip].exy[1],aux[ip].exy[2],aux[ip].ezz);
      printf("\n");
  }}
  if (iF) { //Print fit
    TCircle circ(this);
    const double *xy = GetX(0);
    double ds=circ.Path(xy);
    circ.Move(ds);
    double s=0;
    for (int i=0;i<fN;i++) {
      xy = GetX(i);
      ds = circ.Path(xy);
      s+=ds;
      circ.Move(ds);
      if (fabs( s)<1e-6) s=0;
      if (fabs(ds)<1e-6)ds=0;
      printf("%3d - S=%g(%g) \tX: %g=%g \tY:%g=%g \tdirX=%g dirY=%g\n"
          ,i,s,ds
          ,xy[0],circ.Pos()[0]
          ,xy[1],circ.Pos()[1]
          ,circ.Dir()[0],circ.Dir()[1]);
  }}

}
//______________________________________________________________________________
void TCircleFitter::Test() 
{
  enum {nPts=20};
  double e[4],x[3];    
  double aShift[5];
  aShift[0]=-acos(0.25);
  aShift[1]=-acos(0.50);
  aShift[2]= 0;
  aShift[3]= acos(0.25);
  aShift[5]= acos(0.50);
  double RERR = 0.1;
TRandom ran;
static TCanvas* myCanvas=0;
static TH1F *hh[6]={0,0,0,0,0,0};
static const char *hNams[]={"dH","pH","dA","pA","dC","pC","Xi2",0};
static const char *hTits[]=
{"delta H","pull H","delta Psi","pull Psi","delta Curv","pull Curv","Xi2",0};
static const double lims[]={-0.1,-5,-0.1,-5,-0.1,-5,5};
  int nPads = sizeof(hNams)/sizeof(void*)-1;
  if(!myCanvas)  myCanvas=new TCanvas("TCircleFitter_Test","",600,800);
  myCanvas->Clear();
  myCanvas->Divide(1,nPads);

  for (int i=0;i<nPads;i++) { 
    double limin = lims[i]; if(limin>0) limin=0;
    double limax = fabs(lims[i]); ;
    delete hh[i]; hh[i]= new TH1F(hNams[i],hTits[i],100,limin,limax);
    myCanvas->cd(i+1); hh[i]->Draw();
  }

  int nFit = 0;
  for (int ir = 50; ir <= 1000; ir +=5) 		{//loop over radius
    double aR = ir;
    double len = 100; if (len>aR*3) len = aR*3;
    for (double ang0 = -3; ang0 < 3.1; ang0+=0.05)	{//loop over ang 
      for (int sgn = -1; sgn<=1; sgn+=2)    		{//loop over signes of curv
	double R = sgn*aR;
	double dang = len/R/nPts;
	double C0 = cos(ang0);
	double S0 = sin(ang0);
        TCircleFitter helx;
	for (int is=0;is<nPts;is++) {	//loop over points
          double ang = ang0 + dang*is;
          double S = sin(ang),C = cos(ang);
          double eR = ran.Gaus(0,RERR)*sgn;
          double shift = aShift[is%5];
//shift=0;//???????????????????
          double SS = sin(ang+shift);
          double CC = cos(ang+shift);
          e[0] = pow(RERR*SS,2);
          e[1] =-pow(RERR   ,2)*CC*SS;
          e[2] = pow(RERR*CC,2);

          x[0] = 100 + (R)*(S-S0);
          x[1] = 200 - (R)*(C-C0);
          x[0]+= -SS*eR; 
          x[1]+=  CC*eR; 
          helx.Add (x[0],x[1],e);
	}		//end points
	double Xi2 = helx.Fit();
//if (helx.GetCase()!=1) continue;
//if (R<0) continue;
	nFit++;
	helx.MakeErrs();
	x[0] = 100 ;
	x[1] = 200 ;
	double s = helx.Path(x);
        assert(s<0);
	assert(fabs(s) < len);
	helx.Move(s);
	double dd[10];
	double dx = x[0]-helx.Pos()[0];
	double dy = x[1]-helx.Pos()[1];
	dd[0] = -dx*S0+dy*C0;
	dd[1] = dd[0]/sqrt(helx.Emx()->mHH);
	dd[2] = atan2(helx.Dir()[1],helx.Dir()[0])-ang0;
	if (dd[2]> M_PI) dd[2]-=2*M_PI;
	if (dd[2]<-M_PI) dd[2]+=2*M_PI;
	dd[3] = dd[2]/sqrt(helx.Emx()->mAA);
	dd[4] = helx.Rho()-1./R;
	dd[5] = dd[4]/sqrt(helx.Emx()->mCC);
        dd[6]=Xi2;
	for (int ih=0;ih<nPads;ih++) { hh[ih]->Fill(dd[ih]);}
    } 		//end sign
  }		//end ang0
  } 		// curv
  myCanvas->Modified();
  myCanvas->Update();
  while(!gSystem->ProcessEvents()){}; 

}
//______________________________________________________________________________
void TCircleFitter::Test(int iTest) 
{
  enum {nPts=50};
  double e[4],x[3],chi2[2]={0,0};    
  double aShift[5];
  aShift[0]=-acos(0.25);
  aShift[1]=-acos(0.50);
  aShift[2]= 0;
  aShift[3]= acos(0.25);
  aShift[5]= acos(0.50);

  double R = 1000.;
  if (iTest<0) R = -R;
  iTest = abs(iTest);
  double dang = 3./100;// *0.1;
  printf("TCircle::Test R=%g dS=%g\n",R,dang*R);
  double dR[2],qa0;
  dR[0] = 0.016*(R);
  dR[1] = 4*dR[0];
//  dR = 1e-5;
  double dZ[] = {1,3};
//double ang0=0.5;
  double ang0=0.0;
  double S0=sin(ang0),C0=cos(ang0);
  TCircleFitter helx;
  gRandom->SetSeed();
  for (int i=0;i<nPts;i++) {
    double ang = ang0 + dang*i;
    double S = sin(ang),C = cos(ang);
    if (iTest==1) {
    double dRR = dR[i&1];
    double eR = gRandom->Gaus(0,dRR);
    double shift = aShift[i%5];
    double SS = sin(ang+shift);
    double CC = cos(ang+shift);

    double dif = eR*(CC*C+SS*S);
    chi2[0]+=pow(dif,2);
    chi2[1]+=pow(dif/(dRR*(CC*C+SS*S)),2);
    double eZ = gRandom->Gaus(0,dZ[i&1]);
    e[0] = pow(dRR*SS,2);
    e[1] =-pow(dRR   ,2)*CC*SS;
    e[2] = pow(dRR*CC,2);
    e[3] = pow(dZ[i&1],2);
    x[0] = 100 + fabs((R))*(S-S0);
    x[1] = 200 -     ((R))*(C-C0);
    x[0]+= -SS*eR; 
    x[1]+=  CC*eR; 

    x[2] = 300 -       R*ang*0.5 +eZ;
    helx.Add (x[0],x[1],e);
////    helx.Add (x[0],x[1],0);
    helx.AddZ(x[2],e[3]);

    } else {
    helx.Add(100. + i*1,200 - i*2);
    helx.AddZ(300 - i*3);
    }
  } 
  chi2[0]/=nPts-3;
  chi2[1]/=nPts-3;

  helx.SetCase(1);
  qa0=helx.Fit();
  helx.MakeErrs();
  x[0]=100;x[1]=0;
  S0 = helx.Path(x);
  helx.Move(S0);
  helx.Print("");
  double myChi2 = helx.EvalChi2();
  printf("Res2Ideal=%g Chi2Ideal=%g evalChi2=%g\n\n",chi2[0],chi2[1],myChi2);

  helx.SetCase(2);
  qa0=helx.Fit();
  helx.MakeErrs();
  S0 = helx.Path(x);
  helx.Move(S0);

  helx.Print("");
  myChi2 = helx.EvalChi2();
  printf("Res2Ideal=%g Chi2Ideal=%g evalChi2=%g\n\n",chi2[0],chi2[1],myChi2);


  helx.Move( 100.);
  helx.Print("");
  helx.Move(-100.);
  helx.Print("");


//  helx.Print("F");

}
//______________________________________________________________________________
void TCircleFitter::TestCorr(int kase) 
{
// 1=fit case    1 alowed
// 2=fit case    2 alowed
// 4=fit +ive curv alowed
// 8=fit -ive curv alowed
//16=fit -ive curv alowed

  if (!(kase&3 ))kase+=1+2;
  if (!(kase&12))kase+=4+8;
  enum {nPts=20};
  double e[4],x[3],ex[3];    
  double aShift[5];
  aShift[0]=-acos(0.25);
  aShift[1]=-acos(0.50);
  aShift[2]= 0;
  aShift[3]= acos(0.25);
  aShift[5]= acos(0.50);
  double RERR = 0.001;
TRandom ran;
static TCanvas* myCanvas=0;
static TH1F *hh[6]={0,0,0,0,0,0};
static const char *hNams[]={"HA","HA-","HC","HC-","AC","AC-",0};
  if(!myCanvas)  myCanvas=new TCanvas("TCircleFitter_TestCorr","",600,800);
  myCanvas->Clear();
  myCanvas->Divide(1,6);

  for (int i=0;i<6;i++) { 
    delete hh[i]; hh[i]= new TH1F(hNams[i],hNams[i],100,-1,1);
    myCanvas->cd(i+1); hh[i]->Draw();
  }

  int nFit = 0;
  for (int ir = 50; ir <= 1000; ir +=5) 		{//loop over radius
    double aR = ir;
    double len = 100; if (len>aR*3) len = aR*3;
    for (double ang0 = -3; ang0 < 3.1; ang0+=0.05)	{//loop over ang 
      for (int sgn = -1; sgn<=1; sgn+=2)    		{//loop over signes of curv
if ((sgn>0) && !(kase&4)) continue;
if ((sgn<0) && !(kase&8)) continue;
	double R = sgn*aR;
	double dang = len/R/nPts;
	double C0 = cos(ang0);
	double S0 = sin(ang0);
        TCircleFitter helx;
	for (int is=0;is<nPts;is++) {	//loop over points
          double ang = ang0 + dang*is;
          double S = sin(ang),C = cos(ang);
          double eR = ran.Gaus(0,RERR)*sgn;
          double shift = aShift[is%5];
//shift=0;//???????????????????
          double SS = sin(ang+shift);
          double CC = cos(ang+shift);
          e[0] = pow(RERR*SS,2);
          e[1] =-pow(RERR   ,2)*CC*SS;
          e[2] = pow(RERR*CC,2);

          x[0] = 100 + (R)*(S-S0);
          x[1] = 200 - (R)*(C-C0);
          ex[0]= x[0]-SS*eR; 
          ex[1]= x[1]+CC*eR; 
          helx.Add (ex[0],ex[1],e);
	}		//end points
	helx.Fit();
if (!(helx.GetCase()&kase)) continue;
	nFit++;
	helx.MakeErrs();
        int iFix = 0;
        if (kase&16) iFix +=1;
        if (kase&32) iFix +=4;
	if (iFix) {
	  double vals[5];
	  TCL::ucopy(x,vals,3);
	  vals[3]=0;
	  vals[4]=1./R;
	  helx.FixAt(vals,iFix);
	}
	x[0] = 100 ;
	x[1] = 200 ;
	double s = helx.Path(x);
        assert(s<0);
	assert(fabs(s) < len);
	helx.Move(s);
	double dd[6],hf[6];
	double dx = helx.Pos()[0]-x[0];
	double dy = helx.Pos()[1]-x[1];
        const TCEmx_t *emx = helx.Emx();
	dd[0] = -dx*S0+dy*C0;
	dd[1] = atan2(helx.Dir()[1],helx.Dir()[0])-ang0;
	if (dd[1]> M_PI) dd[1]-=2*M_PI;
	if (dd[1]<-M_PI) dd[1]+=2*M_PI;
	dd[2] = helx.Rho()-1./R;
        hf[0] = (dd[0]*dd[1])	*1e1/(RERR*RERR);
        hf[1] = (emx->mHA)	*1e1/(RERR*RERR);
        hf[2] = dd[0]*dd[2]	*1e3/(RERR*RERR);
        hf[3] = (emx->mHC)	*1e3/(RERR*RERR);
        hf[4] = dd[1]*dd[2]	*1e4/(RERR*RERR);
        hf[5] = (emx->mAC)	*1e4/(RERR*RERR);

        
	for (int ih=0;ih<6;ih++) { hh[ih]->Fill(hf[ih]);}
    } 		//end sign
  }		//end ang0
  } 		// curv
  myCanvas->Modified();
  myCanvas->Update();
  while(!gSystem->ProcessEvents()){}; 

}
//______________________________________________________________________________
void TCircle::Show(int nPts,const double *Pts,int pstep) 
{
static TCanvas *myCanvas = 0;
static TGraph  *ptGraph  = 0;
static TGraph  *ciGraph  = 0;
  double x[100],y[100];
  if (nPts>100) nPts=100;
  for (int i=0;i<nPts;i++) { x[i]=Pts[i*pstep+0];  y[i]=Pts[i*pstep+1]; }


  if(!myCanvas) myCanvas = new TCanvas("TCircle_Show","",600,800);
  myCanvas->Clear();
  delete ptGraph; delete ciGraph;

  ptGraph  = new TGraph(nPts  , x, y);
  ptGraph->SetMarkerColor(kRed);
  ptGraph->Draw("A*");

  TCircle tc(this);
  double xy[2];
  xy[0] = x[0];
  xy[1] = y[0];
  double s = tc.Path(xy);
  tc.Move(s);
  xy[0] = x[nPts-1];
  xy[1] = y[nPts-1];
  s = tc.Path(xy);
  if (s<0) { tc.Backward(); s = tc.Path(xy);}
  double ds = s/99;
  for (int i=0;i<100;i++) {x[i]=tc.Pos()[0];y[i]=tc.Pos()[1];tc.Move(ds);}
  
  ciGraph  = new TGraph(100  , x, y);
  ciGraph->Draw("Same CP");
  myCanvas->Modified();
  myCanvas->Update();
  while(!gSystem->ProcessEvents()){}; 

}
//______________________________________________________________________________
THelixFitter::THelixFitter():fPoli1Fitter(1)
{
  Clear();
  SetEmx();
}
//______________________________________________________________________________
THelixFitter::~THelixFitter()
{;}
//______________________________________________________________________________
void THelixFitter::Clear(const char*)
{
  fCircleFitter.Clear();
  fPoli1Fitter.Clear();
  fPoli1Fitter.SetCoefs(1);
  fChi2=0;
}
//______________________________________________________________________________
void THelixFitter::Print(const char*) const
{
  THelixTrack::Print();
  fCircleFitter.Print();
  fPoli1Fitter.Print();
}
//______________________________________________________________________________
void THelixFitter::Add (double x,double y,double z) 
{
  fCircleFitter.Add(x,y,z);
}  
//______________________________________________________________________________
void THelixFitter::AddErr(const double *err2xy,double err2z) 
{  
  fCircleFitter.AddErr(err2xy,err2z);
}  
//______________________________________________________________________________
double THelixFitter::Fit()
{
  TCircleFitterAux* myAux= GetAux(0);
  int nDat = Size();
  double Xi2xy = fCircleFitter.Fit();
  if (Xi2xy>1e11) return Xi2xy;
  int ndfXY = fCircleFitter.Ndf();
  TCircle circ(fCircleFitter);
  const double *xy=0;
  xy = &(myAux[nDat-1].x);
  double z1 = xy[2];
  double s1 = circ.Path(xy);

  xy = &(myAux[0].x);
  double z0 = xy[2];
  double s  = circ.Path(xy);
//	estimation of tan(dip) to correct z errs
  double tanDip = (z1-z0)/(s1-s);

  circ.Move(s);
//  set lengths
  const double *dc = circ.Dir();
  for (int iDat=0;iDat<nDat;iDat++) {
    TCircleFitterAux* aux = myAux+iDat;
    xy = &(aux->x);
    double ds = circ.Path(xy,aux->exy);
    circ.Move(ds); s+=ds;
//		correct errors
    double corErr = tanDip*tanDip*
                   (dc[0]*dc[0]*aux->exy[0]
                   +dc[1]*dc[1]*aux->exy[2]
                   +dc[0]*dc[1]*aux->exy[1]*2);
    fPoli1Fitter.Add(s,aux->z,aux->ezz+corErr);
  }
  double Xi2z = fPoli1Fitter.Fit();
//	Now set THelixTrack
  int ndfSz = fPoli1Fitter.Ndf();
  Update(1);
  int ndf = ndfSz+ndfXY;
  fChi2 = Xi2xy*ndfXY+Xi2z*ndfSz;
  if (ndf) fChi2/=ndf;
  return fChi2;
   
}  
//_____________________________________________________________________________
double THelixFitter::FixAt(const double val[5],int flag) 
{
  double xx[3],s;
  memcpy(xx,fX,sizeof(xx));
  int move = (flag&1); 
  if (move) {
    s = fCircleFitter.Path(val);
    fCircleFitter.Move(s);
    fPoli1Fitter.Move(s);
  }
  double Xi2c = fCircleFitter.FixAt(val,flag);
  if (flag&1)   fPoli1Fitter.FixAt(0.,val[2]);
//  Update(1+2);
  if (move) {
    s = fCircleFitter.Path(xx);
    fCircleFitter.Move(s);
    fPoli1Fitter.Move(s);
  }
  Update(1+2);
//  double Xi2c = fCircleFitter.EvalChi2();
  double Xi2z = fPoli1Fitter.Chi2();
  int ndfc = fCircleFitter.Ndf();
  int ndfz = fPoli1Fitter.Ndf();
  
  int ndf = ndfc+ndfz;
  fChi2 = Xi2c*ndfc+Xi2z*ndfz;
  if (ndf) fChi2/=ndf;
  return fChi2;
}
//_____________________________________________________________________________
void THelixFitter::Skip(int idx) 
{
  fCircleFitter.Skip(idx);
  fPoli1Fitter.Skip(idx);
  int ndfc = fCircleFitter.Ndf();
  int ndfz = fPoli1Fitter.Ndf();
  int ndf = ndfc+ndfz;
  fChi2 = fCircleFitter.Chi2()*ndfc+fPoli1Fitter.Chi2()*ndfz;
  if (ndf) fChi2/=ndf;
}
//______________________________________________________________________________
void THelixFitter::Update(int kase)
{
  if(kase&1) {
    const double *pol = fPoli1Fitter.Coe();
    fCosL = 1./sqrt(pol[1]*pol[1]+1);
    double *haslet = fCircleFitter.Pos();
    fX[0] = haslet[0];
    fX[1] = haslet[1];
    fX[2] = pol[0];
    fP[0] = haslet[2]*fCosL;
    fP[1] = haslet[3]*fCosL;
    fP[2] = pol[1]*fCosL;
    fRho  = haslet[4];
  }
  if(kase&2) {
    double emx[3];
    emx[0] = fPoli1Fitter.Emx()[0];
    emx[1] = fPoli1Fitter.Emx()[1]*fCosL*fCosL;
    emx[2] = fPoli1Fitter.Emx()[2]*fCosL*fCosL*fCosL*fCosL;
    fEmx->Set(fCircleFitter.Emx()->Arr(),emx);
  }
}
//______________________________________________________________________________
void THelixFitter::MakeErrs()
{
  fCircleFitter.MakeErrs();
  fPoli1Fitter.MakeErrs();
  Update(2);
}
//______________________________________________________________________________
double THelixFitter::EvalChi2() 
{
  double Xi2c = fCircleFitter.EvalChi2();
  double Xi2z = fPoli1Fitter.EvalChi2();
  fChi2 = Xi2c*fCircleFitter.Ndf()+Xi2z*fPoli1Fitter.Ndf();
  fChi2/=(fCircleFitter.Ndf()+fPoli1Fitter.Ndf()+1e-10);
  return fChi2;
}
//______________________________________________________________________________
void THelixFitter::Test(int kase)
{
// 1=fit case    1 alowed
// 2=fit case    2 alowed
// 4=fit +ive curv alowed
// 8=fit -ive curv alowed
// 16=fix last point 
// 32=fix curvature 
// 64=fix angle (not implemented in test) 
//128=show each track 
  if (!(kase&3 ))kase+=1+2;
  if (!(kase&12))kase+=4+8;
//  enum {nPts=20,nHH=8};
  enum {nPts=5,nHH=8};
  double e[4],x[3],xe[3];    
  double aShift[5];
  aShift[0]=-acos(0.25);
  aShift[1]=-acos(0.50);
  aShift[2]= 0;
  aShift[3]= acos(0.25);
  aShift[5]= acos(0.50);
  double RERR = 0.1;
  double ZERR = 0.1;
TRandom ran;
static TCanvas* myCanvas[9]={0};
static TH1F *hh[nHH]={0};
static const char *hNams[]={"pH","pA","pC","pZ","pD","Xi2","Xi2E","Xi2d",0};
  if(!myCanvas[0])  myCanvas[0]=new TCanvas("THelixFitter_TestC1","",600,800);
  myCanvas[0]->Clear();
  myCanvas[0]->Divide(1,nHH);

  for (int i=0;i<nHH;i++) { 
    double low = (i>=5)? 0:-5;
    double upp = 5;
    delete hh[i]; hh[i]= new TH1F(hNams[i],hNams[i],100,low,upp);
    myCanvas[0]->cd(i+1); hh[i]->Draw();
  }

//		Init Second histo group 
static TH1F *h2h[4]={0,0,0,0};
static const char *h2Nams[]={"targYY","targZZ","targYZ","calcYZ",0};
  int n2h=4;
  if(!myCanvas[1])  myCanvas[1]=new TCanvas("THelixFitter_TestC2","",600,800);
  myCanvas[1]->Clear();
  myCanvas[1]->Divide(1,n2h);
  for (int i=0;i<n2h;i++) { 
    delete h2h[i]; h2h[i]= new TH1F(h2Nams[i],h2Nams[i],100,-5,5);
    myCanvas[1]->cd(i+1); h2h[i]->Draw();
  }
//		End Init Second histo group 

//		Init 3rd histo group 
static TH1F *h3h[4]={0,0,0,0};
static const char *h3Nams[]={"dcaXY","dcaXYNor","dcaZ","dcaZNor",0};
  int n3h=4;
  if(!myCanvas[2])  myCanvas[2]=new TCanvas("THelixFitter_TestC3","",600,800);
  myCanvas[2]->Clear();
  myCanvas[2]->Divide(1,n3h);
  for (int i=0;i<n3h;i++) { 
    delete h3h[i]; h3h[i]= new TH1F(h3Nams[i],h3Nams[i],100,-5,5);
    myCanvas[2]->cd(i+1); h3h[i]->Draw();
  }
//		End Init 3rd histo group 


  double spotSurf[4]= {-100,1,0,0};
  double spotAxis[3][3]= {{0,1,0},{0,0,1},{1,0,0}};


  int nFit = 0;
for (double idip=-1;idip<=1;idip+=0.2){
  double dip = idip;
//  dip = 0;
  double cosDip = cos(dip);
  double sinDip = sin(dip);
  double tanDip = tan(dip); if(tanDip){};
  for (int ir = 50; ir <= 1000; ir +=20) 		{//loop over radius
    double aR = ir;
    double len = 100; if (len>aR*3) len = aR*3;
    for (double ang00 = -3; ang00 < 3.1; ang00+=0.2)	{//loop over ang 
      double ang0 = ang00;
//      ang0 = 0;
      for (int sgn = -1; sgn<=1; sgn+=2)    		{//loop over signes of curv
if(sgn>0 && !(kase&4)) continue; 
if(sgn<0 && !(kase&8)) continue; 

	double R = sgn*aR;
	double dang = len/R/nPts;
	double C0 = cos(ang0);
	double S0 = sin(ang0);
        THelixFitter helx;

        double trakPars[7]={100,200,300,C0*cosDip,S0*cosDip,sinDip,1/R};
        THelixTrack trak(trakPars+0,trakPars+3,trakPars[6]);

	for (int is=0;is<nPts;is++) {	//loop over points
          double ang = ang0 + dang*is;
          double S = sin(ang),C = cos(ang);
          double eR = ran.Gaus(0,RERR)*sgn;
          double eZ = ran.Gaus(0,ZERR);
          double shift = aShift[is%5];
//shift=0;//???????????????????
          double SS = sin(ang+shift);
          double CC = cos(ang+shift);
          e[0] = pow(RERR*SS,2);
          e[1] =-pow(RERR   ,2)*CC*SS;
          e[2] = pow(RERR*CC,2);
          e[3] = pow(ZERR,2);
          x[0] = 100 + (R)*(S-S0);
          x[1] = 200 - (R)*(C-C0);
          double len = (R)*(ang-ang0);
          x[2] = 300 + len*tan(dip);
          xe[0]= x[0]-SS*eR; 
          xe[1]= x[1]+CC*eR; 
          xe[2]= x[2]+eZ; 
          helx.Add (xe[0],xe[1],xe[2]);
          helx.AddErr(e,e[3]);
	}		//end points
	double Xi2 =helx.Fit();
if(!(kase&helx.GetCase())) continue; 

	helx.MakeErrs();
	nFit++;
if (kase&16) Xi2=helx.FixAt(x);


if (kase&32) {
	double vals[5];
	TCL::ucopy(x,vals,3);vals[3]=0;vals[4]=1./R;
	Xi2=helx.FixAt(vals,4);
	}
if (kase&128) helx.Show();
	double Xi2E =helx.EvalChi2();

        trak.Move(0.3*len/cosDip);
	memcpy(x,trak.Pos(),sizeof(x));
        ang0 = atan2(trak.Dir()[1],trak.Dir()[0]);
//	double s = helx.Path(x[0],x[1]);
	double s = helx.Path(x);
//      assert(s<0);
//	assert(fabs(s) < len*1.1);

        double pos[3],dir[3],rho;
	helx.Move(s);
        THEmx_t *emx = helx.Emx();
        helx.Get   (pos,dir,rho);
	double psi = atan2(dir[1],dir[0]);
	double sinPsi = sin(psi);
	double cosPsi = cos(psi);
	double tanPsi = sinPsi/cosPsi; if(tanPsi){};
	double dd[10],hf[10];
	double dx = x[0]-pos[0];
	double dy = x[1]-pos[1];
	dd[0] = -dx*sinPsi+dy*cosPsi;
	hf[0] = dd[0]/sqrt(emx->mHH+1e-20);
	dd[2] = psi-ang0;
	if (dd[2]> M_PI) dd[2]-=2*M_PI;
	if (dd[2]<-M_PI) dd[2]+=2*M_PI;
	hf[1] = dd[2]/sqrt(emx->mAA+1e-20);
	dd[4] = rho-1./R;
	hf[2] = dd[4]/sqrt(emx->mCC+1e-20);
        dd[6] = (helx.Pos()[2]-x[2])/pow(helx.GetCos(),2);
        hf[3] = dd[6]/sqrt(emx->mZZ+1e-20);
        dd[8] = asin(dir[2])-dip;
	if (dd[8]> M_PI) dd[8]-=2*M_PI;
	if (dd[8]<-M_PI) dd[8]+=2*M_PI;
        hf[4] = dd[8]/(sqrt(emx->mLL));
        hf[5] = Xi2;
        hf[6] = Xi2E;
        hf[7] = Xi2E-Xi2+1;
	for (int ih=0;ih<nHH;ih++) { hh[ih]->Fill(hf[ih]);}

//		Fill 2nd histo group
        double xIde[3],pIde[3],xFit[3],pFit[3],eSpot[3],hfil,sIde,sFit;
//        if(fabs(dip)>1) continue;
        int closePoint=0;
        spotSurf[0] = -110;
  
        { spotSurf[0] = -x[0]; closePoint=2006;}
        sIde = trak.Step(200.,spotSurf,4, xIde,pIde,closePoint);
 
        if (fabs(spotSurf[0]+TCL::vdot(xIde,spotSurf+1,3))>0.001) {
          printf("***Wrong point found**\n");
          trak.Print();
          assert(0);
	}
        sFit = helx.Step(200.,spotSurf,4, xFit,pFit,closePoint);
        if (sFit>=1000 ) continue;
        if (fabs(pIde[0]-pFit[0])>0.1) continue;
        helx.Move(sFit);
        emx = helx.Emx();
        helx.GetSpot(spotAxis,eSpot);
        hfil = (xFit[1]-xIde[1]); hfil/= sqrt(eSpot[0]); 
        h2h[0]->Fill(hfil);
        hfil = (xFit[2]-xIde[2]); hfil/= sqrt(eSpot[2]); 
        h2h[1]->Fill(hfil);
        hfil = (xFit[1]-xIde[1])*(xFit[2]-xIde[2]);
        h2h[2]->Fill(hfil*100);
        h2h[3]->Fill(hfil/sqrt(eSpot[0]*eSpot[2]));
//        h2h[3]->Fill(eSpot[1]*100);
//		End 2nd histo group

//		Fill 3rd histo group
        double dcaXY,dcaZ,dcaEmx[3];
        double sDca = helx.Dca(trakPars,dcaXY,dcaZ,dcaEmx);
        if (fabs(sDca)<1000) {
          h3h[0]->Fill(dcaXY);
          h3h[1]->Fill(dcaXY/sqrt(dcaEmx[0]));
          h3h[2]->Fill(dcaZ );
          h3h[3]->Fill(dcaZ /sqrt(dcaEmx[2]));
        }
//		End 3rd histo group

    } 		//end sign
  }		//end ang0
  } 		// curv
}		// dip
  for (int ih=0;myCanvas[ih];ih++) {
    myCanvas[ih]->Modified();
    myCanvas[ih]->Update();
  }
  while(!gSystem->ProcessEvents()){}; 
}
//______________________________________________________________________________
void THelixFitter::Show() const
{
static TCanvas *myCanvas = 0;
static TGraph  *ptGraph[2]  = {0,0};
static TGraph  *ciGraph[2]  = {0,0};
  double  x[100],y[100],z[100],l[100]
        , X[100],Y[100],Z[100];
  int nPts = Size();
  if (nPts>100) nPts=100;
  TCircleFitterAux* aux=GetAux(0);
  THelixTrack tc(this);
  double s = tc.Path(aux[0].x,aux[0].y); tc.Move(s);
  s = tc.Path(aux[nPts-1].x,aux[nPts-1].y);
  if (s<0) { tc.Backward();}
  l[0]=0;
  double ds=0;
  for (int i=0;i<nPts;i++) {
    if (i) {ds = tc.Path(aux[i].x,aux[i].y);tc.Move(ds);l[i]=l[i-1]+ds;}
    x[i]=aux[i].x;   y[i]=aux[i].y;   z[i]=aux[i].z;
    X[i]=tc.Pos()[0];Y[i]=tc.Pos()[1];Z[i]=tc.Pos()[2];
  }


  if(!myCanvas) myCanvas = new TCanvas("THelixFitter_Show","",600,800);
  myCanvas->Clear();
  myCanvas->Divide(1,2);

  delete ptGraph[0]; delete ciGraph[0];
  ptGraph[0]  = new TGraph(nPts  , x, y);
  ptGraph[0]->SetMarkerColor(kRed);
  myCanvas->cd(1); ptGraph[0]->Draw("A*");
  delete ptGraph[1]; delete ciGraph[1];
  ptGraph[1]  = new TGraph(nPts  , l, z);
  ptGraph[1]->SetMarkerColor(kRed);
  myCanvas->cd(2); ptGraph[1]->Draw("A*");
  
  ciGraph[0]  = new TGraph(nPts  , X, Y);
  myCanvas->cd(1); ciGraph[0]->Draw("Same CP");
  ciGraph[1]  = new TGraph(nPts  , l, Z);
  myCanvas->cd(2); ciGraph[1]->Draw("Same CP");

  myCanvas->Modified();
  myCanvas->Update();
  while(!gSystem->ProcessEvents()){}; 

}
//______________________________________________________________________________
/***************************************************************************
 *
 * $Id: THelixTrack.cxx,v 1.47 2010/07/15 18:08:43 perev Exp $
 *
 * Author: Victor Perev, Mar 2006
 * Rewritten Thomas version. Error hangling added
 * Author: Thomas Ullrich, Dec 1999
 ***************************************************************************
 *
 * Description:
 *
 * Fast fitting routine using a iterational linear regression 
 * method (ILRM). Reference: N.Chernov, G.A.Ososkov, Computer  
 * Physics Communication 33 (1984) 329-333.                   
 *
 ***************************************************************************
 *
 * $Log: THelixTrack.cxx,v $
 * Revision 1.47  2010/07/15 18:08:43  perev
 * TestMtx added
 *
 * Revision 1.46  2010/06/01 20:54:54  perev
 * Correlation HZ accounted now
 *
 * Revision 1.45  2010/04/23 22:51:27  perev
 * Method Move with derivatives adde
 *
 * Revision 1.44  2009/11/09 19:58:58  perev
 * FitZ removed everywhere
 *
 * Revision 1.43  2009/09/07 04:32:50  fine
 * workaround for the bug #1628
 *
 * Revision 1.42  2009/08/28 16:38:55  fine
 * fix the compilation issues under SL5_64_bits  gcc 4.3.2
 *
 * Revision 1.41  2009/08/24 23:40:33  perev
 * operator=() added
 *
 * Revision 1.40  2009/08/22 00:11:59  perev
 * Full error matrix + derivatives matrix
 *
 * Revision 1.39  2009/07/18 00:12:56  perev
 * method PatX(helx,,,) added
 *
 * Revision 1.38  2009/07/01 21:48:39  perev
 * Fix -tive errors & remove obsolete
 *
 * Revision 1.37  2009/04/06 17:51:32  perev
 * Replace assert(wt>0) by error condition
 *
 * Revision 1.36  2008/10/29 19:36:25  perev
 * flag 2d and 3d dca added
 *
 * Revision 1.35  2007/12/20 00:47:27  perev
 * WarnOff
 *
 * Revision 1.34  2007/12/18 23:11:05  perev
 * Distance to helix & circle added
 *
 * Revision 1.33  2007/10/24 22:43:24  perev
 * Implementation was forgotten. Thanx Adam
 *
 * Revision 1.32  2007/09/10 02:05:37  perev
 * Misstype fixed
 *
 * Revision 1.31  2007/07/13 18:17:10  perev
 * remove member fMax from THelixTrack
 *
 * Revision 1.30  2007/07/12 00:22:29  perev
 * TCircleFitter::Fit case 1 if case 2 failed
 *
 * Revision 1.29  2007/06/25 19:26:40  perev
 * Cleanup
 *
 * Revision 1.28  2007/04/26 04:20:18  perev
 * Some improvements
 *
 * Revision 1.27  2007/03/21 17:41:32  fisyak
 * replace complex by TComplex
 *
 * Revision 1.26  2007/01/26 19:56:24  perev
 * tune up
 *
 * Revision 1.25  2006/08/10 04:09:50  perev
 * Test cleanup
 *
 * Revision 1.23  2006/06/28 18:39:07  perev
 * cos(dip)**4 added to Dca(...) to account z err in the nearest point
 *
 * Revision 1.22  2006/06/26 19:09:21  perev
 * DcaXY & DcaZ with errors added
 *
 * Revision 1.21  2006/06/09 19:53:51  perev
 * double Dca(double x,double y,double *dcaErr=0) added
 *
 * Revision 1.2  2003/09/02 17:59:34  perev
 * gcc 3.2 updates + WarnOff
 *
 * Revision 1.1  1999/12/21 16:28:48  ullrich
 * Initial Revision
 *
 **************************************************************************/
 
