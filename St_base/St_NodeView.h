//*-- Author :    Valery Fine(fine@bnl.gov)   25/12/98 
// $Id:
// $Log:
#ifndef STAR_St_NodeView
#define STAR_St_NodeView

#include "St_Node.h"

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// St_NodeView                                                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class St_NodeView : public St_ObjectSet {
public:
  St_NodeView():St_ObjectSet(){;}
  St_NodeView(St_Node &pattern,const St_NodePosition *nodePosition=0,EDataSetPass iopt=kStruct);
  virtual ~St_NodeView(){}
  virtual void Browse(TBrowser *b);
  virtual St_NodePosition *GetPosition(){ return (St_NodePosition *)GetObject();}
  virtual St_Node         *GetNode();
  ClassDef(St_NodeView,1)
};
#endif

