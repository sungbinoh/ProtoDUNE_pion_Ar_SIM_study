#include "PionAnalyzer.h"
#include "GEANT4Ntuple.h"

void PionAnalyzer::initializeAnalyzer(){
  
  cout << "[[PionAnalyzer::initializeAnalyzer]] Simulator : " << Simulator << endl;
  debug_mode = true;
  debug_mode = false;

}

void PionAnalyzer::executeEvent(){

  // == Collect all particles in an event
  std::vector<Gen> particles_all;
  if(Simulator.Contains("GEANT")){
    particles_all = GetAllParticles_GEANT4();
  }
  else if(Simulator.Contains("FLUKA")){

  }
  else return;

  // == Collect particles depending on PDG ID. Cut on momuntum : .P() > 0.
  std::vector<Gen> piplus_all = GetPiplus(particles_all, 0.);
  std::vector<Gen> protons_all = GetProtons(particles_all, 0.);
  std::vector<Gen> pizeros_all = GetPizeros(particles_all, 0.);
  std::vector<Gen> bkg_particles = GetBkgParticles(particles_all, 100.); // == pi-, kaon, muon with P > 100 MeV
  std::vector<Gen> nuclei = GetNuclei(particles_all);
  // Momentum smearing only
  //std::vector<Gen> piplus_mom_smear = GetPiplus(smear->SmearOutParticles(piplus_all, 1), 100.);

  //if(debug_mode) cout << "[[PionAnalyzer::executeEvent]] particles_all.size() : " << particles_all.size() << endl;

  // == Fill out general plots
  FillHist("N_particles", particles_all.size(), 1., 40, 0., 40.);
  FillHist("PID_of_first_particle", particles_all.at(0).PID(), 1., 20, 200., 220.);
  FillHist("Atomic_number_of_second_particle", GetAtomicNumber(particles_all.at(1).PID()), 1., 25, 0., 25.);
  FillHist("P_1st_particle", particles_all.at(0).P(), 1., 2000, 0., 2000.);
  FillHist("N_piplus", piplus_all.size(), 1., 10, 0., 10.);
  FillHist("N_protons", protons_all.size(), 1., 10, 0., 10.);
  FillHist("N_pizeros", pizeros_all.size(), 1., 10, 0., 10.);
  FillHist("N_bkg_particles", bkg_particles.size(), 1., 20, 0., 20.);

  
  // == Print out for debugging
  //if(debug_mode) cout << "-------" << endl;
  if(debug_mode){
    for(unsigned int i = 0; i < particles_all.size(); i++){
      
      int current_PID = particles_all.at(i).PID();
      Gen this_particle = particles_all.at(i);
      //cout << "[[PionAnalyzer::executeEvent]] particles_all.at(" << i << ").PID() : " << current_PID << ", atomic number : " << GetAtomicNumber(current_PID)<< ", atomic mass : " << GetAtomicMass(current_PID) << ", interType : " << this_particle.interType() << ", (X,Y,Z) = (" << this_particle.X() << ", " << this_particle.Y() << ", " << this_particle.Z() << "), P : " << this_particle.P() << endl;
    }
    //cout << "[[PionAnalyzer::executeEvent]] particles_all.at(0).P() : " << particles_all.at(0).P() << endl;
  }
  
  // == Run for each smearBit
  TString smear_flags[8] = {"NONE", "P", "Theta", "P_Theta", "Phi", "P_Phi", "Phi_Theta", "All"};
  for(int i_smear=0; i_smear<8; i_smear++){
    executeEventWithVariables(i_smear, smear_flags[i_smear], particles_all, piplus_all, protons_all, pizeros_all, bkg_particles, nuclei);
  }
}

void PionAnalyzer::executeEventWithVariables(int smearBit, TString smear_flag, std::vector<Gen> particles_all, std::vector<Gen> piplus_all, std::vector<Gen> protons_all, std::vector<Gen> pizeros_all, std::vector<Gen> bkg_particles, std::vector<Gen> nuclei){

  // == Apply smearing & P cut (piplus > 100 MeV, proton > 300 MeV, pizero > 0 MeV)
  std::vector<Gen> piplus = GetPiplus(smear->SmearOutParticles(piplus_all, smearBit), 100.);
  std::vector<Gen> protons = GetProtons(smear->SmearOutParticles(protons_all, smearBit), 300.);
  std::vector<Gen> pizeros = GetPizeros(smear->SmearOutParticles(pizeros_all, smearBit), 0.);
  
  SR1(smearBit, smear_flag, particles_all, piplus, protons, pizeros, bkg_particles, nuclei);
  SR2(smearBit, smear_flag, particles_all, piplus, protons, pizeros, bkg_particles, nuclei);
  SR3(smearBit, smear_flag, particles_all, piplus, protons, pizeros, bkg_particles, nuclei);
  SR4(smearBit, smear_flag, particles_all, piplus, protons, pizeros, bkg_particles, nuclei);
}

void PionAnalyzer::SR1(int smearBit, TString smear_flag, std::vector<Gen> particles_all, std::vector<Gen> piplus, std::vector<Gen> protons, std::vector<Gen> pizeros, std::vector<Gen> bkg_particles, std::vector<Gen> nuclei){
  // == pi+ + 40Ar -> pi+ + p + 39Cl
  // == N(pi+) = 1, N(p) = 1, N(pi0) = 0, N(bkg) = 0
  bool pass_particle_multiplicities = piplus.size() == 2 && protons.size() == 1 && pizeros.size() == 0 && bkg_particles.size() == 0;
  if(!pass_particle_multiplicities) return;

  // -- Call atomic number and mass of residual nuclear
  int current_atomic_number = 0;
  int current_atomic_mass = 0;
  if(nuclei.size() > 0){
    current_atomic_number = GetAtomicNumber(nuclei.at(0).PID());
    current_atomic_mass = GetAtomicMass(nuclei.at(0).PID());
  }

  // -- Calculate residual mass
  Gen beam = particles_all.at(0);
  Particle Ar_target;
  Ar_target.SetPxPyPzE(0., 0., 0., 37225); //40Ar mass = 37.225 GeV, https://www.wolframalpha.com/input?i2d=true&i=Argon+40+mass+in+GeV 
  Particle residual_reco = beam + Ar_target - piplus.at(1) - protons.at(0);

  // -- Calculate additional variables
  double Q_square = 0., Q = 0., q0 = 0., y = 0., sqrt_s = 0.;
  Particle q_4vec = beam - piplus.at(1);
  Q_square = (-pow(q_4vec.E(), 2) + pow(q_4vec.P(), 2)) / 1000000.;
  Q = pow(Q_square, 0.5);
  q0 = q_4vec.E() / 1000.;
  y = (beam.E() - piplus.at(1).E()) / beam.E();
  sqrt_s = (beam + Ar_target).M();

  double P_balance_piplus_p = ( piplus.at(1).P() - protons.at(0).P() ) / piplus.at(1).P();
  double P_balance_beam_piplus = (beam.P() - piplus.at(1).P()) / beam.P();
  double P_balance_beam_p = (beam.P() - protons.at(0).P() ) / beam.P();

  // -- Draw plots
  TString region = "SR1_" + smear_flag;
  
  SR1_FillHist(region, "M_residual", residual_reco.M(), current_atomic_number, current_atomic_mass, 1., 200, 36200., 36400.);
  SR1_FillHist(region, "Q_square", Q_square, current_atomic_number, current_atomic_mass, 1., 200, 0., 2.0);
  SR1_FillHist(region, "q0", q0, current_atomic_number, current_atomic_mass, 1., 200, 0., 2.0);
  SR1_FillHist(region, "y", y, current_atomic_number, current_atomic_mass, 1., 100, 0., 1.0);
  SR1_FillHist(region, "P_balance_piplus_p", P_balance_piplus_p, current_atomic_number, current_atomic_mass, 1., 80, -3., 1.0);
  SR1_FillHist(region, "P_balance_beam_piplus", P_balance_beam_piplus, current_atomic_number, current_atomic_mass, 1., 80, -3., 1.0);
  SR1_FillHist(region, "P_balance_beam_p", P_balance_beam_p, current_atomic_number, current_atomic_mass, 1., 80, -3., 1.0);
  SR1_FillHist(region, "P_proton", protons.at(0).P(), current_atomic_number, current_atomic_mass, 1., 200, 0., 2000);
  SR1_FillHist(region, "P_piplus", piplus.at(1).P(), current_atomic_number, current_atomic_mass, 1., 200, 0., 2000);


  // -- Plots for all
  JSFillHist(region, "Z_" + region, current_atomic_number, 1., 20, 0., 20.);
  JSFillHist(region, "M_residual_VS_P_proton_" + region, residual_reco.M(), protons.at(0).P(), 1., 140, 36240., 36380., 70, 300., 1000.);

  // == Plost for background
  if(current_atomic_number > 13){
    if(current_atomic_number != 17){
      
    }    
    else{ // -- Plots for Z = 17 bkg
      if(current_atomic_mass != 39){
	int N_neutron = GetNPID(particles_all, 2112);
	int N_gamma = GetNPID(particles_all, 22);
	JSFillHist(region, "N_neutron_" + TString::Itoa(current_atomic_number, 10) + "_" + region, N_neutron, 1., 20, 0., 20.);
	JSFillHist(region, "N_neutron_VS_N_gamma_" + TString::Itoa(current_atomic_number, 10) + "_" + region, N_neutron, N_gamma, 1., 8, 0., 8., 8, 0., 8.);
	JSFillHist(region, "M_residual_" + TString::Itoa(current_atomic_number, 10) + "_Nn_" + TString::Itoa(N_neutron, 10) + "_" + region, residual_reco.M(), 1., 200, 36200., 36400.);
	JSFillHist(region, "M_residual_VS_P_proton_" + TString::Itoa(current_atomic_number, 10) + "_" + region, residual_reco.M(), protons.at(0).P(), 1., 140, 36240., 36380., 70, 300., 1000.);
	JSFillHist(region, "M_residual_VS_P_piplus_" + TString::Itoa(current_atomic_number, 10) + "_" + region, residual_reco.M(), piplus.at(1).P(), 1., 140, 36240., 36380., 90, 100., 1000.);
	JSFillHist(region, "Q_square_VS_P_balance_piplus_p_" + TString::Itoa(current_atomic_number, 10) + "_" + region, Q_square, P_balance_piplus_p, 1., 200, 0., 2.0, 80, -3., 1.);
	JSFillHist(region, "Q_square_VS_P_balance_beam_piplus_" + TString::Itoa(current_atomic_number, 10) + "_" + region, Q_square, P_balance_beam_piplus, 1., 200, 0., 2.0, 80, -3., 1.);
	JSFillHist(region, "Q_square_VS_P_balance_beam_p_" + TString::Itoa(current_atomic_number, 10) + "_" + region, Q_square, P_balance_beam_p, 1., 200, 0., 2.0, 80, -3., 1.);
	JSFillHist(region, "P_balance_beam_piplus_VS_P_balance_beam_p_" + TString::Itoa(current_atomic_number, 10) + "_" + region, P_balance_beam_piplus, P_balance_beam_p, 1., 80, -3., 1.0, 80, -3., 1.);
      }
    }
  }  
  else{

  }

  // == Plots for Signal
  if(current_atomic_number == 17 && current_atomic_mass == 39){
    JSFillHist(region, "Z_Signal_" + region, current_atomic_number, 1., 20, 0., 20.);
    int N_gamma = GetNPID(particles_all, 22);
    JSFillHist(region, "N_gamma_Signal_" + region, N_gamma, 1., 20, 0., 20.);
    JSFillHist(region, "Q_square_Ngamma_" + TString::Itoa(N_gamma, 10) + "_" + region, Q_square, 1., 200, 0., 2.0);
    JSFillHist(region, "Q_square_VS_y_" + region, Q_square, y, 1., 200, 0., 2.0, 100, 0., 1.0);
    JSFillHist(region, "M_residual_VS_Q_square_" + region, residual_reco.M(), Q_square, 1., 200, 0., 2.0, 100, 0., 1.0);
    JSFillHist(region, "M_residual_VS_P_proton_Signal_" + region, residual_reco.M(), protons.at(0).P(), 1., 140, 36240., 36380., 70, 300., 1000.);
    JSFillHist(region, "M_residual_VS_P_piplus_Signal_" + region, residual_reco.M(), piplus.at(1).P(), 1., 140, 36240., 36380., 90, 100., 1000.);
    JSFillHist(region, "Q_square_VS_P_balance_piplus_p_Signal_" + region, Q_square, P_balance_piplus_p, 1., 200, 0., 2.0, 80, -3., 1.);
    JSFillHist(region, "Q_square_VS_P_balance_beam_piplus_Signal_" + region, Q_square, P_balance_beam_piplus, 1., 200, 0., 2.0, 80, -3., 1.);
    JSFillHist(region, "Q_square_VS_P_balance_beam_p_Signal_" + region, Q_square, P_balance_beam_p, 1., 200, 0., 2.0, 80, -3., 1.);
    JSFillHist(region, "P_balance_beam_piplus_VS_P_balance_beam_p_Signal_" + region, P_balance_beam_piplus, P_balance_beam_p, 1., 80, -3., 1.0, 80, -3., 1.);
  }

  if(debug_mode && smear_flag == "NONE" && current_atomic_number == 17){
    cout << "[[PionAnalyzer::SR1]] Ar in SR1" << endl;
    for(unsigned int i = 0; i < particles_all.size(); i++){
      Gen this_particle = particles_all.at(i);
      cout << "[[PionAnalyzer::executeEvent]] particles_all.at(" << i << ").PID() : " << this_particle.PID() << "), P : " << this_particle.P() << endl;
    }
  }
  
  //if(debug_mode && smear_flag == "NONE") cout << "[[PionAnalyzer::SR1]] residual_reco.M() : " << residual_reco.M() << endl;
  if(debug_mode && smear_flag == "NONE") cout << "[[PionAnalyzer::SR1]] Q_square : " << Q_square << ", Q : " << Q << endl;
}

void PionAnalyzer::SR2(int smearBit, TString smear_flag, std::vector<Gen> particles_all, std::vector<Gen> piplus, std::vector<Gen> protons, std::vector<Gen> pizeros, std::vector<Gen> bkg_particles, std::vector<Gen> nuclei){
  // == pi+ + 40Ar -> pi+ + p + p + 38S
  // == N(pi+) = 1, N(p) = 2, N(pi0) = 0, N(bkg) = 0
  bool pass_particle_multiplicities = piplus.size() == 2 && protons.size() == 2 && pizeros.size() == 0 && bkg_particles.size() == 0;
  if(!pass_particle_multiplicities) return;

  // -- Call atomic number and mass of residual nuclear
  int current_atomic_number = 0;
  int current_atomic_mass = 0;
  if(nuclei.size() > 0){
    current_atomic_number = GetAtomicNumber(nuclei.at(0).PID());
    current_atomic_mass = GetAtomicMass(nuclei.at(0).PID());
  }

  // -- Calculate residual mass
  Gen beam = particles_all.at(0);
  Particle Ar_target;
  Ar_target.SetPxPyPzE(0., 0., 0., 37225); //40Ar mass = 37.225 GeV, https://www.wolframalpha.com/input?i2d=true&i=Argon+40+mass+in+GeV
  Particle residual_reco = beam + Ar_target - piplus.at(1) - protons.at(0) - protons.at(1);
  Particle residual_reco_Mneutron;
  Particle iso_sym_neutron = protons.at(1);
  iso_sym_neutron.SetPtEtaPhiM(iso_sym_neutron.Pt(), iso_sym_neutron.Eta(), iso_sym_neutron.Phi(), M_neutron  * 1000);
  residual_reco_Mneutron = beam + Ar_target - piplus.at(1) - protons.at(0) - iso_sym_neutron;

  TString region = "SR2_" + smear_flag;

  SR1_FillHist(region, "M_residual", residual_reco.M(), current_atomic_number, current_atomic_mass, 1., 200, 36200., 36400.);
  SR1_FillHist(region, "M_residual_Mneutron", residual_reco_Mneutron.M(), current_atomic_number, current_atomic_mass, 1., 200, 36200., 36400.);


}

void PionAnalyzer::SR3(int smearBit, TString smear_flag, std::vector<Gen> particles_all, std::vector<Gen> piplus, std::vector<Gen> protons, std::vector<Gen> pizeros, std::vector<Gen> bkg_particles, std::vector<Gen> nuclei){
  // == pi+ + 40Ar -> pi0 + p + 39Ar
  // == N(pi+) = 0, N(p) = 1, N(pi0) = 1, N(bkg) = 0
  bool pass_particle_multiplicities = piplus.size() == 1 && protons.size() == 1 && pizeros.size() == 1 && bkg_particles.size() == 0;
  if(!pass_particle_multiplicities) return;

}

void PionAnalyzer::SR4(int smearBit, TString smear_flag, std::vector<Gen> particles_all, std::vector<Gen> piplus, std::vector<Gen> protons, std::vector<Gen> pizeros, std::vector<Gen> bkg_particles, std::vector<Gen> nuclei){
  // == pi+ + 40Ar -> pi0 + p + p + 38Cl
  // == N(pi+) = 0, N(p) = 2, N(pi0) = 1, N(bkg) = 0
  bool pass_particle_multiplicities = piplus.size() == 1 && protons.size() == 2 && pizeros.size() == 1 && bkg_particles.size() == 0;
  if(!pass_particle_multiplicities) return;

}

int PionAnalyzer::GetNPID(std::vector<Gen> particles, int PID){

  int out = 0;
  for(unsigned int i=0; i<particles.size(); i++){
    if(particles.at(i).PID() == PID) out++;
  }

  return out;
}

void PionAnalyzer::SR1_FillHist(TString region, TString variable_str, double variable, int atomic_number, int atomic_mass, double weight, int N_bin, double x_min, double x_max){

  // -- Plots for all
  JSFillHist(region, variable_str + "_" + region, variable, weight, N_bin, x_min, x_max);

  // -- Plots for background
  if(atomic_number > 13){
    if(atomic_number != 17){
      JSFillHist(region, variable_str + "_" + TString::Itoa(atomic_number, 10) + "_" + region, variable, weight, N_bin, x_min, x_max);
    }
    else{
      if(atomic_mass != 39){
	JSFillHist(region, variable_str + "_" + TString::Itoa(atomic_number, 10) + "_" + region, variable, weight, N_bin, x_min, x_max);
      }
    }
  }
  else{
    JSFillHist(region, variable_str + "_bellow14_" + region, variable, weight, N_bin, x_min, x_max);
  }

  // -- Plots for signal
  if(atomic_number == 17 && atomic_mass == 39){
    JSFillHist(region, variable_str + "_Signal_" + region, variable, weight, N_bin, x_min, x_max);
  }
}

void PionAnalyzer::SR2_FillHist(TString region, TString variable_str, double variable, int atomic_number, int atomic_mass, double weight, int N_bin, double x_min, double x_max){

  // -- Plots for all
  JSFillHist(region, variable_str + "_" + region, variable, weight, N_bin, x_min, x_max);

  // -- Plots for background
  if(atomic_number > 13){
    if(atomic_number != 16){
      JSFillHist(region, variable_str + "_" + TString::Itoa(atomic_number, 10) + "_" + region, variable, weight, N_bin, x_min, x_max);
    }
    else{
      if(atomic_mass != 38){
        JSFillHist(region, variable_str + "_" + TString::Itoa(atomic_number, 10) + "_" + region, variable, weight, N_bin, x_min, x_max);
      }
    }
  }
  else{
    JSFillHist(region, variable_str + "_bellow14_" + region, variable, weight, N_bin, x_min, x_max);
  }

  // -- Plots for signal
  if(atomic_number == 16 && atomic_mass == 38){
    JSFillHist(region, variable_str + "_Signal_" + region, variable, weight, N_bin, x_min, x_max);
  }
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


