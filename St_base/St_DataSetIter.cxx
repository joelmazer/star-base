//*CMZ :          12/07/98  18.27.27  by  Valery Fine(fine@mail.cern.ch)
//*-- Author :    Valery Fine(fine@mail.cern.ch)   03/07/98
// Copyright (C) Valery Fine (Valeri Faine) 1998. All right reserved 
//*KEEP,TDataset,T=C++.
#include <iostream.h>
#include <iomanip.h>

#include "St_DataSetIter.h"
#include "St_DataSet.h"
//*KEEP,TBrowser.
#include "TBrowser.h"
//*KEND.
#include "TSystem.h"

#ifndef WIN32
# ifndef HASSTRCASE
#  define HASSTRCASE
# endif
#endif

#ifndef HASSTRCASE
#  define strcasecmp(arg1,arg2) stricmp(arg1,arg2)
#endif

ClassImp(St_DataSetIter)
 
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// St_DataSetIter                                                       //
//                                                                      //
// St_DataSetIter is a class iterator to navigate St_DataSet objects    //
// via 3 internal pointers :                                            //
//                                                                      //
//  1. fRootDataSet    - "root" dataset                                 //
//  2. fWorkingDataSet - Working dataset                                //
//  3. fNext           - TIter for the the list of the "root" dataset   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
 
//______________________________________________________________________________
 St_DataSetIter::St_DataSetIter(St_DataSet *link, Bool_t dir)
 {
  fWorkingDataSet= fRootDataSet   =link;
  fMaxDepth      = fDepth         =1;
  fDataSet=0;
  fNext = link ? new TIter(link->GetList() ,dir):0;
 }
 
//______________________________________________________________________________
 St_DataSetIter::St_DataSetIter(St_DataSet *link, Int_t depth, Bool_t dir)
{ 
  fRootDataSet = fWorkingDataSet = link; 
  fMaxDepth    = depth;
  fDepth       = 1;
  fDataSet     = 0; 
  fNext = (link)? new TIter(link->GetList() ,dir):0;

  // Create a DataSet iterator to pass all nodes of the 
  //     "depth"  levels
  //  of  St_DataSet *link  

  if (fMaxDepth != 1) {
     fNextSet[fDepth-1]= fNext;
     if (fMaxDepth > 100) fMaxDepth = 100;
  }
}

//______________________________________________________________________________
St_DataSetIter::~St_DataSetIter()
{
  if (fMaxDepth != 1) {
   for (int i = fDepth-1;i>=0;i--) {
     TIter *s = fNextSet[i];
     if (s) delete s;
   }
  }
  else 
     SafeDelete(fNext);
  fDepth = 1;
}
//______________________________________________________________________________
St_DataSet *St_DataSetIter::Add(St_DataSet *set, St_DataSet *dataset)
{
 ///////////////////////////////////////////////////////////////////////////////
 //                                                                           //
 // Add - adds the set to the dataset defined with the second parameters      //
 //                                                                           //
 // St_DataSet dataset != 0 - Add the set to the St_DataSet *dataset          //
 //                                                                           //
 //                     = 0 - (by default) to the current St_DataSet defined  //
 //                          with fWorkingDataSet data member                 //
 //                                                                           //
 //  returns  the pointer to set is success or ZERO poiner                    //
 //  =======                                                                  //
 //                                                                           //
 //  Note: If this St_DataSetIter is empty (i.e. Cwd() returns 0), the "set"  //
 //        becomes the "root" dataset of this iterator                        //                                                                         //
 ///////////////////////////////////////////////////////////////////////////////
 
  if (!set) return 0;
  St_DataSet *s =  dataset;
  if (!s) s = Cwd();
  if (s) {
     s->Add(set);
     s = set;
  }
  else {
  //  make the coming dataset the current one for the iterator
     s = set;
     fRootDataSet    = s;
     fWorkingDataSet = s;
     if (fNext) {
       Error("Add","St_DataSetIter has been corrupted ;-!"); 
       delete fNext; 
       fNext = 0;
     }
     fNext = new TIter(s->GetList() );
  }
  return s;
}
 
//______________________________________________________________________________
St_DataSet *St_DataSetIter::Add(St_DataSet *dataset, const Char_t *path)
{
 ///////////////////////////////////////////////////////////////////////////////
 //                                                                           //
 // Add                                                                       //
 //                                                                           //
 // Char_t path != 0 - Add a St_DataSet dataset to the St_DataSet dataset     //
 //                    defined with "path"                                    //
 //              = 0 - (by default) to the current St_DataSet defined         //
 //                     with fWorkingDataSet data member                      //
 //                                                                           //
 //  returns the dataset is success or ZERO pointer                           //
 //  =======                                                                  //
 //                                                                           //
 ///////////////////////////////////////////////////////////////////////////////
 if (!dataset) return 0;
 St_DataSet *set = 0;
 if (path && strlen(path)) set = Find(path);
 return Add(dataset,set);
}
 
//______________________________________________________________________________
St_DataSet *St_DataSetIter::Cd(const Char_t *dirname){
/////////////////////////////////////////////////////////////////////
//                                                                 //
// St_DataSet *St_DataSetIter::Cd(const Char_t *dirname)           //
//                                                                 //
// Change the current working directory to dirname                 //
//                                                                 //
// Returns the pointer to the new "working" St_DataSet             //
// =======   0,  if the new directory doesn't exist.               //
//                                                                 //
// Remark:  The name = ".." has a special meaning.                 //
// ------   St_DataSetIter::Cd("..") returns the parent set        //
//          But one still can not use ".." as a legal part         // 
//          of the full path                                       //
/////////////////////////////////////////////////////////////////////
  St_DataSet *set = 0;
  if (strcmp(dirname,"..")) 
    set =  Find(dirname);
  else
    set = fWorkingDataSet->GetParent();
  if (set) fWorkingDataSet = set;
  return set;
}
 
//______________________________________________________________________________
St_DataSet *St_DataSetIter::Dir(Char_t *dirname)
{
//
// Print the names of the St_DataSet objects for the datatset named with "dirname"
// apart of St_DataSet::Ls()  this method prints one level only
//
  St_DataSet *set = (St_DataSet *)fWorkingDataSet;
  if (dirname) set = Find(dirname);
  if (set) set->ls();
  return set;
}

//______________________________________________________________________________
Int_t St_DataSetIter::Du() const {
 // summarize dataset usage by Herb Ward proposal 
  if (!fWorkingDataSet) return 0;
  St_DataSetIter next(fWorkingDataSet,0); 
  St_DataSet *nextset = 0;
  Int_t count = 0;
  while((nextset = (count) ? next():fWorkingDataSet)) {
      count++;
      if (nextset->IsFolder()) cout << endl;
      TString path = nextset->Path();
      cout << path << setw(TMath::Max(Int_t(40-strlen(path.Data())),Int_t(0))) << "...";
      const Char_t *type = nextset->IsFolder() ? "directory" : "table" ;
      cout << setw(10) << type;
      cout << endl;   
  }
  return count;
} 
//______________________________________________________________________________
St_DataSet *St_DataSetIter::FindObject(const Char_t *name,const Char_t *path,Option_t *opt)
{
  //
  // FindObject looks for the object with the name supplied across dataset.
  //
  // name        - the "base" name (with no path) of the St_DataSet
  // path        - path to start the search from (the current dataset "by default")
  // opt = "-i"  - case insensitive search
  //
  // Note: If the name provided is not unique 
  //       the first found is returned.
  //

  if (!name || strlen(name) == 0) return 0;
  if (strchr(name,'/')) {
    Error("FindObject","The name of the object <%s> can not contain any \"/\"",name);
    return 0;
  }
  
  Bool_t opti = opt ? strcasecmp(opt,"-i") == 0 : kFALSE;

  St_DataSet *startset = 0;
  if (path && strlen(path)) startset = Find(path);
  else                      startset = fWorkingDataSet;
  if (!startset) return 0;

  St_DataSetIter next(startset,100);
  St_DataSet *set = 0;
  while ((set = next())){
     if (opti) 
        if (strcasecmp(set->GetName(),name) == 0 ) 
           break;
     else 
        if (set->IsThisDir(name))
           break;
  }
  return set;
}

//______________________________________________________________________________
St_DataSet *St_DataSetIter::FindObject(St_DataSet *set,const Char_t *path,Option_t *opt)
{
  //
  // Check whether the object does belong the St_DataSet defined with "path"
  // opt = "-l"  - check the "reference" links only
  //       "-s"  - check the "structural" links only
  //             = "by default" - checks all links
  //
  if (!set) return 0;
//  Bool_t optl = opt ? strcmp(opt,"-l") == 0 || strcmp(opt,"-L") == 0 : kFALSE;
//  Bool_t opts = opt ? strcmp(opt,"-s") == 0 || strcmp(opt,"-S") == 0 : kFALSE;

  St_DataSet *startset = 0;
  if (path) startset = Find(path);
  else      startset = fWorkingDataSet;
  if (!startset) return 0;

  St_DataSetIter next(startset);
  St_DataSet *nextSet = 0;
  while ( (nextSet = next()) ) 
        if (set == nextSet) break;

  return nextSet;
}

//______________________________________________________________________________
Int_t St_DataSetIter::Flag(const Char_t *path,UInt_t flag,EBitOpt reset)
{
  St_DataSet *set = Find(path);
  if (set) set->SetBit(flag,reset);
  return 0;
}
//______________________________________________________________________________
Int_t St_DataSetIter::Flag(St_DataSet *dataset,UInt_t flag,EBitOpt reset)
{
  if (dataset) dataset->SetBit(flag,reset);
  return 0;
}

//______________________________________________________________________________
St_DataSet *St_DataSetIter::Ls(const Char_t *dirname,Option_t *opt) {
//
//   Ls(const Char_t *dirname,Option_t)
//
//   Prints the list of the St_DataSet defined with dirname
//
//   dirname     = 0   - prints the current dataset
//   dirname[0]  = '/' - print St_DataSet defined with dirname
//   dirname[0] != '/' - prints DataSet with respect of the current class
//
 
  St_DataSet *set= 0;
  if (dirname && strlen(dirname)) set = Find(dirname);
  if (!set && dirname==0) set=Cwd();
  if (set) set->ls(opt);
  return set;
}
 
//______________________________________________________________________________
St_DataSet *St_DataSetIter::Ls(const Char_t *dirname,Int_t depth) {
//
//   Ls(const Char_t *dirname,Int_t depth)
//
//   Prints the list of the St_DataSet defined with dirname
//   Returns the dataset defined by "path" or Cwd();
//
//   dirname     = 0   - prints the current dataset
//   dirname[0]  = '/' - print St_DataSet defined with dirname
//   dirname[0] != '/' - prints DataSet with respect of the current class
//
//   depth       = 0   - print all level of the St_DataSet defined with dirname
//               > 0   - print depth levels at most of the dirname St_DataSet
//
  St_DataSet *set= fWorkingDataSet;
  if (dirname && strlen(dirname)) set= Find(dirname);
  if (set) set->ls(depth);
  return set;
}
 
//______________________________________________________________________________
St_DataSet *St_DataSetIter::Mkdir(const Char_t *dirname)
{
 St_DataSet *set = 0;
 set = Find(dirname,0,kTRUE);
 if (!fNext)  Reset();  // Create a new iterator
 return set;
}

//______________________________________________________________________________
void St_DataSetIter::Notify(St_DataSet *)
{
  //
  //  Notify(St_DataSet *dataset)
  //
  //  This dummy method is called when St_DataSetIter::Find dive in "dataset"
  //  to look for thew next level of the dataset's
  //  printf("void St_DataSetIter::Notify(St_DataSet *) level: %d %s\n",fDepth,ds->GetName());
  //
} 
//______________________________________________________________________________
Int_t St_DataSetIter::Rmdir(St_DataSet *dataset,Option_t *)
{
//
//  Remove the St_DataSet *dataset from the current dataset
//
  St_DataSet *set = dataset;
  if (set) {
    delete set;
    if (set == fRootDataSet) fRootDataSet = 0;
    fWorkingDataSet = fRootDataSet;
  }
  return (Int_t)dataset;
}
 
//______________________________________________________________________________
St_DataSet *St_DataSetIter::Next( EDataSetPass mode)
{
 ////////////////////////////////////////////////////////////////////////////////
 //
 // returns the pointer the "next" St_DataSet object
 //         = 0 if all objects have been returned.
 //
 //  mode = kContinue  - default normal mode
 //         kPrune     - stop passing of the current branch but continue with the next one if any
 //         kUp        - break passing, return to the previous level, then continue
 //         all other  - are treated as "kContinue"
 //
 ////////////////////////////////////////////////////////////////////////////////
 
  if (fMaxDepth==1) fDataSet = fNext ? NextDataSet(*fNext) :0;
  else {
    // Check the whether the next level does exist 
    Bool_t mustNotify = kFALSE;
    if (fDataSet && (fDepth < fMaxDepth || fMaxDepth ==0) && mode == kContinue ) 
    {
      // create the next level iterator, go deeper
      TList *list  = fDataSet->GetList();
      // Look for the next level
      if (list && list->GetSize() ) {
         fDepth++;
         mustNotify = kTRUE;
         if (fDepth >= 100) 
            Error("Next()"
                  ," to many nested levels of your St_DataSet has been detected");
         fNextSet[fDepth-1] = new TIter(list);
      }
    }

    // Pick the next object of the current level
    TIter *next = fNextSet[fDepth-1];
    if (next) {
      fDataSet = 0;
      if (mode != kUp) fDataSet = NextDataSet(*next);

      // Go upstair if the current one has been escaped
      if (!fDataSet) {
        // go backwards direction
        while (!fDataSet && fDepth > 1) {
          fDepth--;
          delete next;
          next =  fNextSet[fDepth-1];
          St_DataSet *set = NextDataSet(*next);
          if (set) 
             fDataSet = set;
        }
      }
      if (mustNotify && fDataSet) Notify((St_DataSet *)fDataSet);
    }
  }
  return (St_DataSet *)fDataSet;
}

//______________________________________________________________________________
St_DataSet *St_DataSetIter::NextDataSet(Int_t nDataSet)
{
 // Pick the next object of the  level provided
    TIter *next = fNextSet[nDataSet];
    if (next) return NextDataSet(*next);
    return 0;
}
//______________________________________________________________________________
St_DataSet *St_DataSetIter::Find(const Char_t *path, St_DataSet *rootset,
                                 Bool_t mkdirflag)
{
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//           "path" ::= <relative path> | <absolute path> | <empty>           //
//                                                                            //
//  "relative path" ::= <dataset name> | <dataset name>/<dataset name>        //
//                                                                            //
//  "absolute path" ::= /<relative path>                                      //
//  "empty"         ::= zero pointer | pointer to zero length string          //
//                                                                            //
// "relative path": the search is done against of fWorkingDataSet data mem    //
// "absolute path": the search is done against of fRootDataSet    data mem    //
// "empty path"   : no search is done just next St_DataSet is returned if any //
//                                                                            //
//  Remark: This version can not treat any "special name" like "..", ".", etc //
//  ------                                                                    //
////////////////////////////////////////////////////////////////////////////////
   St_DataSet *dataset=0,*dsnext=0,*ds=0;
   Int_t len=0,nextlen=0,yes=0,anywhere=0,rootdir=0;
   const Char_t *name=0,*nextname=0;
   TList *tl=0;
   
   name = path;
   if (!name) return rootset;
   dataset = rootset;
   if (!dataset) {// Starting point
     rootdir = 1999;
     dataset = (path[0]=='/') ? fRootDataSet:fWorkingDataSet;}

   if (name[0] == '/') name++;
   
   if (!strncmp(name,".*/",3)) {anywhere=1998; name +=3;} 

   len = strcspn(name," /");   
   if (!len) return dataset;

   if (!dataset) goto NOTFOUND;

//	Check name of root directory
   if (rootdir) 
   {
     nextname = dataset->GetName();
     nextlen  = strlen(nextname);
     if (nextlen==len && !strncmp(name,nextname,len)) 
        return Find(name+len,dataset,mkdirflag);
   }

   tl = dataset->GetList();
   if (tl) {
     TIter next(tl);
     while ( (dsnext = NextDataSet(next)) ) 
     { //horisontal loop 
        nextname = dsnext->GetName();
        if (!nextname)	continue;
        yes = name[0]=='*';	// wildcard test
        if (!yes) {		// real     test
           nextlen  = strlen(nextname);
           yes = (len == nextlen);
           if (yes) 
              yes = !strncmp(name,nextname,len);
        }
      
        if (yes) 
        {//go down
          Notify(dsnext);
          fDepth++;
          ds = Find(name+len,dsnext,mkdirflag);
          fDepth--;
          if (ds) 
             return ds;
        }
       
        if (!anywhere) continue; 	// next horizontal
        ds = Find(name,dsnext,mkdirflag);
        if (ds) 
             return ds;
     } 					// end of while
   }

NOTFOUND:
   if (mkdirflag) 
   {
     // create dir 
     char buf[512];buf[0]=0; strncat(buf,name,len);
     ds = new St_DataSet(buf);
     if (!fRootDataSet) 	fRootDataSet    = ds;
     if (!fWorkingDataSet) 	fWorkingDataSet = ds;
     if (dataset)
        dataset->Add(ds);
     else {
        dataset = ds;
        name +=len;
     }
     
     return Find(name,dataset,mkdirflag);
   }
  
   return 0;
}     
 
//______________________________________________________________________________
void St_DataSetIter::Reset(St_DataSet *l, int depth)
{
  //
  // St_DataSet *l != 0 means the new start pointer
  //    depth      != 0 means the new value for the depth 
  //                    otherwise the privious one is used;
  //
  fDataSet = 0;
  if (fMaxDepth != 1) {
  // clean all interators
    for (int i = fDepth-1;i>=0;i--) {
      TIter *s = fNextSet[i];
      if (s) delete s;
    }
    fNext = 0; // this iterator has been deleted in the loop above
  }
  fDepth = 1;

  if (l) {
    fRootDataSet    = l;
    fWorkingDataSet = l;
    SafeDelete(fNext);
    if (fRootDataSet->GetList() )
             fNext = new TIter(fRootDataSet->GetList() );
  }
  else {
    fWorkingDataSet = fRootDataSet;
    if (fNext)
        fNext->Reset();
    else if (fRootDataSet->GetList() )
        fNext = new TIter(fRootDataSet->GetList() );
  }
  // set the new value of the maximum depth to bypass
  if (depth) fMaxDepth = depth;
}
//______________________________________________________________________________
St_DataSet *St_DataSetIter::Shunt(St_DataSet *set, St_DataSet *dataset)
{
 ///////////////////////////////////////////////////////////////////////////////
 //                                                                           //
 // Shunt - moves the set to the dataset defined with the second parameters   //
 //                                                                           //
 // St_DataSet dataset != 0 - Add the set to the St_DataSet *dataset          //
 //                                                                           //
 //                     = 0 - (by default) to the current St_DataSet defined  //
 //                          with fWorkingDataSet data member                 //
 //                                                                           //
 //  returns  the pointer to set if successful or ZERO pointer                //
 //  =======                                                                  //
 //                                                                           //
 //  Note: If this St_DataSetIter is empty (i.e. Cwd() returns 0), the "set"  //
 //        becomes the "root" dataset of this iterator                        //                                                                         //
 ///////////////////////////////////////////////////////////////////////////////
 
  if (!set) return 0;
  St_DataSet *s =  dataset;
  if (!s) s = Cwd();
  if (s) {
     s->Shunt(set);
     s = set;
  }
  else {
  //  make the coming dataset the current one for the iterator
     s = set;
     fRootDataSet    = s;
     fWorkingDataSet = s;
     if (fNext) {
       Error("Shunt","St_DataSetIter has been corrupted ;-!"); 
       delete fNext; 
       fNext = 0;
     }
     fNext = new TIter(s->GetList() );
  }
  return s;
}
 
//______________________________________________________________________________
St_DataSet *St_DataSetIter::Shunt(St_DataSet *dataset, const Char_t *path)
{
 ///////////////////////////////////////////////////////////////////////////////
 //                                                                           //
 // Shunt                                                                     //
 //                                                                           //
 // Char_t path != 0 - Move a St_DataSet dataset from its parent to           //
 //                    the St_DataSet dataset                                 //
 //                    defined with "path"                                    //
 //              = 0 - (by default) to the current St_DataSet defined         //
 //                    with fWorkingDataSet data member                       //
 //                                                                           //
 //  returns the dataset is success or ZERO pointer                           //
 //  =======                                                                  //
 //                                                                           //
 ///////////////////////////////////////////////////////////////////////////////
 if (!dataset) return 0;
 St_DataSet *set = 0;
 if (path && strlen(path)) set = Find(path);
 return Shunt(dataset,set);
}
//______________________________________________________________________________
St_DataSet *St_DataSetIter::operator[](const Char_t *path)
{ 
 //
 // operator [] returns the pointer to the St_DataSet if it does contain 
 // any data (St_Table for example)
 //
 //  Input:
 //     path  = The path to the dataset to find
 //
 //  Output:
 //     pointer to the dataset if it found and 
 //     its St_DataSet::HasData() method returns non-zero
 //     (see for example St_Table::HasData() )     
  St_DataSet *dataSet = Find(path);
  if (dataSet && dataSet->HasData()) return dataSet;
  return 0;
}
