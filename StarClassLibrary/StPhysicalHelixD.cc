/***************************************************************************
 *
 * $Id: StPhysicalHelixD.cc,v 1.5 2002/06/21 17:49:26 genevb Exp $
 *
 * Author: Thomas Ullrich, Jan 1999
 ***************************************************************************
 *
 * Description:
 *
 * Remarks:   This is a 'handmade' specialisation of StPhysicalHelix
 *            with StThreeVector<T> replaced by StThreeVectorD
 *            and pair<T, T> replaced by pairD.
 *            This code contains no templates.
 *
 ***************************************************************************
 *
 * $Log: StPhysicalHelixD.cc,v $
 * Revision 1.5  2002/06/21 17:49:26  genevb
 * Some minor speed improvements
 *
 * Revision 1.4  2002/02/20 00:56:31  ullrich
 * Added methods to calculate signed DCA.
 *
 * Revision 1.3  1999/02/24 11:43:12  ullrich
 * Fixed bug in momentum().
 *
 * Revision 1.2  1999/02/17 11:08:03  ullrich
 * Fix bug in momentum calculation.
 *
 * Revision 1.1  1999/01/30 03:59:05  fisyak
 * Root Version of StarClassLibrary
 *
 * Revision 1.1  1999/01/23 00:29:22  ullrich
 * Initial Revision
 *
 **************************************************************************/
#include <math.h>
#include "StPhysicalHelixD.hh"
#include "PhysicalConstants.h" 
#include "SystemOfUnits.h"

#ifdef __ROOT__
ClassImp(StPhysicalHelixD)
#endif

StPhysicalHelixD::StPhysicalHelixD() { /* nop */}

StPhysicalHelixD::~StPhysicalHelixD() { /* nop */ }

StPhysicalHelixD::StPhysicalHelixD(const StThreeVectorD& p,
				   const StThreeVectorD& o,
				   double B, double q)
{
    mH = (q*B <= 0) ? 1 : -1;
    if(p.y() == 0 && p.x() == 0)
	setPhase((M_PI/4)*(1-2.*mH));
    else
	setPhase(atan2(p.y(),p.x())-mH*M_PI/2);
    setDipAngle(atan2(p.z(),p.perp()));
    mOrigin = o;
    
#ifndef ST_NO_NAMESPACES
    {
	using namespace units;
#endif
	setCurvature(fabs((c_light*nanosecond/meter*q*B/tesla)/
			  (abs(p)/GeV*mCosDipAngle)/meter));   
#ifndef ST_NO_NAMESPACES
    }
#endif
}

StPhysicalHelixD::StPhysicalHelixD(double c, double d, double phase,
				   const StThreeVectorD& o, int h)
    : StHelixD(c, d, phase, o, h) { /* nop */}


StThreeVectorD StPhysicalHelixD::momentum(double B) const
{
    if(mSingularity)
	return(StThreeVectorD(0,0,0));
    else {
#ifndef ST_NO_NAMESPACES
	{
	    using namespace units;
#endif
	    double pt =	GeV*fabs(c_light*nanosecond/meter*B/tesla)/(fabs(mCurvature)*meter);
	    
	    return (StThreeVectorD(pt*cos(mPhase+mH*M_PI/2),   // pos part pos field
				   pt*sin(mPhase+mH*M_PI/2),
				   pt*tan(mDipAngle)));
#ifndef ST_NO_NAMESPACES
	}
#endif
    }
}

StThreeVectorD StPhysicalHelixD::momentumAt(double S, double B) const
{
    // Obtain phase-shifted momentum from phase-shift of origin
    double xc = this->xcenter();
    double yc = this->ycenter();
    double rx = (y(S)-yc)/(mOrigin.y()-yc);
    double ry = (x(S)-xc)/(mOrigin.x()-xc);
    return (this->momentum(B)).pseudoProduct(rx,ry,1.0);
}

int StPhysicalHelixD::charge(double B) const
{
    return (B > 0 ? -mH : mH);
}

double StPhysicalHelixD::geometricSignedDistance(double x, double y) 
{
    // Geometric signed distance
    double thePath = this->pathLength(x,y);
    StThreeVectorD DCA2dPosition = this->at(thePath);
    DCA2dPosition.setZ(0);
    StThreeVectorD position(x,y,0);
    StThreeVectorD DCAVec = (DCA2dPosition-position);
    StThreeVectorD momVec;
    // Deal with straight tracks
    if (this->mSingularity) {
	momVec = this->at(1)- this->at(0);
	momVec.setZ(0);
    }
    else {
	momVec = this->momentumAt(thePath,1./tesla); // Don't care about Bmag.  Helicity is what matters.
	momVec.setZ(0);
	
    }
    
    double cross = DCAVec.x()*momVec.y() - DCAVec.y()*momVec.x();
    double theSign = (cross>=0) ? 1. : -1.;
    return theSign*DCAVec.perp();
}

double StPhysicalHelixD::curvatureSignedDistance(double x, double y)
{
    // Protect against helicity 0 or zero field
    if (this->mSingularity || fabs(this->mH)<=static_cast<double>(0)) {
	return (this->geometricSignedDistance(x,y));
    }
    else {
	return (this->geometricSignedDistance(x,y))/(this->mH);
    }
    
}

double StPhysicalHelixD::geometricSignedDistance(const StThreeVectorD& pos)
{
    double sdca2d = this->geometricSignedDistance(pos.x(),pos.y());
    double theSign = (sdca2d>=0) ? 1. : -1.;
    return (this->distance(pos))*theSign;
}

double StPhysicalHelixD::curvatureSignedDistance(const StThreeVectorD& pos)
{
    double sdca2d = this->curvatureSignedDistance(pos.x(),pos.y());
    double theSign = (sdca2d>=0) ? 1. : -1.;
    return (this->distance(pos))*theSign;
}
