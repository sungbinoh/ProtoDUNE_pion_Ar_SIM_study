#ifndef GEANT4Ntuple_h
#define GEANT4Ntuple_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>

// Header file for the classes stored in the TTree if any.
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

// Fixed size dimensions of array or collections stored in the TTree if any.

using namespace std;

class GEANT4Ntuple {
public :

   GEANT4Ntuple();
   virtual ~GEANT4Ntuple();

   //virtual Int_t GetEntry(Long64_t entry);
   /*
   virtual void SetTreeName(TString tname){
     fChain = new TChain(tname);
   }

   virtual void AddFile(TString filename){
     fChain->Add(filename);
   }
   */
   virtual void Init_GEANT4(TChain *ch);
   //virtual void Loop();

   //TChain *fChain;

   // Declaration of leaf types
   Int_t           EventID;
   vector<int>     *PDGcode;
   vector<int>     *interType;
   vector<int>     *targetZ;
   vector<double>  *massPart;
   vector<double>  *X;
   vector<double>  *Y;
   vector<double>  *Z;
   vector<double>  *Px;
   vector<double>  *Py;
   vector<double>  *Pz;
   vector<double>  *E;

   // List of branches
   TBranch        *b_EventID;   //!
   TBranch        *b_PDGcode;   //!
   TBranch        *b_interType;   //!
   TBranch        *b_targetZ;   //!
   TBranch        *b_massPart;   //!
   TBranch        *b_X;   //!
   TBranch        *b_Y;   //!
   TBranch        *b_Z;   //!
   TBranch        *b_Px;   //!
   TBranch        *b_Py;   //!
   TBranch        *b_Pz;   //!
   TBranch        *b_E;   //!

};

#endif
