#include "StMessMgr.h"
#include <string.h>

#ifdef __ROOT__
ClassImp(StMessMgr)
#endif

StMessMgr* gMessMgr = 0;
StMessage* endm     = 0;
StMessage* gMessage = 0;

//______________________________________________________________________________
StMessMgr::StMessMgr() : ostrstream() {}



//
// C and Fortran routines:
//________________________________________
static const char defaultMessType = 'I';
static char emptyString[] = "";
static char oOpt[] = "O";
static char otsOpt[] = "OTS";
static char eOpt[] = "E";
static char nullMess[] = "Null message!!!";
#ifdef __linux__
static int sMessLength;
#endif

void type_of_call Message_(const char* mess, int* lines, int*, size_t len) {
  static char space = ' ';
  static const char* messReturnChar = "\n";
  size_t messSize = strlen(mess);
  char* mess2=const_cast<char*> (mess);
  int del_mess = 0;
  if (*lines>1) {
    char* mess1 = const_cast<char*> (mess);
    mess2 = new char[messSize];          // Build a new version of the
    *mess2 = 0;                          // message with trailing spaces
    for (int i=(*lines); i>0; i--) {     // removed, and \n's inserted.
      int clen = len;                    // Now trusting line length argument.
      while (mess1[--clen] == space) {}
      strncat(mess2,mess1,(++clen));
      if (i>1) {
        strcat(mess2,messReturnChar);
        mess1 = &(mess1[len]);
      }
    }
    strcat(mess2,emptyString);
    del_mess = 1;
  } else {
#ifdef __linux__
    sMessLength = len;
#endif
    if ((len>1) && (messSize > len)) {
      mess2 = new char[(len+1)];
      strncpy(mess2,mess,len);
      mess2[len]=0;
      del_mess = 1;
    }
  }
  gMessMgr->Message(mess2);
  if (del_mess) delete [] mess2;
}
//________________________________________
void type_of_call Msg_Enable_(const char* mess, size_t len) {
  size_t messlen = strlen(mess);
  if ((len>1) && (messlen > len)) {
    char* mess2 = new char[(len+1)];
    strncpy(mess2,mess,len);
    mess2[len] = 0;
    gMessMgr->SwitchOn(mess2);
    delete [] mess2;
  } else {
    gMessMgr->SwitchOn(mess);
  }
}
//________________________________________
int type_of_call Msg_Enabled_(const char* mess, int*, size_t len) {
  size_t messlen = strlen(mess);
  int ret_val = 1;
  if ((len>1) && (messlen > len)) {
    char* mess2 = new char[(len+1)];
    strncpy(mess2,mess,len);
    mess2[len] = 0;
    if ((gMessMgr->GetLimit(mess2))==0) ret_val = 0;
    delete [] mess2;
  } else {
    if ((gMessMgr->GetLimit(mess))==0) ret_val = 0;
  }
  return ret_val;
}
//________________________________________
void type_of_call Msg_Disable_(const char* mess, size_t len) {
  size_t messlen = strlen(mess);
  if ((len>1) && (messlen > len)) {
    char* mess2 = new char[(len+1)];
    strncpy(mess2,mess,len);
    mess2[len] = 0;
    gMessMgr->SwitchOff(mess2);
    delete [] mess2;
  } else {
    gMessMgr->SwitchOff(mess);
  }
}
//________________________________________
void type_of_call MessageOut( const char *msg ) {
  gMessMgr->Message(msg);
}
//________________________________________
void type_of_call StCaller(const char* mess, const char* typString,
                           const char* opt, size_t len) {
#ifdef __linux__
  sMessLength = len;
#endif
  if (mess[0]==0) {
    gMessMgr->Message(nullMess,"E",eOpt);
    return;
  }

  size_t messlen = strlen(mess);
  if ((len>1) && (messlen > len)) messlen = len;
  char* mess2 = new char[(messlen+1)];
  strncpy(mess2,mess,messlen);
  mess2[messlen] = 0;
  
  gMessMgr->Message(mess2,typString,opt);
  delete [] mess2;
}
//________________________________________
void type_of_call StCallerOpt(const char* mess, const char* typString,
                              const char* opt, size_t len1, size_t len2,
                              char* optString) {
  char* opt2=const_cast<char*> (opt);
  int del_opt=0;

  if (len2<=0) {
    opt2=optString;
  } else if (strlen(opt) > len2) {
    opt2 = new char[(len2+1)];
    strncpy(opt2,opt,len2);
    opt2[len2]=0;
    del_opt = 1;
  }

  StCaller(mess,typString,opt2,len1);
  if (del_opt) delete [] opt2;
}
//________________________________________
void type_of_call StMessage_(const char* mess, const char* type,
                             const char* opt, size_t len1,
			     size_t len2, size_t len3) {
  char* type2=const_cast<char*> (type);
  int del_type=0;

  if (len2<=0) {
    type2=emptyString;
  } else if (strlen(type) > len2) {
    type2 = new char[(len2+1)];
    strncpy(type2,type,len2);
    type2[len2]=0;
    del_type = 1;
  }

  StCallerOpt(mess,type2,opt,len1,len3,oOpt);
  if (del_type) delete [] type2;
}
//________________________________________
void type_of_call StInfo_(const char* mess, size_t len) {
  StCaller(mess,"I",oOpt,len);
}
//________________________________________
void type_of_call StWarning_(const char* mess, size_t len) {
  StCaller(mess,"W",eOpt,len);
}
//________________________________________
void type_of_call StError_(const char* mess, size_t len) {
  StCaller(mess,"E",eOpt,len);
}
//________________________________________
void type_of_call StDebug_(const char* mess, size_t len) {
  StCaller(mess,"D",oOpt,len);
}
//________________________________________
void type_of_call QAInfo_(const char* mess, size_t len) {
  StCaller(mess,"Q",otsOpt,len);
}
//________________________________________
void type_of_call StInfoOpt_(const char* mess, const char* opt,
                             size_t len1, size_t len2) {
  StCallerOpt(mess,"I",opt,len1,len2,oOpt);
}
//________________________________________
void type_of_call StWarningOpt_(const char* mess, const char* opt,
                                size_t len1, size_t len2) {
  StCallerOpt(mess,"W",opt,len1,len2,eOpt);
}
//________________________________________
void type_of_call StErrorOpt_(const char* mess, const char* opt,
                              size_t len1, size_t len2) {
  StCallerOpt(mess,"E",opt,len1,len2,eOpt);
}
//________________________________________
void type_of_call StDebugOpt_(const char* mess, const char* opt,
                              size_t len1, size_t len2) {
  StCallerOpt(mess,"D",opt,len1,len2,oOpt);
}
//________________________________________
void type_of_call QAInfoOpt_(const char* mess, const char* opt,
                             size_t len1, size_t len2) {
  StCallerOpt(mess,"Q",opt,len1,len2,otsOpt);
}
//________________________________________
void type_of_call StMessAddType_(const char* type, const char* text,
                                 size_t len1, size_t len2) {
  if (strlen(type) > len1) (const_cast<char*> (type))[len1] = 0;
  if (strlen(text) > len2) (const_cast<char*> (text))[len2] = 0;
  gMessMgr->AddType(type,text);
}

//_____________________________________________________________________________
// $Id: StMessMgr.cxx,v 1.2 2004/05/03 20:51:54 fine Exp $