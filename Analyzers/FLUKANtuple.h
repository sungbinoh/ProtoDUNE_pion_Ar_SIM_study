#ifndef FLUKANtuple_h
#define FLUKANtuple_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>

// Header file for the classes stored in the TTree if any.
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

const Int_t MAXHits = 400000;
const Int_t MAXParticles = 500000;

using namespace std;

class FLUKANtuple {
public :

   FLUKANtuple();
   virtual ~FLUKANtuple();

   virtual void Init_FLUKA(TChain *ch);

   // Declaration of leaf types
   Int_t           RunNum;
   Int_t           EveNum;
   Int_t           NIncHits;
   Int_t           IdInc[MAXHits];   //[NIncHits]
   Int_t           IdParInc[MAXHits];   //[NIncHits]
   Int_t           TrInc[MAXHits];   //[NIncHits]
   Int_t           ParTrInc[MAXHits];   //[NIncHits]
   Int_t           IntParInc[MAXHits];   //[NIncHits]
   Int_t           RegInc[MAXHits];   //[NIncHits]
   Float_t         PosInc[MAXHits][3];   //[NIncHits]
   Float_t         PInc[MAXHits][5];   //[NIncHits]
   Float_t         TimeInc[MAXHits];   //[NIncHits]
   Int_t           NCalHits;
   Int_t           WhCal[MAXHits];   //[NCalHits]
   Float_t         PosCal[MAXHits][3];   //[NCalHits]
   Float_t         EneCal[MAXHits];   //[NCalHits]
   Float_t         EqCal[MAXHits];   //[NCalHits]
   Int_t           NCHitPiz;
   Int_t           WhCPiz[MAXHits];   //[NCHitPiz]
   Float_t         PosCPiz[MAXHits][3];   //[NCHitPiz]
   Float_t         EneCPiz[MAXHits];   //[NCHitPiz]
   Float_t         EqCPiz[MAXHits];   //[NCHitPiz]
   Int_t           NIneHits;
   Int_t           TypeIne[MAXHits];   //[NIneHits]
   Int_t           IdIne[MAXHits];   //[NIneHits]
   Int_t           TrIne[MAXHits];   //[NIneHits]
   Int_t           IdParIne[MAXHits];   //[NIneHits]
   Int_t           IntParIne[MAXHits];   //[NIncHits]
   Float_t         PIne[MAXHits][5];   //[NIneHits]
   Float_t         PosIne[MAXHits][3];   //[NIneHits]
   Float_t         TimeIne[MAXHits];   //[NIneHits]
   Int_t           NSecIne[MAXHits];   //[NIneHits]
   Int_t           FirstSec[MAXHits];   //[NIneHits]
   Int_t           NTIneSec;
   Int_t           HitSecIne[MAXParticles];   //[NTIneSec]
   Int_t           IdSecIne[MAXParticles];   //[NTIneSec]
   Int_t           TrSecIne[MAXParticles];   //[NTIneSec]
   Float_t         PSec[MAXParticles][5];   //[NTIneSec]

   // List of branches
   TBranch        *b_RunNum;   //!
   TBranch        *b_EveNum;   //!
   TBranch        *b_NIncHits;   //!
   TBranch        *b_IdInc;   //!
   TBranch        *b_IdParInc;   //!
   TBranch        *b_TrInc;   //!
   TBranch        *b_ParTrInc;   //!
   TBranch        *b_IntParInc;   //!
   TBranch        *b_RegInc;   //!
   TBranch        *b_PosInc;   //!
   TBranch        *b_PInc;   //!
   TBranch        *b_TimeInc;   //!
   TBranch        *b_NCalHits;   //!
   TBranch        *b_WhCal;   //!
   TBranch        *b_PosCal;   //!
   TBranch        *b_EneCal;   //!
   TBranch        *b_EqCal;   //!
   TBranch        *b_NCHitPiz;   //!
   TBranch        *b_WhCPiz;   //!
   TBranch        *b_PosCPiz;   //!
   TBranch        *b_EneCPiz;   //!
   TBranch        *b_EqCPiz;   //!
   TBranch        *b_NIneHits;   //!
   TBranch        *b_TypeIne;   //!
   TBranch        *b_IdIne;   //!
   TBranch        *b_TrIne;   //!
   TBranch        *b_IdParIne;   //!
   TBranch        *b_IntParIne;   //!
   TBranch        *b_PIne;   //!
   TBranch        *b_PosIne;   //!
   TBranch        *b_TimeIne;   //!
   TBranch        *b_NSecIne;   //!
   TBranch        *b_FirstSec;   //!
   TBranch        *b_NTIneSec;   //!
   TBranch        *b_HitSecIne;   //!
   TBranch        *b_IdSecIne;   //!
   TBranch        *b_TrSecIne;   //!
   TBranch        *b_PSec;   //!

};

#endif
