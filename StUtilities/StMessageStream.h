/*!
  \author G. Van Buren, BNL

  Determination of which out/err streams to use

*/

#ifndef StMessageStream
#define StMessageStream

#include <Stiostream.h>

static ostream& myout(cout);
static ostream& myerr(cerr);

// 64 character string
static const char* ch64 =
"1234567 10 234567 20 234567 30 234567 40 234567 50 234567 60 234";
static const char* ch6a = ch64; // avoids "unused" warning

#endif

// $Id: StMessageStream.h,v 1.2 2003/10/01 20:06:50 genevb Exp $
// $Log: StMessageStream.h,v $
// Revision 1.2  2003/10/01 20:06:50  genevb
// Initialize and test ostrstream buffer sizes (support for gcc before 3.2)
//
// Revision 1.1  2003/09/25 21:19:22  genevb
// Some new cout-like functions and friend functions, some doxygen-ization
//
//