#include "PionAnalyzer.h"
#include "GEANT4Ntuple.h"

void PionAnalyzer::initializeAnalyzer(){
  
  cout << "[[PionAnalyzer::initializeAnalyzer]] Simulator : " << Simulator << endl;
  debug_mode = true;
  debug_mode = false;

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

  FillHist("Number_of_particles", particles_all.size(), 1., 40, 0., 40.);
  FillHist("PID_of_first_particle", particles_all.at(0).PID(), 1., 20, 200., 220.);
  FillHist("Atomic_number_of_second_particle", GetAtomicNumber(particles_all.at(1).PID()), 1., 25, 0., 25.);
  FillHist("P_1st_particle", particles_all.at(0).P(), 1., 2000, 0., 2000.);
  
  if(debug_mode){
    for(unsigned int i = 0; i < particles_all.size(); i++){
      
      int current_PID = particles_all.at(i).PID();
      cout << "particles_all.at(" << i << ").PID() : " << current_PID << ", atomic number : " << GetAtomicNumber(current_PID)<< ", atomic mass : " << GetAtomicMass(current_PID) << endl;
    }
    cout << "particles_all.at(0).P() : " << particles_all.at(0).P() << endl;

  }
  if(debug_mode) cout << "-------" << endl;
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


