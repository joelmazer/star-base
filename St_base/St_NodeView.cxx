//*-- Author :    Valery Fine(fine@bnl.gov)   25/12/98 
// $Id:
// $Log:

#include "TBrowser.h"
#include "St_NodeView.h"
#include "St_NodeViewIter.h"
#include "St_NodePosition.h"
#include "TROOT.h"
#include "TView.h"
#include "TPadView3D.h"
#include "TGeometry.h"
#include "TVirtualPad.h"
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// St_NodeView                                                          //
//                                                                      //
// St_NodeView class is a special kind of St_DataSet with one extra     //
// pointer to wrap any TObject onto St_DataSet object                   //
//                                                                      //
//  BE CAREFUL !!!                                                      //
//  One has to use it carefully no conrol over that extra object        //
//  is performed. This means: the onject m_Obj data-member points to can//
//  be destroyed with no this kbject notifying.                         //
//  There is no tool /protection to check whether m_Obj is till alive.  //
//  It is one's  code responsilitiy                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

ClassImp(St_NodeView)
//_____________________________________________________________________________
St_NodeView::St_NodeView(St_NodeView *viewNode,St_NodePosition *nodePosition)
            : St_ObjectSet(viewNode->GetName(),(TObject *)nodePosition)
{
  if (viewNode) 
  {
     SetTitle(viewNode->GetTitle());
     EDataSetPass mode = kContinue;
     St_NodeViewIter next(viewNode,0);
     St_NodeView *nextView = 0;
     while (nextView = (St_NodeView *)next(mode)){     
       mode = kContinue;
       if (nextView->IsMarked()) {
         St_NodePosition tempPos = next[0];
         St_NodePosition *position = new St_NodePosition(tempPos);
         printf("AAAA!!! %s \n",GetName());
         if (!position->GetNode()) {
             printf("uuuuuuuuuu !!! %s \n",GetName());
         }
         Add(new St_NodeView(nextView,position));
//         mode = kPrune;
       }
    }
  }
}
//_____________________________________________________________________________
St_NodeView::St_NodeView(St_Node &pattern,const St_NodePosition *nodePosition,EDataSetPass iopt)
            : St_ObjectSet(pattern.GetName(),(TObject *)nodePosition)
{
  //
  // Creates St_DataSet (clone) with a topology similar with St_DataSet *pattern
  //
  //  Parameters:
  //  -----------
  //  pattern        - the pattern dataset
  //  iopt = kStruct - clone only my structural links
  //         kAll    - clone all links
  //         kRefs   - clone only refs
  //         kMarked - clone marked (not implemented yet) only
  //
  //   All new-created sets become the structural ones anyway.
  //

  //  cout << "ctor for " << GetName() << " - " << GetTitle() << endl;
  SetTitle(pattern.GetTitle());
  if ( pattern.IsMarked() ) Mark(); 
  St_NodePosition *position = 0;
  TIter next(pattern.GetListOfPositions());
  Bool_t optsel = (iopt == kStruct);
  Bool_t optall = (iopt == kAll);
  while (position = (St_NodePosition *)next()) {
    // define the the related St_Node
     St_Node *node = position->GetNode(); 
     if (node) {
       St_DataSet *parent = node->GetParent(); 
       if ( optall || (optsel && parent == (St_DataSet *)&pattern) )
                                  Add(new St_NodeView(*node,position));
     }
     else 
       Error("St_NodeView ctor","Position with NO node attached has been supplied");
     
  }
}
//_____________________________________________________________________________
void St_NodeView::Browse(TBrowser *b){
  St_ObjectSet::Browse(b);
//  St_NodePosition *pos = GetPosition();
//  if (pos) pos->Browse(b);
//    b->Add(pos); 
}

//______________________________________________________________________________
Int_t St_NodeView::DistancetoPrimitive(Int_t px, Int_t py)
{
//*-*-*-*-*-*-*-*-*Compute distance from point px,py to a St_NodeView*-*-*-*-*-*
//*-*                  ===========================================
//*-*  Compute the closest distance of approach from point px,py to the position of 
//*-*  this node.
//*-*  The distance is computed in pixels units.
//*-*
//*-*  It is restricted by 2 levels of St_Nodes
//*-*
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
 
   const Int_t big = 9999;
   const Int_t inaxis = 7;
   const Int_t maxdist = 5;

   Int_t dist = big;
 
   Int_t puxmin = gPad->XtoAbsPixel(gPad->GetUxmin());
   Int_t puymin = gPad->YtoAbsPixel(gPad->GetUymin());
   Int_t puxmax = gPad->XtoAbsPixel(gPad->GetUxmax());
   Int_t puymax = gPad->YtoAbsPixel(gPad->GetUymax());
 
//*-*- return if point is not in the user area
   if (px < puxmin - inaxis) return big;
   if (py > puymin + inaxis) return big;
   if (px > puxmax + inaxis) return big;
   if (py < puymax - inaxis) return big;
 
   TView *view =gPad->GetView();
   if (!view) return big;
 
   St_NodePosition *position = GetPosition();
   St_Node *thisNode  = 0;
   TShape  *thisShape = 0;
   if (position) {
     thisNode = position->GetNode();     
     position->UpdatePosition(); 
     if (thisNode) {     
        thisShape    = thisNode->GetShape();
        if (thisNode->GetVisibility() && thisShape && thisShape->GetVisibility()) {
          dist = thisShape->DistancetoPrimitive(px,py);
          if (dist < maxdist) {
             gPad->SetSelected(this);
             return 0;
          }
       }
     }
   }
//   if ( TestBit(kSonsInvisible) ) return dist;
 
//*-*- Loop on all sons
   TList *fNodes =  GetList();
   Int_t nsons = fNodes?fNodes->GetSize():0;
   Int_t dnode = dist;
   if (nsons) {
      gGeometry->PushLevel();
      St_Node *node;
      TIter  next(fNodes);
      while ((node  = (St_Node *)next())) {
         dnode = node->DistancetoPrimitive(px,py);
         if (dnode <= 0)  break;
         if (dnode < dist) dist = dnode;
         if (gGeometry->GeomLevel() > 2) break;
      }
      gGeometry->PopLevel();
   }
 
   if (gGeometry->GeomLevel()==0 && dnode > maxdist) {
      gPad->SetSelected(view);
      return 0;
   } else
      return dnode;
}

//______________________________________________________________________________
void St_NodeView::Draw(Option_t *option)
{
//*-*-*-*-*-*-*-*-*-*-*-*Draw Referenced node with current parameters*-*-*-*
//*-*                   =============================================
 
   TString opt = option;
   opt.ToLower();
//*-*- Clear pad if option "same" not given
   if (!gPad) {
      if (!gROOT->GetMakeDefCanvas()) return;
      (gROOT->GetMakeDefCanvas())();
   }
   if (!opt.Contains("same")) gPad->Clear();
 
//*-*- Draw Referenced node
   gGeometry->SetGeomLevel();
   gGeometry->UpdateTempMatrix();
 
   AppendPad(option);
 
//*-*- Create a 3-D View
   TView *view = gPad->GetView();
   if (!view) {
      view = new TView(1);
      view->SetAutoRange(kTRUE);
      Paint("range");
      view->SetAutoRange(kFALSE);
   }
}

//_____________________________________________________________________________
St_Node *St_NodeView::GetNode(){ 
  St_NodePosition *pos = GetPosition();
  if (pos)
    return pos->GetNode();
  return 0;
}

//______________________________________________________________________________
void St_NodeView::Paint(Option_t *option)
{
//*-*-*-*-*-*-*-*-*-*-*-*Paint Referenced node with current parameters*-*-*-*
//*-*                   ==============================================
//*-*
//*-*  vis = 1  (default) shape is drawn
//*-*  vis = 0  shape is not drawn but its sons may be not drawn
//*-*  vis = -1 shape is not drawn. Its sons are not drawn
//*-*  vis = -2 shape is drawn. Its sons are not drawn
//*-*
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

// restrict the levels for "range" option
  Int_t level = gGeometry->GeomLevel();
//  if (option && option[0]=='r' && level > 3 && strcmp(option,"range") == 0) return;
//  if (option && option[0]=='r' && level > 3 ) return;

  TPadView3D *view3D=gPad->GetView3D();

  St_Node *thisNode  = 0;
  St_NodePosition *position = GetPosition();
  
  // UpdatePosition does change the current matrix and it MUST be called FIRST !!!
  if (position) {
     thisNode  = position->GetNode();
     position->UpdatePosition(option);      
  }

  if (thisNode)  thisNode->PaintShape(option);  
////---   if ( thisNode->TestBit(kSonsInvisible) ) return;
 
//*-*- Paint all sons
  TList *fNodes =  GetList();
  Int_t nsons = fNodes?fNodes->GetSize():0;

  if(!nsons) return;
 
  gGeometry->PushLevel();
  St_NodeView *node;
  TIter  next(fNodes);
  while ((node = (St_NodeView *)next())) {
     if (view3D && position)  view3D->PushMatrix();

     node->Paint(option);

     if (view3D && position)  view3D->PopMatrix();
  }
  gGeometry->PopLevel();
}

