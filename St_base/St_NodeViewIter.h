//*-- Author :    Valery Fine(fine@bnl.gov)   25/01/99  
// $Id: 
// $Log:

#ifndef STAR_St_NodeViewIter
#define STAR_St_NodeViewIter

#include "St_DataSetIter.h" 
#include "St_NodePosition.h"

class TObjArray;
class St_NodeView;

class St_NodeViewIter : public St_DataSetIter {
private:
protected:
     TObjArray    *m_Positions; // the array of the Node position in the absolute system 
     virtual St_NodePosition *GetPosition(Int_t level=0);
     virtual St_NodePosition *SetPositionAt(St_Node *node,Double_t x=0, Double_t y=0, Double_t z=0, TRotMatrix *matrix=0);
public:
     St_NodeViewIter(St_NodeView *view, Int_t depth=1, Bool_t dir=kIterForward);
    ~St_NodeViewIter();
     virtual void            Notify(St_DataSet *set);
     virtual void            Reset(St_DataSet *l=0,Int_t depth=0);

     St_NodePosition         operator[](Int_t level);

     St_NodePosition        *UpdateTempMatrix(St_NodePosition *curPosition);
     void                    ResetPosition(Int_t level=0, St_NodePosition *newPosition=0);
     ClassDef(St_NodeViewIter,0)
};


#endif 

