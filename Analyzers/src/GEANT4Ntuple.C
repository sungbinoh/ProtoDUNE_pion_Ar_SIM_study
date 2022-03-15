#define GEANT4Ntuple_cxx
#include "GEANT4Ntuple.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
/*
void GEANT4Ntuple::Loop(){
  
  Long64_t nentries = fChain->GetEntriesFast();
  if(MaxEvent>0){
    nentries = std::min(nentries,MaxEvent);
  }

  //==== Before starting the loop, print setups
  cout << "[GEANT4Ntuple::Loop] MaxEvent = " << MaxEvent << endl;
  cout << "[GEANT4Ntuple::Loop] NSkipEvent = " << NSkipEvent << endl;
  cout << "[GEANT4Ntuple::Loop] LogEvery = " << LogEvery << endl;
  cout << "[GEANT4Ntuple::Loop] MCSample = " << MCSample << endl;
  cout << "[GEANT4Ntuple::Loop] Simulator = " << Simulator << endl;
  cout << "[GEANT4Ntuple::Loop] Userflags = {" << endl;
  for(unsigned int i=0; i<Userflags.size(); i++){
    cout << "[GEANT4Ntuple::Loop]   \"" << Userflags.at(i) << "\"," << endl;
  }
  cout << "[GEANT4Ntuple::Loop] }" << endl;


  cout << "[GEANT4Ntuple::Loop] Event Loop Started " << printcurrunttime() << endl;
  
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
  
    if(jentry<NSkipEvent){
      //cout << "[GEANT4Ntuple::Loop] Skipping " << jentry << "'th event" << endl; 
      //exit(EXIT_FAILURE);
       continue;
    }

    if(jentry%LogEvery==0){
      cout << "[GEANT4Ntuple::Loop RUNNING] " << jentry << "/" << nentries << " ("<<100.*jentry/nentries<<" %) @ " << printcurrunttime() << endl;
    }

    fChain->GetEntry(jentry);

    executeEvent();
  }

  cout << "[GEANT4Ntuple::Loop] LOOP END " << printcurrunttime() << endl;

}
*/

//==== Basic
GEANT4Ntuple::GEANT4Ntuple(){
}

GEANT4Ntuple::~GEANT4Ntuple()
{
  if (!fChain) return;
  delete fChain->GetCurrentFile();
  cout << "[GEANT4Ntuple::~GEANT4Ntuple] JOB FINISHED " << printcurrunttime() << endl;
}

Int_t GEANT4Ntuple::GetEntry(Long64_t entry)
{
  // Read contents of entry  
  if (!fChain) return 0;
  return fChain->GetEntry(entry);
}

void GEANT4Ntuple::Init(TChain *ch)
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
  fChain->SetBranchAddress("EventID", &EventID, &b_EventID);
  fChain->SetBranchAddress("PDGcode", &PDGcode, &b_PDGcode);
  fChain->SetBranchAddress("interType", &interType, &b_interType);
  fChain->SetBranchAddress("targetZ", &targetZ, &b_targetZ);
  fChain->SetBranchAddress("massPart", &massPart, &b_massPart);
  fChain->SetBranchAddress("X", &X, &b_X);
  fChain->SetBranchAddress("Y", &Y, &b_Y);
  fChain->SetBranchAddress("Z", &Z, &b_Z);
  fChain->SetBranchAddress("Px", &Px, &b_Px);
  fChain->SetBranchAddress("Py", &Py, &b_Py);
  fChain->SetBranchAddress("Pz", &Pz, &b_Pz);
  fChain->SetBranchAddress("E", &E, &b_E);

}
