/**
 * \class StHelix
 * \author Thomas Ullrich, Sep 26 1997
 * 
 * Parametrization of a helix. Can also cope with straight tracks, i.e.
 * with zero curvature. This represents only the mathematical model of 
 * a helix. See the SCL user guide for more. 
 */
/***************************************************************************
 *
 * $Id: StHelix.hh,v 1.9 2004/12/02 02:51:16 ullrich Exp $
 *
 * Author: Thomas Ullrich, Sep 1997
 ***************************************************************************
 *
 * Description: Parametrization of a helix
 * 
 ***************************************************************************
 *
 * $Log: StHelix.hh,v $
 * Revision 1.9  2004/12/02 02:51:16  ullrich
 * Added option to pathLenghth() and distance() to search for
 * DCA only within one period. Default stays as it was.
 *
 * Revision 1.8  2003/10/30 20:06:46  perev
 * Check of quality added
 *
 * Revision 1.7  2002/06/21 17:49:25  genevb
 * Some minor speed improvements
 *
 * Revision 1.6  2002/04/24 02:41:55  ullrich
 * Restored old format.
 *
 **************************************************************************/

#ifndef ST_HELIX_HH
#define ST_HELIX_HH

#include <math.h>
#include <utility>
#include <algorithm>
#include "StThreeVector.hh"
#if !defined(ST_NO_NAMESPACES)
using std::pair;
using std::swap;
using std::max;
#endif

class StHelix {
public:
    /// curvature, dip angle, phase, origin, h
    StHelix(double c, double dip, double phase,
	    const StThreeVector<double>& o, int h=-1);
    
    virtual ~StHelix();
    // StHelix(const StHelix&);			// use default
    // StHelix& operator=(const StHelix&);	// use default

    double       dipAngle()   const;           
    double       curvature()  const;	/// 1/R in xy-plane
    double       phase()      const;	/// aziumth in xy-plane measured from ring center
    double       xcenter()    const;	/// x-center of circle in xy-plane
    double       ycenter()    const;	/// y-center of circle in xy-plane
    int          h()          const;	/// -sign(q*B);
    
    const StThreeVector<double>& origin() const;	/// starting point

    void setParameters(double c, double dip, double phase, const StThreeVector<double>& o, int h);
    
    double       x(double s)  const;
    double       y(double s)  const;
    double       z(double s)  const;

    StThreeVector<double>  at(double s) const;

    /// returns period length of helix
    double       period()       const;
    
    /// path length at given r (cylindrical r)
    pair<double, double> pathLength(double r)   const;
    
    /// path length at given r (cylindrical r, cylinder axis at x,y)
    pair<double, double> pathLength(double r, double x, double y);
    
    /// path length at distance of closest approach to a given point
    double       pathLength(const StThreeVector<double>& p, bool scanPeriods = true) const;
    
    /// path length at intersection with plane
    double       pathLength(const StThreeVector<double>& r,
			    const StThreeVector<double>& n) const;

    /// path length at distance of closest approach in the xy-plane to a given point
    double       pathLength(double x, double y) const;

    /// path lengths at dca between two helices 
    pair<double, double> pathLengths(const StHelix&) const;
    
    /// minimal distance between point and helix
    double       distance(const StThreeVector<double>& p, bool scanPeriods = true) const;    
    
    /// checks for valid parametrization
    bool         valid(double world = 1.e+5) const;
    
    /// move the origin along the helix to s which becomes then s=0
    virtual void moveOrigin(double s);
    
protected:
    StHelix();
    
    void setCurvature(double);	/// performs also various checks   
    void setPhase(double);	        
    void setDipAngle(double);
    
    /// value of S where distance in x-y plane is minimal
    double fudgePathLength(const StThreeVector<double>&) const;
    
protected:
    bool                   mSingularity;	// true for straight line case (B=0)
    StThreeVector<double>  mOrigin;
    double                 mDipAngle;
    double                 mCurvature;
    double                 mPhase;
    int                    mH;			// -sign(q*B);

    double                 mCosDipAngle;
    double                 mSinDipAngle;
    double                 mCosPhase;
    double                 mSinPhase;
};

//
//     Non-member functions
//
int operator== (const StHelix&, const StHelix&);
int operator!= (const StHelix&, const StHelix&);
ostream& operator<<(ostream&, const StHelix&);

//
//     Inline functions
//
inline int StHelix::h() const {return mH;}

inline double StHelix::dipAngle() const {return mDipAngle;}

inline double StHelix::curvature() const {return mCurvature;}

inline double StHelix::phase() const {return mPhase;}

inline double StHelix::x(double s) const
{
    if (mSingularity)
	return mOrigin.x() - s*mCosDipAngle*mSinPhase;
    else
	return mOrigin.x() + (cos(mPhase + s*mH*mCurvature*mCosDipAngle)-mCosPhase)/mCurvature;
}
 
inline double StHelix::y(double s) const
{
    if (mSingularity)
	return mOrigin.y() + s*mCosDipAngle*mCosPhase;
    else
	return mOrigin.y() + (sin(mPhase + s*mH*mCurvature*mCosDipAngle)-mSinPhase)/mCurvature;
}

inline double StHelix::z(double s) const
{
    return mOrigin.z() + s*mSinDipAngle;
}

inline const StThreeVector<double>& StHelix::origin() const {return mOrigin;}

inline StThreeVector<double> StHelix::at(double s) const
{
    return StThreeVector<double>(x(s), y(s), z(s));
}

inline double StHelix::pathLength(double x, double y) const
{
    return fudgePathLength(StThreeVector<double>(x, y, 0));
}

#endif
