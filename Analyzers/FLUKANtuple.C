#define FLUKANtuple_cxx
#include "FLUKANtuple.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

FLUKANtuple::FLUKANtuple(){
}

FLUKANtuple::~FLUKANtuple(){
}

void FLUKANtuple::Init_FLUKA(TChain *ch)
{

  ch->SetMakeClass(0);
  // Set object pointer
  
  ch->SetBranchAddress("RunNum", &RunNum, &b_RunNum);
  ch->SetBranchAddress("EveNum", &EveNum, &b_EveNum);
  ch->SetBranchAddress("NIncHits", &NIncHits, &b_NIncHits);
  ch->SetBranchAddress("IdInc", IdInc, &b_IdInc);
  ch->SetBranchAddress("IdParInc", IdParInc, &b_IdParInc);
  ch->SetBranchAddress("TrInc", TrInc, &b_TrInc);
  ch->SetBranchAddress("ParTrInc", ParTrInc, &b_ParTrInc);
  ch->SetBranchAddress("IntParInc", IntParInc, &b_IntParInc);
  ch->SetBranchAddress("RegInc", RegInc, &b_RegInc);
  ch->SetBranchAddress("PosInc", PosInc, &b_PosInc);
  ch->SetBranchAddress("PInc", PInc, &b_PInc);
  ch->SetBranchAddress("TimeInc", TimeInc, &b_TimeInc);
  ch->SetBranchAddress("NCalHits", &NCalHits, &b_NCalHits);
  ch->SetBranchAddress("WhCal", &WhCal, &b_WhCal);
  ch->SetBranchAddress("PosCal", &PosCal, &b_PosCal);
  ch->SetBranchAddress("EneCal", &EneCal, &b_EneCal);
  ch->SetBranchAddress("EqCal", &EqCal, &b_EqCal);
  ch->SetBranchAddress("NCHitPiz", &NCHitPiz, &b_NCHitPiz);
  ch->SetBranchAddress("WhCPiz", &WhCPiz, &b_WhCPiz);
  ch->SetBranchAddress("PosCPiz", &PosCPiz, &b_PosCPiz);
  ch->SetBranchAddress("EneCPiz", &EneCPiz, &b_EneCPiz);
  ch->SetBranchAddress("EqCPiz", &EqCPiz, &b_EqCPiz);
  ch->SetBranchAddress("NIneHits", &NIneHits, &b_NIneHits);
  ch->SetBranchAddress("TypeIne", TypeIne, &b_TypeIne);
  ch->SetBranchAddress("IdIne", IdIne, &b_IdIne);
  ch->SetBranchAddress("TrIne", TrIne, &b_TrIne);
  ch->SetBranchAddress("IdParIne", IdParIne, &b_IdParIne);
  ch->SetBranchAddress("IntParIne", IntParIne, &b_IntParIne);
  ch->SetBranchAddress("PIne", PIne, &b_PIne);
  ch->SetBranchAddress("PosIne", PosIne, &b_PosIne);
  ch->SetBranchAddress("TimeIne", TimeIne, &b_TimeIne);
  ch->SetBranchAddress("NSecIne", NSecIne, &b_NSecIne);
  ch->SetBranchAddress("FirstSec", FirstSec, &b_FirstSec);
  ch->SetBranchAddress("NTIneSec", &NTIneSec, &b_NTIneSec);
  ch->SetBranchAddress("HitSecIne", HitSecIne, &b_HitSecIne);
  ch->SetBranchAddress("IdSecIne", IdSecIne, &b_IdSecIne);
  ch->SetBranchAddress("TrSecIne", TrSecIne, &b_TrSecIne);
  ch->SetBranchAddress("PSec", PSec, &b_PSec);

}
