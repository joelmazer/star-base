//*CMZ :          13/08/98  18.27.27  by  Valery Fine(fine@bnl.gov)
//*-- Author :    Valery Fine(fine@mail.cern.ch)   13/08/98 

#ifndef ROOT_St_DataSet
#define ROOT_St_DataSet
 
 
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// St_DataSet                                                           //
//                                                                      //
// St_DataSet class is a base class to implement the directory-like     //
// data structures and maintain it via St_DataSetIter class iterator    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
 
//*KEEP,TList.
#include "TList.h"
//*KEEP,TNamed.
#include "TNamed.h"
//*KEND.
 
class St_DataSetIter;
class TBrowser;
 
class St_DataSet : public TNamed
{
 friend class St_DataSetIter;
 protected:
    TObject     *fMother; // pointer to mother of the directory
    TList       *fList;   // List of the the the objects included into this dataset
    virtual void SetParent(St_DataSet *parent);
    virtual void SetMother(TObject *mother) {fMother = mother;}
    St_DataSet(const Char_t *name,const Char_t *title) : TNamed(name,title){} // to support TDictionary
 
 public:
 
    St_DataSet(const Char_t *name="", St_DataSet *parent=0);
    virtual ~St_DataSet();
            void        Add(St_DataSet *dataset);
    virtual void        Browse(TBrowser *b);
    virtual St_DataSet *Data() { return HasData() ? this : 0; }  // returns this pointer the derived classes if any
            TObject    *GetMother() const { return fMother; }
    virtual St_DataSet *GetParent() const { return (St_DataSet *)fMother;}
            TList      *GetList() const {return fList;}
            TList      *GetListOfDataset() const {return GetList();}
            Int_t       GetListSize() const;
    virtual Long_t      HasData() const {return 0;} // Check whether this dataset has extra "data-members"
    virtual Bool_t      IsFolder(){return kTRUE;}
    virtual Bool_t      IsThisDir(const Char_t *dirname) const ;
    virtual void        ls(Option_t *option="");    // Option "*" means print all levels
    virtual void        ls(Int_t deep);             // Print the "deep" levels of this datatset
    virtual void        Update();                   // Update dataset
    virtual void        Remove(St_DataSet *set);
    ClassDef(St_DataSet,1)
};
 
inline Int_t     St_DataSet::GetListSize() const { return fList ? fList->GetSize():0; }
// inline Bool_t    St_DataSet::IsFolder(){ return fList->Last() ? kTRUE : kFALSE;}
 
 
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// St_DataSetIter                                                       //
//                                                                      //
// Iterator of St_DataSet lists.                                        //
//                                                                      //
// Provides "standard" features of the TIter class for St_DataSet object//
//                             and                                      //
// allows navigating St_DataSet structure using the custom "directory"  //
//    notation (see St_DataSet::Next(const Char *path) method)          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
 
class St_DataSetIter : public TObject{
private:
   TIter      *fNext;            // "standard" ROOT iterator for containers
   St_DataSet *fRootDataSet;     // Pointer to the ROOT DataSet
   St_DataSet *fWorkingDataSet;  // Pointer to the working DataSet
 
public:
  St_DataSetIter(St_DataSet *l=0, Bool_t dir = kIterForward);
  virtual         ~St_DataSetIter() {if (fNext) delete fNext; fNext = 0;}
 
  virtual St_DataSet    *Add(St_DataSet *set){return Add(set,(St_DataSet *)0);}
  virtual St_DataSet    *Add(St_DataSet *set, const Char_t *path);
  virtual St_DataSet    *Add(St_DataSet *set, St_DataSet *dataset);
 
  virtual St_DataSet    *Cd(const Char_t *dirname);
  virtual St_DataSet    *operator()() { return  (St_DataSet *)(fNext?fNext->Next():0);}
  virtual St_DataSet    *operator()(const Char_t *path) { return Next(path); }
  virtual St_DataSet    *Dir(Char_t *dirname);
  virtual St_DataSet    *Ls(const Char_t *dirname="",Option_t *opt="");
  virtual St_DataSet    *Ls(const Char_t *dirname,Int_t deep);
  virtual St_DataSet    *ls(const Char_t *dirname="",Option_t *opt=""){return Ls(dirname,opt);}
  virtual St_DataSet    *ls(const Char_t *dirname,Int_t deep){return Ls(dirname,deep);}
  virtual St_DataSet    *Mkdir(const Char_t *dirname);
  virtual St_DataSet    *Md(Char_t *dirname){return Mkdir(dirname);}
  virtual St_DataSet    *Pwd(){return fWorkingDataSet;}
  virtual Int_t          Rmdir(St_DataSet *dataset,Option_t *option="");
  virtual Int_t          Rmdir(Char_t *dirname,Option_t *option=""){return Rmdir(Next(dirname),option);}
  virtual Int_t          Rd(Char_t *dirname,Option_t *option=""){return Rmdir(Next(dirname),option);}
  virtual St_DataSet    *Next(){ return (St_DataSet *) (fNext ? fNext->Next():0);}
  virtual St_DataSet    *Next(const Char_t *path, St_DataSet *rootset=0,Bool_t mkdir=kFALSE);
  const Option_t *GetOption() const { return fNext ? fNext->GetOption():0; }
  virtual void           Reset(St_DataSet *l=0);
  ClassDef(St_DataSetIter,0)
};
 
#endif
