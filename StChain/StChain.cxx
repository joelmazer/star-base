// $Id: StChain.cxx,v 1.33 1999/03/04 02:26:04 fisyak Exp $
// $Log: StChain.cxx,v $
// Revision 1.33  1999/03/04 02:26:04  fisyak
// Add read Chain
//
// Revision 1.32  1999/03/02 03:20:52  fine
//  Table counter has been  moved from StMaker to StChain
//
// Revision 1.31  1999/02/28 19:18:44  fisyak
// Add tag information
//
// Revision 1.30  1999/02/27 21:18:36  fine
//  Minor fix for Maker timers
//
// Revision 1.29  1999/02/27 20:13:45  fine
// Total job time and relative time have been introduced
//
// Revision 1.28  1999/02/27 18:13:40  fine
// Bug fixed: SetEvent(i) has been introduced for Make(i) method
//
// Revision 1.27  1999/02/22 02:21:49  fisyak
// Add GetGeometry
//
// Revision 1.26  1999/02/20 18:48:56  fisyak
// Add event/run information to Chain
//
// Revision 1.25  1999/01/20 23:44:46  fine
// The special Input/Output makers and the static variable StChain::g_Chain have been introduced
//
// Revision 1.24  1999/01/02 19:08:12  fisyak
// Add ctf
//
// Revision 1.23  1998/12/21 19:42:50  fisyak
// Move ROOT includes to non system
//
// Revision 1.22  1998/11/29 20:01:09  fisyak
// Fix typo with Run/run
//
// Revision 1.21  1998/11/25 21:58:21  fisyak
// Cleanup
//
// Revision 1.20  1998/11/22 18:28:05  fisyak
// Add name of tag
//
// Revision 1.19  1998/11/19 01:23:56  fine
// StChain::MakeDoc has been introduced, StChain::MakeDoc has been fixed (see macros/bfc_doc.C macro
//
// Revision 1.18  1998/10/31 00:21:30  fisyak
// Makers take care about branches
//
// Revision 1.17  1998/10/07 18:43:57  perev
// Add Spy classes for Farm Monitor
//
// Revision 1.16  1998/10/06 18:00:26  perev
// cleanup
//
// Revision 1.15  1998/09/23 20:22:51  fisyak
// Prerelease SL98h
//
// Revision 1.14  1998/09/16 14:29:33  love
// St_DataSetIter.h added
//
// Revision 1.13  1998/08/26 12:15:08  fisyak
// Remove asu & dsl libraries
//
// Revision 1.12  1998/08/18 14:05:01  fisyak
// Add to bfc dst
//
// Revision 1.11  1998/08/07 19:34:53  fisyak
// Add St_run_Maker
//
// Revision 1.10  1998/07/23 21:03:30  fisyak
// Add more comments
//
// Revision 1.9  1998/07/23 11:32:11  fisyak
// Add comments
//
// Revision 1.8  1998/07/20 15:08:08  fisyak
// Add tcl and tpt
//
// Revision 1.7  1998/07/19 21:16:29  fisyak
// add log information
//
// Revision 1.6  1998/07/19 21:14:48  fisyak
// add log information
//
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StChain                                                              //
//                                                                      //
// Main class to control the StChain program.                           //
//                                                                      //
// This class was done on the base of Begin_html <a href="http://root.cern.ch/root/Atlfast.html"> ATLFAST </a>End_Html C++ class library           //
// This class is a general framework for programs that needs to:        //
//    - Initialise some parameters                                      //
//    - Loop on events                                                  //
//    - Print results and save histograms, etc                          //
//                                                                      //
// The event processor StChain::Make loops on a list of Makers          //
// where each maker performs some task on the event data and generates  //
// results.                                                             //
// New Makers can be inserted by a user without modifying this class.   //
// Note that the order in which the Makers are called is the order      //
// of insertion in the list of Makers.                                  //
// Each Maker is responsible for creating its branch of the Tree.       //
// The following table shows the list of makers currently implemented   //
// The default option to Save the Maker info in the Tree is mentioned.  //
//                                                                      //
//    Maker name        Save in Tree                                    //
//    ==========        ============                                    //
//   xdfin              event/geant                                     //
//   evg_Maker          event                                           //
//   tss_Maker          event/raw_data/tpc                              //
//   srs_Maker          event/raw_data/svt                              //
//   tcl_Maker          event/data/tpc/hits                             //
//   tpt_Maker          event/data/tpc/tracks                           //
//                                                                      //
//                                                                      //
// Makers must derive from the base class StMaker.                      //
// StMaker provides a common interface to all Makers.                   //
// Each Maker is responsible for defining its own parameters and        //
// histograms.                                                          //
// Each Maker has its own list of histograms.                           //
// Each Maker has an associated companion class corresponding to the    //
// type of physics object reconstructed by the Maker.                   //
// For example, St_tcl_Maker     fill   event/data/tpc/hits DataSet     //
//              St_tpt_Maker     fill   event/data/tpc/tracks DataSet   // 
// The pointer supporting the created object(s) is defined in StMaker   //
//   m_DataSet points to a DataSet owned by the Maker                   //
//                                                                      //
// The function StChain::Maketree must be called after the creation     //
// of the StChain object to create a Root Tree (not yet implemented).   //
//                                                                      //
// An example of main program/macro to use StChain is given below:      //
//========================================================================
//void umain(Int_t nevents=100)
//{
//   gROOT->Reset();
//   gSystem->Load("StChain");  // dynamically link the compiled shared library
//
//   // Open the root output file
//   TFile file("StChain.root","recreate","StChain root file",2);
//   St_XDFFile xdffile_in("StChain_in.xdf","r"); // Open XDF file to read  event
//   St_XDFFile xdffile_out("StChain.xdf","w");     // Open XDF file to write event
//   
//   StChain chain("StChain");     // create main object to run StChain
//   St_xdfin_Maker xdfin("XdfIn","test"); // create xdfin object to run in StChain
//   St_evg_Maker evg_Maker("evg_Maker","event"); // event generator
//   St_tss_Maker tss_Maker("tss_Maker","event/raw_data/tpc"); // TPC slow simulator
//   St_srs_Maker srs_Maker("srs_Maker","event/data/svt"); // SVT fast simulator
//   St_tcl_Maker tcl_Maker("tcl_Maker","event/data/tpc/hits"); // TPC clustering
//   St_tpt_Maker tpt_Maker("tpt_Maker","event/data/tpc/tracks"); // TPVC tracking
//   St_xdfout_Maker xdfout("XdfOut","test"); // create xdfin object to run in StChain
//   chain.SetInputXDFile(&xdffile_in);      // pass file to xdfin
//   chain.SetOutputXDFile(&xdffile_out);    // pass file to xdfout
//
//   User user;           // create an object of the User class defined in user.C
//
//   chain.Init();      // Initialise event (maker histograms,etc)
//   chain.MakeTree();  // Create the Root tree
//
//   gROOT->LoadMacro("user.C");  // compile/interpret user file
//
//   for (Int_t i=0; i<nevents; i++) {
//      if (i%100 == 0) printf("In loop:%d\n",i);
//      chain.Make(i);       // Generate and reconstruct event
//      user.FillHistograms(); // User has possibility to decide if store event here!
//      chain.FillTree();
//||    chain.FillXDF(xdffile_out); 
//||    xdffile.NextEventPut(chain.DataSet()); 
//      chain.Clear();       // Clear all event lists
//   }
//   chain.Finish();
//
//   // save objects in Root file
//   chain.Write();  //save main StChain object (and run parameters)
//   xdffile_out.CloseXDF();  
//}
//========================================================================
//                                                                      //
// This example illustrates how to:                                     //
//    - Load a shared library                                           //
//    - Open a Root file                                                //
//    - Initialise StChain                                              //
//    - Load some user code (interpreted)                               //
//      This user code may redefine some Maker parameters               //
//    - Make a loop on events                                           //
//    - Save histograms and the main StChain object and its Makers      //
//                                                                      //
//========================================================================
//  An example of a User class is given below:                          //
//========================================================================
//
//#ifndef user_H
//#define user_H
//
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// User                                                                 //
//                                                                      //
// Example of a user class to perform user specific tasks when running  //
// the StChain program.                                                 //
//                                                                      //
// This class illustrates:                                              //
//   - How to set run parameters                                        //
//   - How to create and fill histograms                                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
//
//class TH1F;
//class StChain;
//class StClusterMaker;
//class StPhotonMaker;
//
//class User {
//
//private:
//   TH1F             *m_hist1;       //pointer to histogram
//   TH1F             *m_hist2;       //pointer to histogram
//   TH1F             *m_hist3;       //pointer to histogram
//public:
//               User();
//   void        FillHistograms();
//   void        SetRunParameters();
//
//#endif
//};
//
//_________________________________________________________________________
//User::User() 
//{
//   SetRunParameters();  //change default parameters
//
//         Create a few histograms
//   m_hist1 = new TH1F("hist1","Number of tracks per event",100,0,100);
//   m_hist2 = new TH1F("hist2","Number of clusters",100,0,100);
//   m_hist3 = new TH1F("hist3","Number of isolated muons",20,0,20);
//}
//
//_________________________________________________________________________
//void User::FillHistograms()
//{
////   m_hist1.Fill(event->GetNtracks());
////   m_hist2.Fill(event->GetNclusters));
////   m_hist3.Fill(event->GetNIsoMuons());
//}
//
//_________________________________________________________________________
//void User::SetRunParameters()
//{
//  // change StChain default parameters
//
//   gStChain->SetSmearMuonOpt(0);
//   gStChain->ClusterMaker()->SetGranBarrelEta(0.12);
//   gStChain->PhotonMaker()->SetMinPT(6.);
//   gStChain->TriggerMaker()->SetMuoEtaCoverage(2.8);
//
//}
//======================end of User class=================================
//
//////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <malloc.h>
#include <iostream.h>
#include <iomanip.h>

#include "TROOT.h"
#include "TChain.h"
#include "TTree.h"
#include "TBrowser.h"
#include "TClonesArray.h"
#include "TGeometry.h"
#include "TSystem.h"
#include "St_XDFFile.h"
#include "St_Table.h"
#include "St_DataSetIter.h"
#include "St_FileSet.h"
#include "StChain.h"
#include "StMaker.h"
// #include "stHistBrowser.h"
// #include "StVirtualDisplay.h"

StChain *gStChain;

ClassImp(StChain)


//_____________________________________________________________________________
StChain::StChain() 
{
   SetName("StChain");
   SetTitle("The STAR default chain");
   m_Tree          = 0;
   m_Makers        = 0;
   m_Mode          = 0;
   m_DataSet       = 0;
   m_DebugLevel    = kNormal;
   if (!gStChain) gStChain = this;
}

//_____________________________________________________________________________
StChain::StChain(const char *name, const char *title):
m_VersionCVS("$Id: StChain.cxx,v 1.33 1999/03/04 02:26:04 fisyak Exp $"),
m_VersionTag("$Name:  $"),
m_DateTime(),
mProcessTime()
{
   SetName(name);
   SetTitle(title);
   gStChain      = this;
   m_Version     = 100;       //StChain  version number and release date
   m_VersionDate = 180698;
   m_Run         = 0;
   m_Event       = 0;
   m_Tree        = 0;
   m_Mode        = 0;
   m_BImpact     = 0;
   m_PhImpact    = 0;
   m_Mode        = 0;
   m_DateTime.Set(1995,0);
   mAwest        = 0;
   mAeast        = 0;
   mCenterOfMassEnergy = 0;
   mBunchCrossingNumber = 0;
   mTriggerMask  = 0;
   m_EvenType    = TString("Unknown");
//   m_Display     = 0;
   m_DataSet       = 0;
   m_DebugLevel    = kNormal;
   SetDefaultParameters();

   gROOT->GetListOfBrowsables()->Add(this,GetName());

// create the support list for the various lists of StChain objects
   m_Makers  = new TList();

}

//_____________________________________________________________________________
StChain::~StChain()
{
//   m_Makers->Delete();
//   delete m_Makers;
}
//______________________________________________________________________________
void StChain::Browse(TBrowser *b)
{
// Browse includes the various maker-made objects into the standard ROOT TBrowser
// The following picture can be done with just a staement like
//
//  TBrowser b("Event",event);  
//
// where the "event" variable should be defined somewhere in the user's code as 
//
//          St_DataSet *event;
//
// Begin_Html <P ALIGN=CENTER> <IMG SRC="gif/ChainBrowser.gif"> </P> End_Html 
//

  if( b == 0) return;

  if (m_Tree) b->Add(m_Tree,m_Tree->GetName());

//  b->Add(&m_HistBrowser, "Histograms");
//  b->Add(&m_BigBang, "BigBang");

  TIter next(m_Makers);
  StMaker *maker;
  while ((maker = (StMaker*)next())) {
     b->Add(maker,maker->GetName());
  }
  if (m_DataSet) b->Add(m_DataSet,m_DataSet->GetName()); 
}

//_____________________________________________________________________________
void StChain::Clear(Option_t *option)
{
//    Reset lists of event objects
   TIter next(m_Makers);
   StMaker *maker;
   while ((maker = (StMaker*)next())) {
      maker->Clear(option);
   }
//   if (m_Display) m_Display->Clear();
   return;
}
//_____________________________________________________________________________
St_DataSet *StChain::DataSet(const Char_t *makername, const Char_t *path) const
{
// find the maker by name and return its dataset or subdataset if "path" supplied
 St_DataSet *set = 0;
 StMaker *inputMaker = 0;
 const Char_t *inputMakerName = "Input";
 if (makername) {
  set = 0;
  if (m_Makers) {
     TIter next(m_Makers);
     StMaker *maker;
     while (maker = (StMaker*) next())
     {
         const Char_t *name = maker->GetName();
         // Find and remner the point of the special "Input" maker
         if (strcmp(name,inputMakerName) == 0) inputMaker = maker;
         if (!strcmp(maker->GetName(),makername)) break;
     }
     if (maker) 
         set = maker->DataSet(); // Get dataset from Maker
     else if (inputMaker) 
         set = inputMaker->DataSet(makername); // If failed try to pool it from the "Input" maker
       
     if (set && path && strlen(path)) set=0;
#if 0
     {
       set = set->Find(path);
       if (!set) {
       }
     }
#endif
   }
 }
 return set;
}
 
//_____________________________________________________________________________
void StChain::Draw(Option_t *option)
{
//    Insert current event in graphics pad list

#if 0
    // Check if the Event Display object has been created
   if (!m_Display) {
      Error("Draw","You must create an StDisplay object first");
      return;
   }

   m_Display->Draw(option);
#endif
}

//_____________________________________________________________________________
Int_t StChain::GetEvent(Int_t event)
{
//    Read event from Tree
   if (m_Tree) m_Tree->GetEvent(event);
   SetEvent(event);
   return kStOK;
} 

//_____________________________________________________________________________
Int_t StChain::Init()
{// Initialize Chain
   if (! m_DataSet) m_DataSet = new St_DataSet(GetName()); 
//    Initialise makers
   TIter next(m_Makers);
   StMaker *maker;
   TObject *objfirst, *objlast;
   while ((maker = (StMaker*)next())) {
     // save last created histogram in current Root directory
      objlast = gDirectory->GetList()->Last();

     // Initialise maker
      maker->StartTimer();
      St_DataSet *makerset = maker->DataSet();
      if (!makerset){
        const Char_t *makertype = maker->GetTitle();
        const Char_t *name = gSystem->BaseName(makertype);
        makerset = new St_DataSet(name);
        maker->SetDataSet(makerset);
      }
      if ( maker->Init()) {
         maker->StopTimer();
         return kStErr;
      }
      maker->StopTimer();
     // Add the Maker histograms in the Maker histograms list
      if (objlast) objfirst = gDirectory->GetList()->After(objlast);
      else         objfirst = gDirectory->GetList()->First();
      while (objfirst) {
         maker->Histograms()->Add(objfirst);
         objfirst = gDirectory->GetList()->After(objfirst);
      }
   }
   // Save Run to XDF Output file if any
   if (m_FileOut) {
     St_DataSetIter nextRun(m_DataSet);
     nextRun.Cd(GetName());
     St_DataSet *set = nextRun("run");
     if (!set) set = nextRun("Run");
     if (set) m_FileOut->NextEventPut(set);
   }
   return kStOK;
}
//_____________________________________________________________________________
void StChain::MakeDoc(const TString &stardir,const TString &outdir)
{
  // Create html documentation of this class and all makers as well
  StMaker::MakeDoc(stardir,outdir);
  
  //     Make html-docs for all defined Makers
   TIter next(m_Makers);
   StMaker *maker=0;
   while ((maker = (StMaker*)next())) 
      maker->MakeDoc(stardir,outdir);  
}

//_____________________________________________________________________________
void StChain::Paint(Option_t *option)
{
//    Paint StChain objects

//   m_Display->Paint(option);
}

//_____________________________________________________________________________
void StChain::PrintInfo()
{
//     Gives information about versions etc.
   printf("\n\n");
   printf("**************************************************************\n");
   printf("*             StChain version:%3d released at %6d         *\n",m_Version, m_VersionDate);
   printf("**************************************************************\n");
   printf("* $Id: StChain.cxx,v 1.33 1999/03/04 02:26:04 fisyak Exp $    \n");
   printf("* The chain was tagged with $Name:  $                \n");
   printf("**************************************************************\n");
//     Print info for all defined Makers
   TIter next(m_Makers);
   StMaker *maker;
   while ((maker = (StMaker*)next())) {
      maker->PrintInfo();
   }
}

//_____________________________________________________________________________
Int_t StChain::FillTree()
{
//  Fill the ROOT tree, looping on all active branches
#if 0
   TIter next(m_Makers);
   StMaker *maker;
   //   Save();
   //   MakeBranch();
   while ((maker = (StMaker*)next())) {
   //   maker->Save();
      if (maker->IsToSave())
              maker->FillClone();
   }
#endif
  // Now ready to fill the Root Tree
   if(m_Tree) return m_Tree->Fill();
   return 0;
}
//_____________________________________________________________________________
void StChain::FillXDF(St_XDFFile &file)
{
//  Put current event into XDF file
   if (&file && m_DataSet) file.NextEventPut(m_DataSet);
}

//_____________________________________________________________________________
void StChain::InitChain(TChain *chain)
{
//  Initialize branch addresses for all makers in a TChain

   if (chain == 0) return;

   m_Tree = chain;

   TIter next(m_Makers);
   StMaker *maker;
   while ((maker = (StMaker*)next())) {
      maker->SetChainAddress(chain);
   }
}

//_____________________________________________________________________________
TTree *StChain::MakeTree(const char* name, const char*title)
{
//  Create a ROOT tree
//  Loop on all makers to create the Root branch (if any)

   if (m_Tree) return m_Tree;

   m_Tree = new TTree(name,title);

   TIter next(m_Makers);
   StMaker *maker;
   //   Save();
   //   MakeBranch();
   while (maker = (StMaker*)next()) {
   //   maker->Save();
      if (maker->IsToSave()) {
#if 0
              maker->MakeTree();
#endif
              maker->MakeBranch();
      }
   }

   return m_Tree;
}
//_____________________________________________________________________________
void StChain::MakeBranch()
{
//   Adds the list of physics objects to the ATLFast tree as a new branch
   if (m_Save == 0 || m_Tree == 0) return;

//  Make a branch tree if a branch name has been set
   Int_t buffersize = 4000;
   m_BranchName = GetName();
   m_Tree->Branch(m_BranchName.Data(),ClassName(), &gStChain, buffersize);
}

//_____________________________________________________________________________
void StChain::SetBranches()
{
   if (!m_Tree) return;

   TIter next(m_Makers);
   StMaker *maker;
   //   Save();
   //   MakeBranch();
   while ((maker = (StMaker*)next())) {
   //   maker->Save();
   //   if (maker->IsToSave())
              maker->SetBranch();
   }
}


//_____________________________________________________________________________
void StChain::SetDefaultParameters()
{

//    Setters for flags and switches
}

//_____________________________________________________________________________
static void countTables(StMaker *maker,Int_t *numTotalAlloc, Int_t *numTotalUsed)
{
  // Count the allocated and used space but ht e tables of the maler's dataset
  // increament the input parameters:
  //
  //    Int_t *numTotalAlloc
  //    Int_t *numTotalUsed
  //
   const Float_t percent = 15.0;
   const Float_t Mega = 1024.0*1024.;
   Int_t nTotalAlloc = 0;
   Int_t nTotalUsed  = 0;
   St_DataSet *ds = maker->DataSet();
   St_DataSetIter nextSet(ds,0);
   St_DataSet *set = 0;
   Int_t isPrinted   = kFALSE;
   while (set =  (St_DataSet *)nextSet()) {
     if (!set->InheritsFrom("St_Table")) continue;
     St_Table *tab = (St_Table *)set;
     Int_t nAlloc  =  tab->GetTableSize();
     Int_t nUsed   =  tab->GetNRows();
     Int_t nSize   =  tab->GetRowSize();
     nTotalAlloc  += nAlloc*nSize;
     nTotalUsed   += nUsed*nSize;
     Float_t wastePercent = 0;
     if (nAlloc > 0) wastePercent = 100*(1.0-Float_t(nUsed)/Float_t(nAlloc));
     if ( wastePercent > percent) {
        if (!isPrinted) {
            isPrinted = kTRUE;
            cout << " --------  Statistics of tables \"wasted\" > " 
                 <<  percent << "% of the allocated by maker < " << maker->GetName() << " > ------------"
                 << endl;
        }
        cout << "Table: "<< setw(25) << tab->GetName()
                                    << ": Allocated = "  << setw(6) << nAlloc 
                                    << " rows : Used = " << setw(6) << nUsed << " rows : Wasted: " 
                                    << wastePercent << "% space"
                                    << endl;

     }
   }
   if (nTotalAlloc > 0 && isPrinted) {
      Float_t totalWastePercent = 100*(1-Float_t(nTotalUsed)/Float_t(nTotalAlloc));
      if ( totalWastePercent  >  percent )
        cout << "Maker: "      << setw(10) << maker->GetName() << " : " 
            << "Allocated = " << setw(8)  << nTotalAlloc/Mega << " Mbytes : " 
            << "Used = "      << setw(8)  << nTotalUsed/Mega  << " Mbytes : " 
            << "Wasted: " << totalWastePercent << "% of the space"
            << endl
            << " -----------------------" << endl << endl;
   }     
   *numTotalAlloc += nTotalAlloc;
   *numTotalUsed  += nTotalUsed;
} 

//_____________________________________________________________________________
static void printAlloc(const Char_t *chain, Int_t nTotalAlloc, Int_t nTotalUsed)
{
  const Float_t Mega = 1024.0*1024.;
  const Float_t percent = 15.0;
  if (nTotalAlloc > 0) 
  {
     Float_t totalWastePercent = 100*(1-Float_t(nTotalUsed)/Float_t(nTotalAlloc));
//     if ( totalWastePercent  >  percent )
      cout << " ------------------------------------------------" << endl
           << "Total chain: "<< setw(10) << chain  << " : " 
           << "Allocated = " << setw(8)  << nTotalAlloc/Mega  << " Mbytes : " 
           << "Used = "      << setw(8)  << nTotalUsed/Mega  << " Mbytes : " 
           << "Wasted: "     << totalWastePercent << "% of the space"
           << endl
           << " ------------------------------------------------" << endl << endl;
  }     
}

//_____________________________________________________________________________
Int_t StChain::Make(Int_t i)
{
// Create event 
   //   St_DataSetIter nextDataSet(m_DataSet);
   //   nextDataSet.Cd(gStChain->GetName());
//   Loop on all makers
   Int_t ret = kStOK;
   TIter nextMaker(m_Makers);
   StMaker *maker;
   Int_t numTotalAlloc=0;
   Int_t numTotalUsed= 0;
   SetEvent(i);
   while ((maker = (StMaker*)nextMaker())) {
     // Create the new DataSet for each new type of the maker if any
     const Char_t *makertype = maker->GetTitle();
     // Test the special case
     St_DataSet *makerset = maker->DataSet();
     if (!makerset){
       const Char_t *name = gSystem->BaseName(makertype);
       makerset = new St_DataSet(name);
       maker->SetDataSet(makerset);
     }
  // Call Maker
     StartMaker(maker);
     ret = maker->Make();
     countTables(maker,&numTotalAlloc, &numTotalUsed);
     EndMaker(maker,ret);
     
     if (gStChain->Debug()) printf("%s %i\n",maker->GetName(),ret);
     if (ret) break; 
     if (gStChain->Debug()) m_DataSet->ls(2);
   }
   printAlloc(GetName(), numTotalAlloc, numTotalUsed);
   return ret;
}

//_____________________________________________________________________________
void StChain::FillClone()
{
   // Fill Makers fruits clones
   
   TIter next(m_Makers);
   StMaker *maker;
   while ((maker = (StMaker*)next())) {
      maker->FillClone();
   }
}

//_____________________________________________________________________________
Int_t StChain::Finish()
{
//    Terminate a run
//   place to make operations on histograms, normalization,etc.
   int nerr = 0;

   TIter next(m_Makers);
   StMaker *maker;
   Double_t totalCpuTime = 0;
   Double_t totalRealTime = 0;
   while ((maker = (StMaker*)next())) {
      if ( maker->Finish() ) nerr++;
      maker->PrintTimer();
      totalCpuTime  += maker->CpuTime();
      totalRealTime += maker->RealTime();
   }
   Printf("------------------------------------------------------------------");
   Printf("%-10s: Real Time = %6.2f seconds Cpu Time = %6.2f seconds"
                               ,GetName(),totalRealTime,totalCpuTime);
   Printf("------------------------------------------------------------------");
   // Print realtive time
   if (totalCpuTime && totalRealTime) {
     next.Reset();
     while ((maker = (StMaker*)next())) {
        Printf("%-10s: Real Time = %5.1f %%        Cpu Time = %5.1f %% "
               ,maker->GetName()
               ,100*maker->RealTime()/totalRealTime
               ,100*maker->CpuTime()/totalCpuTime);
     }
     Printf("------------------------------------------------------------------");
   }
   return nerr;
}
//_____________________________________________________________________________
void StChain::StartMaker(StMaker *mk)
{
  if (mk) mk->StartTimer();
}
//_____________________________________________________________________________
void StChain::EndMaker(StMaker *mk,Int_t iret)
{
  if (mk) mk->StopTimer();

}
//_____________________________________________________________________________
void StChain::Fatal(int Ierr, const char *com)
{
   printf("StChain::Fatal: Error %d %s\n",Ierr,com);
   fflush(stdout);
   exit(Ierr);
}
//_____________________________________________________________________________
void StChain::SortDown(Int_t n1, Float_t *a, Int_t *index, Bool_t down)
{
//  sort the n1 elements of array a.
//  In output the array index contains the indices of the sorted array.
//  if down is false sort in increasing order (default is decreasing order)
//   This is a translation of the CERNLIB routine sortzv (M101)
//   based on the quicksort algorithm

   Int_t i,i1,n,i2,i3,i33,i222,iswap,n2;
   Int_t i22 = 0;
   Float_t ai;
   n = n1;
   if (n <= 0) return;
   if (n == 1) {index[0] = 0; return;}
   for (i=0;i<n;i++) index[i] = i+1;
   for (i1=2;i1<=n;i1++) {
      i3 = i1;
      i33 = index[i3-1];
      ai  = a[i33-1];
      while(1) {
         i2 = i3/2;
         if (i2 <= 0) break;
         i22 = index[i2-1];
         if (ai <= a[i22-1]) break;
         index[i3-1] = i22;
         i3 = i2;
      }
      index[i3-1] = i33;
   }

   while(1) {
      i3 = index[n-1];
      index[n-1] = index[0];
      ai = a[i3-1];
      n--;
      if(n-1 < 0) {index[0] = i3; break;}
      i1 = 1;
      while(2) {
         i2 = i1+i1;
         if (i2 <= n) i22 = index[i2-1];
         if (i2-n > 0) {index[i1-1] = i3; break;}
         if (i2-n < 0) {
            i222 = index[i2];
            if (a[i22-1] - a[i222-1] < 0) {
                i2++;
                i22 = i222;
            }
         }
         if (ai - a[i22-1] > 0) {index[i1-1] = i3; break;}
         index[i1-1] = i22;
         i1 = i2;
      }
   }
   if (!down) return;
   n2 = n1/2;
   for (i=0;i<n1;i++) index[i]--;
   for (i=0;i<n2;i++) {
      iswap         = index[i];
      index[i]      = index[n1-i-1];
      index[n1-i-1] = iswap;
   }
}
//______________________________________________________________________________
#ifdef WIN32
void StChain::Streamer(TBuffer &R__b)
{
   // Stream an object of class StChain.

   if (R__b.IsReading()) {
      R__b.ReadVersion(); //  Version_t R__v = R__b.ReadVersion();
      StMaker::Streamer(R__b);
      if (!gStChain) gStChain = this;
      gROOT->GetListOfBrowsables()->Add(this,"StChain");
      R__b >> m_Version;
      R__b >> m_VersionDate;
      R__b >> m_Run;
      R__b >> m_Event;
      R__b >> m_Mode;
      m_Tree = (TTree*)gDirectory->Get("T");
      R__b >> m_Makers;
 //     m_HistBrowser.Streamer(R__b);
   } else {
      R__b.WriteVersion(StChain::IsA());
      StMaker::Streamer(R__b);
      R__b << m_Version;
      R__b << m_VersionDate;
      R__b << m_Run;
      R__b << m_Event;
      R__b << m_Mode;
      R__b << m_Tree;

     // replace list of makers with the list of its name
     // instead of  R__b << m_Makers;

      TIter next(m_Makers);
      TList makerNames;
      StMaker *maker = 0;
      while (maker = (StMaker *)next() )
         makerNames.Add(new TObjString(maker->GetName()));
      makerNames.Streamer();
      makeNames.Delete();
      
//      m_HistBrowser.Streamer(R__b);
   }
}
#endif
//_____________________________________________________________________________
void StChain::ResetPoints()
{
}
//_____________________________________________________________________________
void StChain::ResetHits()
{
}
//_____________________________________________________________________________
void StChain::CleanDetectors()
{
}
//_____________________________________________________________________________
TGeometry *StChain::GetGeometry()
{
  return 0;
}
