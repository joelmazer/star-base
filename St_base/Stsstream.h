/*!
 * \file Stsstream.h
 *
 * \author Victor Perev  8/22/2003
 *  Wrapper for old <strstream>
 *                                          
 *                                                                      
 */
#ifndef STSSTREAM_H
#define STSSTREAM_H
#include "Rstrstream.h"
#ifdef R__SSTREAM
using std::streamsize;
class ostrstream : public std::ostringstream {
public:
const char *str()         {return std::ostringstream::str().c_str();}
int        pcount() const {return std::ostringstream::str().size() ;}
void freeze(bool) const{;};
};
class istrstream : public std::istringstream {
public:
istrstream(): std::istringstream(){}
istrstream(const char *init):std::istringstream(std::string(init)){}
};
#else
#endif 

#endif 
