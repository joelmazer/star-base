#include "StIOInterFace.h"
//_____________________________________________________________________________
ClassImp(StIOInterFace)

//_____________________________________________________________________________
StIOInterFace::StIOInterFace(const char *name,const char *iomode)
:StMaker(name)
{
  if (iomode) SetIOMode(iomode);
  fNIO = 0;
}  
//_____________________________________________________________________________
void StIOInterFace::SetBranch(const Char_t *brName,const Char_t *file,const Char_t *mode,Option_t *opt)
{
  TString ts("SetBranch:");

  if (file) { ts += " file="; ts += file;}
  if (mode) { ts += " mode="; ts += mode;}
  if (opt ) { ts += "  opt="; ts += opt ;}
  IntoBranch(brName,ts);  
}
//_____________________________________________________________________________
void StIOInterFace::IntoBranch(const Char_t *brName,const Char_t *logNames)
{
 AddAlias(brName,logNames,".branches");  
}
//_____________________________________________________________________________
//void StIOInterFace::Streamer(TBuffer &b) {};
//_____________________________________________________________________________
Int_t StIOInterFace::Skip(int nskip)
{  
  for (int skp=0; skp<nskip; skp++) 
  {
    Clear(); int ret = MakeRead(); 
    if (ret) return ret;
  }
  return kStOK;
}
//_____________________________________________________________________________
Int_t StIOInterFace::Finish()
{
  printf("<%s::Finish> %s: %d I/O's\n",ClassName(),GetName(),fNIO);
  return StMaker::Finish();
}
