#include "PionAnalyzer.h"
#include "GEANT4Ntuple.h"

void PionAnalyzer::initializeAnalyzer(){
  
  cout << "[[PionAnalyzer::initializeAnalyzer]] Simulator : " << Simulator << endl;
  
}

void PionAnalyzer::executeEvent(){

  std::vector<Gen> particles_all;
  if(Simulator.Contains("GEANT")){
    particles_all = GetAllParticles_GEANT4();
  }
  else if(Simulator.Contains("FLUKA")){

  }
  else return;

  //cout << "[[PionAnalyzer::executeEvent]] particles_all.size() : " << particles_all.size() << endl;

  FillHist("Number_of_particles", particles_all.size(), 1., 40, -0.5, 39.5);

}
void PionAnalyzer::Init(){
  
  cout << "Let initiallize!" << endl;
  if(Simulator.Contains("GEANT")){
    Init_GEANT4(fChain);
  }
  else if(Simulator.Contains("FLUKA")){

  }
  else return;

}

PionAnalyzer::PionAnalyzer(){

}

PionAnalyzer::~PionAnalyzer(){

}


