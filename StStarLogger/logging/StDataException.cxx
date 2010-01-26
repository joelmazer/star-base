/*
 * @file StDataException.cpp
 * @author Stephen Tramer
 *
 * @(#)cpp/api:$Id: StDataException.cxx,v 1.1 2010/01/26 23:43:44 fine Exp $
 *
 * See StDataException.h for detals.
 */

#include <string>
 
#include "StDataException.h"

StDataException::StDataException( const std::string& description,
				  ObjectType object,
				  StUCMException::Severity severity)
  : StUCMException(severity),
    object_(object) {
  setDescription("[DATA:" + typeToString() + "]" + description);
}


StDataException::~StDataException()
{} //EMPTY


StDataException::ObjectType StDataException::getObjectType() const {
  return object_;
}
								   

std::string StDataException::typeToString() const {
  switch (object_) {
    case STORE:
      return "STORE";
      break;
    case COLLECTION:
      return "COLLECTION";
      break;
    case RECORD:
      return "RECORD";
      break;
    case FIELD:
      return "FIELD";
      break;
    default:
      return "UNKNOWN";
  }
}
