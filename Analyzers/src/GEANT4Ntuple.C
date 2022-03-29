#define GEANT4Ntuple_cxx
#include "GEANT4Ntuple.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

GEANT4Ntuple::GEANT4Ntuple(){
}

GEANT4Ntuple::~GEANT4Ntuple(){
}

void GEANT4Ntuple::Init_GEANT4(TChain *ch)
{

  ch->SetMakeClass(0);
  // Set object pointer                                                                                                                                                                                            
  PDGcode = 0;
  interType = 0;
  targetZ = 0;
  massPart = 0;
  X = 0;
  Y = 0;
  Z = 0;
  Px = 0;
  Py = 0;
  Pz = 0;
  E = 0;

  // Set branch addresses and branch pointers                                                                                                                                                                      
  ch->SetBranchAddress("EventID", &EventID, &b_EventID);
  ch->SetBranchAddress("PDGcode", &PDGcode, &b_PDGcode);
  ch->SetBranchAddress("interType", &interType, &b_interType);
  ch->SetBranchAddress("targetZ", &targetZ, &b_targetZ);
  ch->SetBranchAddress("massPart", &massPart, &b_massPart);
  ch->SetBranchAddress("X", &X, &b_X);
  ch->SetBranchAddress("Y", &Y, &b_Y);
  ch->SetBranchAddress("Z", &Z, &b_Z);
  ch->SetBranchAddress("Px", &Px, &b_Px);
  ch->SetBranchAddress("Py", &Py, &b_Py);
  ch->SetBranchAddress("Pz", &Pz, &b_Pz);
  ch->SetBranchAddress("E", &E, &b_E);

}
