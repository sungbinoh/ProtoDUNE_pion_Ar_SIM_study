#include "PionAnalyzer.h"

void PionAnalyzer::initializeAnalyzer(){
  
  cout << "[[PionAnalyzer::initializeAnalyzer]] Simulator : " << Simulator << endl;
  debug_mode = true;
  debug_mode = false;

}

void PionAnalyzer::executeEvent(){

  if(debug_mode) cout << "[[PionAnalyzer::executeEvent]] : START" << endl;
  // == Returning events for FLUKA samples
  if(Simulator.Contains("FLUKA")){
    int N_inela_inter = this_FLUKANtuple.NIneHits;
    if(N_inela_inter != 1) return;
  }

  // == Collect all particles in an event
  std::vector<Gen> particles_all;
  if(Simulator.Contains("GEANT")){
    particles_all = GetAllParticles_GEANT4();
  }
  else if(Simulator.Contains("FLUKA")){
    particles_all = GetAllParticles_FLUKA();
  }
  else return;
  
  if(particles_all.size() < 1) return;

  if(debug_mode) cout << "[[PionAnalyzer::executeEvent]] : called all particles" << endl;

  // == Collect particles depending on PDG ID. Cut on momuntum : .P() > 0.
  std::vector<Gen> piplus_all = GetPiplus(particles_all, 0.);
  std::vector<Gen> protons_all = GetProtons(particles_all, 0.);
  std::vector<Gen> pizeros_all = GetPizeros(particles_all, 0.);
  std::vector<Gen> bkg_particles = GetBkgParticles(particles_all, 0.1); // == pi-, kaon, muon with P > 100 MeV
  std::vector<Gen> nuclei = GetNuclei(particles_all);
  // Momentum smearing only
  //std::vector<Gen> piplus_mom_smear = GetPiplus(smear->SmearOutParticles(piplus_all, 1), 100.);

  if(debug_mode) cout << "[[PionAnalyzer::executeEvent]] particles_all.size() : " << particles_all.size() << endl;

  // == Fill out general plots
  FillHist("N_particles", particles_all.size(), 1., 40, 0., 40.);
  FillHist("PID_of_first_particle", particles_all.at(0).PID(), 1., 20, 200., 220.);
  //FillHist("Atomic_number_of_second_particle", GetAtomicNumber(particles_all.at(1).PID()), 1., 25, 0., 25.);
  FillHist("P_1st_particle", particles_all.at(0).P(), 1., 2000, 0., 2000.);
  //FillHist("N_piplus", piplus_all.size(), 1., 10, 0., 10.);
  FillHist("N_protons", protons_all.size(), 1., 10, 0., 10.);
  FillHist("N_pizeros", pizeros_all.size(), 1., 10, 0., 10.);
  FillHist("N_bkg_particles", bkg_particles.size(), 1., 20, 0., 20.);
  FillHist("N_nuclei", nuclei.size(), 1., 20, 0., 20.);

  QE_Study(particles_all, piplus_all, protons_all, pizeros_all, bkg_particles, nuclei);
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
    //if(i_smear == 0 || i_smear == 7) executeEventWithVariables(i_smear, smear_flags[i_smear], particles_all, piplus_all, protons_all, pizeros_all, bkg_particles, nuclei);
  }
}

void PionAnalyzer::QE_Study(std::vector<Gen> particles_all, std::vector<Gen> piplus_all, std::vector<Gen> protons_all, std::vector<Gen> pizeros_all, std::vector<Gen> bkg_particles, std::vector<Gen> nuclei){
  
  // == N particles
  FillHist("P_beam", particles_all.at(0).P() * 1000., 1., 2000., 0., 2000.);
  //cout << "[PionAnalyzer::QE_Study] Beam Pz : " << particles_all.at(0).Pz() * 1000. << ", E_beam : " << particles_all.at(0).E() * 1000. << endl;
  FillHist("N_piplus", piplus_all.size() - 1., 1., 10., -0.5, 9.5);
  int N_gamma = 0;
  for(unsigned int i = 0; i < particles_all.size(); i++){
    if(particles_all.at(i).PID() == 22) N_gamma ++;
  }
  FillHist("N_gamma", N_gamma, 1., 10., 0., 10.);
  
  // == Protons and neutrons
  for(unsigned int i = 0; i < protons_all.size(); i++){
    FillHist("E_proton", protons_all.at(i).E() * 1000., 1., 2000., 0., 2000.);
    FillHist("KE_proton", protons_all.at(i).E() * 1000. - protons_all.at(i).M() * 1000. ,1., 100., 0., 100.);
  }
  for(unsigned int i = 0; i < particles_all.size(); i++){
    if(particles_all.at(i).PID() == 2112){
      double KE_neutron = particles_all.at(i).E() * 1000. - particles_all.at(i).M() * 1000.;
      FillHist("E_neutron", particles_all.at(i).E() * 1000.,1., 2000., 0., 2000.);
      FillHist("KE_neutron", KE_neutron , 1., 100., 0., 100.);
      //cout << "Neutron KE : " << particles_all.at(i).E() * 1000. - particles_all.at(i).M() * 1000. << endl;
    }
  }

  // == Pion energy loss
  if(piplus_all.size() < 2) return;
  Gen beam = particles_all.at(0);
  //FillHist("P_beam", beam.P() * 1000., 1., 2000., 0., 2000.);
  
  for(unsigned int i = 1; i < piplus_all.size(); i++){
    double E_loss = (beam.E() - piplus_all.at(i).E()) * 1000.;
    double cos_theta = cos(piplus_all.at(i).Theta());
    //cos_theta = acos(piplus_all.at(i).Pz() / piplus_all.at(i).P());
    double EQE = Get_EQE(piplus_all.at(i).P() * 1000., cos_theta);
    double EQE_massive_plus = Get_EQE_NC_Pion(piplus_all.at(i).P() * 1000., cos_theta, 4., 1.);
    double EQE_massive_minus = Get_EQE_NC_Pion(piplus_all.at(i).P() * 1000., cos_theta, 4., -1.);
    double EQE_delta_plus = Get_EQE_NC_Delta_Pion(piplus_all.at(i).P() * 1000., cos_theta, 4., 1.);
    double EQE_delta_minus = Get_EQE_NC_Delta_Pion(piplus_all.at(i).P() * 1000., cos_theta, 4., -1.);

    double mX_QE = Get_EQE_NC_Pion_mX(piplus_all.at(i).P() * 1000., cos_theta, 4., beam.P() * 1000.);

    //cout << "[PionAnalyzer::QE_Study] EQE_massive_plus : " << EQE_massive_plus << ", EQE_massive_minus : " << EQE_massive_minus << endl;
    if(i == 1){
      FillHist("E_loss_pion", E_loss, 1., 1000., 0., 1000.);
      FillHist("EQE_pion", EQE, 1., 1500., 0., 1500.);
      FillHist("EQE_massive_plus_pion", EQE_massive_plus, 1., 10000., -5000., 5000.);
      FillHist("EQE_massive_minus_pion", EQE_massive_minus, 1., 1500., 0., 1500.);
      FillHist("EQE_massive_2D_pion", EQE_massive_plus, EQE_massive_minus, 1., 1000, -5000., 5000., 1000, -5000., 5000.);
      FillHist("EQE_delta_plus", EQE_delta_plus, 1., 10000., -5000., 5000.);
      FillHist("EQE_delta_minus", EQE_delta_minus, 1., 1500., 0., 1500.);
      FillHist("mX_QE", mX_QE, 1., 2000., 0., 2000.);
      FillHist("Outgoing_pion_P_vs_angle", piplus_all.at(i).P() * 1000., piplus_all.at(i).Theta(), 1., 1050., 0., 1050., 1000., 0., 4.);
      FillHist("Eloss_vs_angle", E_loss, piplus_all.at(i).Theta(), 1., 1000., 0., 1000., 1000., 0., 4.);
      FillHist("EQE_vs_angle", EQE, piplus_all.at(i).Theta(), 1., 1500., 0., 1500., 1000., 0., 4.);
      
      if(piplus_all.size() == 2){
	FillHist("E_loss_pion_OnlyOne", E_loss, 1., 1000., 0., 1000.);
	FillHist("EQE_pion_OnlyOne", EQE, 1., 1500., 0., 1500.);
	FillHist("EQE_massive_plus_pion_OnlyOne", EQE_massive_plus, 1., 10000., -5000., 5000.);
	FillHist("EQE_massive_minus_pion_OnlyeOne", EQE_massive_minus, 1., 1500., 0., 1500.);
	FillHist("EQE_massive_2D_pion_OnlyOne", EQE_massive_plus, EQE_massive_minus, 1., 1000, -5000., 5000., 1000, -5000., 5000.);
	FillHist("EQE_delta_plus_OnlyOne", EQE_delta_plus, 1., 10000., -5000., 5000.);
	FillHist("EQE_delta_minus_OnlyOne", EQE_delta_minus, 1., 1500., 0., 1500.);
	FillHist("mX_QE_OnlyOne",mX_QE, 1., 2000., 0., 2000.);
	FillHist("Outgoing_pion_OnlyOne_P_vs_angle", piplus_all.at(i).P() * 1000., piplus_all.at(i).Theta(), 1., 1050., 0., 1050., 1000., 0., 4.);
	FillHist("Eloss_vs_angle_OnlyOne", E_loss, piplus_all.at(i).Theta(), 1., 1000., 0., 1000., 1000., 0., 4.);
	FillHist("EQE_vs_angle_OnlyOne", EQE, piplus_all.at(i).Theta(), 1., 1500., 0., 1500., 1000., 0., 4.);
      }
    }
    FillHist("E_loss_pion_all", E_loss, 1., 1000., 0., 1000.);
    FillHist("EQE_pion_all", EQE, 1., 1500., 0., 1500.);
    FillHist("EQE_massive_plus_pion_all", EQE_massive_plus, 1., 10000., -5000., 5000.);
    FillHist("EQE_massive_minus_pion_all", EQE_massive_minus, 1., 1500., 0., 1500.);
    FillHist("EQE_massive_2D_pion_all", EQE_massive_plus, EQE_massive_minus, 1., 1000, -5000., 5000., 1000, -5000., 5000.);
    FillHist("EQE_delta_plus_all", EQE_delta_plus, 1., 10000., -5000., 5000.);
    FillHist("EQE_delta_minus_all", EQE_delta_minus, 1., 1500., 0., 1500.);
    FillHist("mX_QE_all",mX_QE, 1., 2000., 0., 2000.);
    FillHist("Outgoing_pion_all_P_vs_angle", piplus_all.at(i).P() * 1000., piplus_all.at(i).Theta(), 1., 1050., 0., 1050., 1000., 0., 4.);
    FillHist("Eloss_vs_angle_all", E_loss, piplus_all.at(i).Theta(), 1., 1000., 0., 1000., 1000., 0., 4.);
    FillHist("EQE_vs_angle_all", EQE, piplus_all.at(i).Theta(), 1., 1500., 0., 1500., 1000., 0., 4.);
    for(int j = 0; j < 50; j++){
      double this_E_binding = 0. + (j + 0.) * 1.; // == [MeV]
      double this_EQE_plus = Get_EQE_NC_Pion(piplus_all.at(i).P() * 1000., cos_theta, this_E_binding, 1.);
      double this_EQE_minus = Get_EQE_NC_Pion(piplus_all.at(i).P() * 1000., cos_theta, this_E_binding, -1.);
      double delta_EQE_plus = beam.E() * 1000. - this_EQE_plus;
      double delta_EQE_minus = beam.E() * 1000. - this_EQE_minus;
      TString E_binding_str = Form("%.0f", this_E_binding);
      FillHist("EQE_minus_Eb_" + E_binding_str, this_EQE_minus, 1., 1500., 0., 1500.);
      FillHist("delta_EQE_minus_Eb_" + E_binding_str, delta_EQE_minus, 1., 3000., -1500., 1500.);
    }
  }
}

void PionAnalyzer::executeEventWithVariables(int smearBit, TString smear_flag, std::vector<Gen> particles_all, std::vector<Gen> piplus_all, std::vector<Gen> protons_all, std::vector<Gen> pizeros_all, std::vector<Gen> bkg_particles, std::vector<Gen> nuclei){

  // == Apply smearing & P cut (piplus > 0.1 GeV, proton > 0.3 GeV, pizero > 0 GeV)
  std::vector<Gen> piplus = GetPiplus(smear->SmearOutParticles(piplus_all, smearBit), 0.1);
  std::vector<Gen> protons = GetProtons(smear->SmearOutParticles(protons_all, smearBit), 0.3);
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
  if(Simulator.Contains("FLUKA")){
    std::vector<Gen> piplus_nocut = GetPiplus(particles_all, 0.);
    std::vector<Gen> piminus_nocut = GetPiminus(particles_all, 0.);
    std::vector<Gen> protons_nocut = GetProtons(particles_all, 0.);
    std::vector<Gen> neutrons_nocut = GetNeutrons(particles_all, 0.);
    
    current_atomic_number = 18 - piplus_nocut.size() + 2 + piminus_nocut.size() - protons_nocut.size();
    current_atomic_mass = 40 - protons_nocut.size() - neutrons_nocut.size();

    if(nuclei.size() > 0){
      if(fabs(nuclei.at(0).M() - 3.72738) < 0.01){
	//cout << "nuclei.at(0).M() : " << nuclei.at(0).M() << endl;
	current_atomic_number = current_atomic_number - 2;
	current_atomic_mass = current_atomic_mass - 4;
      }
      else return;
    }
  }

  std::vector<Gen> neutrons_all = GetNeutrons(particles_all, 0.);


  // -- Calculate residual mass
  Gen beam = particles_all.at(0);
  Particle Ar_target;
  Ar_target.SetPxPyPzE(0., 0., 0., 37.225); //40Ar mass = 37.225 GeV, https://www.wolframalpha.com/input?i2d=true&i=Argon+40+mass+in+GeV 
  Particle residual_reco = beam + Ar_target - piplus.at(1) - protons.at(0);
  double residual_mass = residual_reco.M();

  // -- Calculate additional variables
  double Q_square{0.}, Q{0.}, q0{0.}, y{0.}, sqrt_s{0.}, Delta_E{0.};
  Particle q_4vec = beam - piplus.at(1);
  Q_square = (-pow(q_4vec.E(), 2) + pow(q_4vec.P(), 2));
  Q = pow(Q_square, 0.5);
  q0 = q_4vec.E();
  y = (beam.E() - piplus.at(1).E()) / beam.E();
  Delta_E = beam.E() - piplus.at(1).E();
  sqrt_s = (beam + Ar_target).M();

  double P_balance_piplus_p = ( piplus.at(1).P() - protons.at(0).P() ) / piplus.at(1).P();
  double P_balance_beam_piplus = (beam.P() - piplus.at(1).P()) / beam.P();
  double P_balance_beam_p = (beam.P() - protons.at(0).P() ) / beam.P();

  // -- Draw plots
  TString region = "SR1_" + smear_flag;
  //if(residual_mass > 36.310) return;
  
  SR1_FillHist(region, "M_residual", residual_reco.M(), current_atomic_number, current_atomic_mass, 1., 200, 36.200, 36.400);
  SR1_FillHist(region, "Q_square", Q_square, current_atomic_number, current_atomic_mass, 1., 200, 0., 2.0);
  SR1_FillHist(region, "q0", q0, current_atomic_number, current_atomic_mass, 1., 200, 0., 2.0);
  SR1_FillHist(region, "y", y, current_atomic_number, current_atomic_mass, 1., 100, 0., 1.0);
  SR1_FillHist(region, "Delta_E", Delta_E, current_atomic_number, current_atomic_mass, 1., 200, 0., 2.0);
  SR1_FillHist(region, "P_balance_piplus_p", P_balance_piplus_p, current_atomic_number, current_atomic_mass, 1., 80, -3., 1.0);
  SR1_FillHist(region, "P_balance_beam_piplus", P_balance_beam_piplus, current_atomic_number, current_atomic_mass, 1., 80, -3., 1.0);
  SR1_FillHist(region, "P_balance_beam_p", P_balance_beam_p, current_atomic_number, current_atomic_mass, 1., 80, -3., 1.0);
  SR1_FillHist(region, "P_proton", protons.at(0).P(), current_atomic_number, current_atomic_mass, 1., 200, 0., 2.);
  SR1_FillHist(region, "P_piplus", piplus.at(1).P(), current_atomic_number, current_atomic_mass, 1., 200, 0., 2.);
  
  // -- Plots for all
  JSFillHist(region, "Z_" + region, current_atomic_number, 1., 20, 0., 20.);
  JSFillHist(region, "N_nuclei_" + region, nuclei.size(), 1., 20, 0., 20.);
  if(nuclei.size() > 0){
    JSFillHist(region, "M_nuclei_" + region, nuclei.at(0).M(), 1., 20000, 0., 20.);
  }
  JSFillHist(region, "M_residual_VS_P_proton_" + region, residual_reco.M(), protons.at(0).P(), 1., 140, 36.240, 36.380, 70, 300., 1000.);
  
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
	JSFillHist(region, "M_residual_" + TString::Itoa(current_atomic_number, 10) + "_Nn_" + TString::Itoa(N_neutron, 10) + "_" + region, residual_reco.M(), 1., 200, 36.200, 36.400);
	JSFillHist(region, "M_residual_VS_P_proton_" + TString::Itoa(current_atomic_number, 10) + "_" + region, residual_reco.M(), protons.at(0).P(), 1., 140, 36.240, 36.380, 70, 300., 1000.);
	JSFillHist(region, "M_residual_VS_P_piplus_" + TString::Itoa(current_atomic_number, 10) + "_" + region, residual_reco.M(), piplus.at(1).P(), 1., 140, 36.240, 36.380, 90, 100., 1000.);
	JSFillHist(region, "Q_square_VS_P_balance_piplus_p_" + TString::Itoa(current_atomic_number, 10) + "_" + region, Q_square, P_balance_piplus_p, 1., 200, 0., 2.0, 80, -3., 1.);
	JSFillHist(region, "Q_square_VS_P_balance_beam_piplus_" + TString::Itoa(current_atomic_number, 10) + "_" + region, Q_square, P_balance_beam_piplus, 1., 200, 0., 2.0, 80, -3., 1.);
	JSFillHist(region, "Q_square_VS_P_balance_beam_p_" + TString::Itoa(current_atomic_number, 10) + "_" + region, Q_square, P_balance_beam_p, 1., 200, 0., 2.0, 80, -3., 1.);
	JSFillHist(region, "P_balance_beam_piplus_VS_P_balance_beam_p_" + TString::Itoa(current_atomic_number, 10) + "_" + region, P_balance_beam_piplus, P_balance_beam_p, 1., 80, -3., 1.0, 80, -3., 1.);
	if(neutrons_all.size() > 0){
	  for(unsigned int i_n = 0; i_n < neutrons_all.size(); i_n ++){
	    JSFillHist(region, "P_neutron_Nn_" + TString::Itoa(neutrons_all.size(), 10) + "_" + TString::Itoa(i_n, 10) + "th_n_" + region, neutrons_all.at(i_n).P(), 1., 1000, 0., 1.);
	  }
	}
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
    JSFillHist(region, "M_residual_VS_P_proton_Signal_" + region, residual_reco.M(), protons.at(0).P(), 1., 140, 36.240, 36.380, 70, 300., 1000.);
    JSFillHist(region, "M_residual_VS_P_piplus_Signal_" + region, residual_reco.M(), piplus.at(1).P(), 1., 140, 36.240, 36.380, 90, 100., 1000.);
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
  //if(debug_mode && smear_flag == "NONE") cout << "[[PionAnalyzer::SR1]] Q_square : " << Q_square << ", Q : " << Q << endl;

  // -- Draw plots agains after applying residual mass cut
  if(residual_mass > 36.310) return;
  region = region + "_mXcut";

  SR1_FillHist(region, "M_residual", residual_reco.M(), current_atomic_number, current_atomic_mass, 1., 200, 36.200, 36.400);
  SR1_FillHist(region, "Q_square", Q_square, current_atomic_number, current_atomic_mass, 1., 200, 0., 2.0);
  SR1_FillHist(region, "q0", q0, current_atomic_number, current_atomic_mass, 1., 200, 0., 2.0);
  SR1_FillHist(region, "y", y, current_atomic_number, current_atomic_mass, 1., 100, 0., 1.0);
  SR1_FillHist(region, "Delta_E", Delta_E, current_atomic_number, current_atomic_mass, 1., 200, 0., 2.0);
  SR1_FillHist(region, "P_balance_piplus_p", P_balance_piplus_p, current_atomic_number, current_atomic_mass, 1., 80, -3., 1.0);
  SR1_FillHist(region, "P_balance_beam_piplus", P_balance_beam_piplus, current_atomic_number, current_atomic_mass, 1., 80, -3., 1.0);
  SR1_FillHist(region, "P_balance_beam_p", P_balance_beam_p, current_atomic_number, current_atomic_mass, 1., 80, -3., 1.0);
  SR1_FillHist(region, "P_proton", protons.at(0).P(), current_atomic_number, current_atomic_mass, 1., 200, 0., 2.);
  SR1_FillHist(region, "P_piplus", piplus.at(1).P(), current_atomic_number, current_atomic_mass, 1., 200, 0., 2.);

  JSFillHist(region, "Z_" + region, current_atomic_number, 1., 20, 0., 20.);
  JSFillHist(region, "N_nuclei_" + region, nuclei.size(), 1., 20, 0., 20.);
  if(nuclei.size() > 0){
    JSFillHist(region, "M_nuclei_" + region, nuclei.at(0).M(), 1., 20000, 0., 20.);
  }
  JSFillHist(region, "M_residual_VS_P_proton_" + region, residual_reco.M(), protons.at(0).P(), 1., 140, 36.240, 36.380, 70, 300., 1000.);

  if(current_atomic_number > 13){
    if(current_atomic_number != 17){

    }
    else{ // -- Plots for Z = 17 bkg
      if(current_atomic_mass != 39){
        int N_neutron = GetNPID(particles_all, 2112);
        int N_gamma = GetNPID(particles_all, 22);
        JSFillHist(region, "N_neutron_" + TString::Itoa(current_atomic_number, 10) + "_" + region, N_neutron, 1., 20, 0., 20.);
        JSFillHist(region, "N_neutron_VS_N_gamma_" + TString::Itoa(current_atomic_number, 10) + "_" + region, N_neutron, N_gamma, 1., 8, 0., 8., 8, 0., 8.);
        JSFillHist(region, "M_residual_" + TString::Itoa(current_atomic_number, 10) + "_Nn_" + TString::Itoa(N_neutron, 10) + "_" + region, residual_reco.M(), 1., 200, 36.200, 36.400);
        JSFillHist(region, "M_residual_VS_P_proton_" + TString::Itoa(current_atomic_number, 10) + "_" + region, residual_reco.M(), protons.at(0).P(), 1., 140, 36.240, 36.380, 70, 300., 1000.);
        JSFillHist(region, "M_residual_VS_P_piplus_" + TString::Itoa(current_atomic_number, 10) + "_" + region, residual_reco.M(), piplus.at(1).P(), 1., 140, 36.240, 36.380, 90, 100., 1000.);
        JSFillHist(region, "Q_square_VS_P_balance_piplus_p_" + TString::Itoa(current_atomic_number, 10) + "_" + region, Q_square, P_balance_piplus_p, 1., 200, 0., 2.0, 80, -3., 1.);
        JSFillHist(region, "Q_square_VS_P_balance_beam_piplus_" + TString::Itoa(current_atomic_number, 10) + "_" + region, Q_square, P_balance_beam_piplus, 1., 200, 0., 2.0, 80, -3., 1.);
        JSFillHist(region, "Q_square_VS_P_balance_beam_p_" + TString::Itoa(current_atomic_number, 10) + "_" + region, Q_square, P_balance_beam_p, 1., 200, 0., 2.0, 80, -3., 1.);
        JSFillHist(region, "P_balance_beam_piplus_VS_P_balance_beam_p_" + TString::Itoa(current_atomic_number, 10) + "_" + region, P_balance_beam_piplus, P_balance_beam_p, 1., 80, -3., 1.0, 80, -3., 1.);
        if(neutrons_all.size() > 0){
          for(unsigned int i_n = 0; i_n < neutrons_all.size(); i_n ++){
            JSFillHist(region, "P_neutron_Nn_" + TString::Itoa(N_neutron, 10) + "_" + TString::Itoa(i_n, 10) + "th_n_" + region, neutrons_all.at(i_n).P(), 1., 1000, 0., 1.);
          }
        }
      }
    }
  }
  else{

  }
 
  if(current_atomic_number == 17 && current_atomic_mass == 39){
    JSFillHist(region, "Z_Signal_" + region, current_atomic_number, 1., 20, 0., 20.);
    int N_gamma = GetNPID(particles_all, 22);
    JSFillHist(region, "N_gamma_Signal_" + region, N_gamma, 1., 20, 0., 20.);
    JSFillHist(region, "Q_square_Ngamma_" + TString::Itoa(N_gamma, 10) + "_" + region, Q_square, 1., 200, 0., 2.0);
    JSFillHist(region, "Q_square_VS_y_" + region, Q_square, y, 1., 200, 0., 2.0, 100, 0., 1.0);
    JSFillHist(region, "M_residual_VS_Q_square_" + region, residual_reco.M(), Q_square, 1., 200, 0., 2.0, 100, 0., 1.0);
    JSFillHist(region, "M_residual_VS_P_proton_Signal_" + region, residual_reco.M(), protons.at(0).P(), 1., 140, 36.240, 36.380, 70, 300., 1000.);
    JSFillHist(region, "M_residual_VS_P_piplus_Signal_" + region, residual_reco.M(), piplus.at(1).P(), 1., 140, 36.240, 36.380, 90, 100., 1000.);
    JSFillHist(region, "Q_square_VS_P_balance_piplus_p_Signal_" + region, Q_square, P_balance_piplus_p, 1., 200, 0., 2.0, 80, -3., 1.);
    JSFillHist(region, "Q_square_VS_P_balance_beam_piplus_Signal_" + region, Q_square, P_balance_beam_piplus, 1., 200, 0., 2.0, 80, -3., 1.);
    JSFillHist(region, "Q_square_VS_P_balance_beam_p_Signal_" + region, Q_square, P_balance_beam_p, 1., 200, 0., 2.0, 80, -3., 1.);
    JSFillHist(region, "P_balance_beam_piplus_VS_P_balance_beam_p_Signal_" + region, P_balance_beam_piplus, P_balance_beam_p, 1., 80, -3., 1.0, 80, -3., 1.);
  }

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
  Ar_target.SetPxPyPzE(0., 0., 0., 37.225); //40Ar mass = 37.225 GeV, https://www.wolframalpha.com/input?i2d=true&i=Argon+40+mass+in+GeV
  Particle residual_reco = beam + Ar_target - piplus.at(1) - protons.at(0) - protons.at(1);
  Particle residual_reco_Mneutron;
  Particle iso_sym_neutron = protons.at(1);
  iso_sym_neutron.SetPtEtaPhiM(iso_sym_neutron.Pt(), iso_sym_neutron.Eta(), iso_sym_neutron.Phi(), M_neutron  * 1000);
  residual_reco_Mneutron = beam + Ar_target - piplus.at(1) - protons.at(0) - iso_sym_neutron;

  TString region = "SR2_" + smear_flag;

  SR1_FillHist(region, "M_residual", residual_reco.M(), current_atomic_number, current_atomic_mass, 1., 200, 36.200, 36.400);
  SR1_FillHist(region, "M_residual_Mneutron", residual_reco_Mneutron.M(), current_atomic_number, current_atomic_mass, 1., 200, 36.200, 36.400);


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

double PionAnalyzer::Get_EQE(double P_pion, double cos_theta){
  double m_proton = 938.272;
  double m_neutron = 939.565;
  double m_pion = 139.57;
  double E_binding = 40.;

  double E_pion = sqrt( pow(P_pion, 2.0) + pow(m_pion, 2.0) );

  double numer = pow(m_proton, 2.0)  - pow(m_neutron - E_binding, 2.0) - pow(m_pion, 2.0) + 2.0 * (m_neutron - E_binding) * E_pion;
  double denom = 2.0 * ( m_neutron - E_binding - E_pion + P_pion * cos_theta );

  double EQE = numer / denom;

  return EQE;
}

double PionAnalyzer::Get_EQE_NC_Pion(double P_pion, double cos_theta, double E_binding, int which_sol){
  double m_proton = 938.272;
  double m_pion = 139.57;

  double E_pion = sqrt( pow(P_pion, 2.0) + pow(m_pion, 2.0) );

  double A = m_proton - E_binding - E_pion;
  double B = pow(m_pion, 2.) - pow(P_pion, 2.) - pow(m_proton, 2.);

  // == ax^2 + bx + c = 0
  double a = 4. * (A*A - P_pion * P_pion * cos_theta * cos_theta);
  double b = 4. * A * (A*A + B);
  double c = pow(A*A + B, 2.) + 4. * m_pion * m_pion * P_pion * P_pion * cos_theta * cos_theta;

  
  double numer1 = (-1.) * b;
  double numer_sqrt = sqrt(b*b - 4. * a * c);
  double denom = 2. * a;

  double EQE = (numer1 + (which_sol + 0.) * numer_sqrt ) / denom;

  return EQE;
}

double PionAnalyzer::Get_EQE_NC_Delta_Pion(double P_pion, double cos_theta, double E_binding, int which_sol){
  double m_proton = 938.272;
  double m_delta = 1232.;
  double m_pion = 139.57;

  double E_pion = sqrt( pow(P_pion, 2.0) + pow(m_pion, 2.0) );

  double A = m_proton - E_binding - E_pion;
  double B = pow(m_pion, 2.) - pow(P_pion, 2.) - pow(m_delta, 2.);

  // == ax^2 + bx + c = 0
  double a = 4. * (A*A - P_pion * P_pion * cos_theta * cos_theta);
  double b = 4. * A * (A*A + B);
  double c = pow(A*A + B, 2.) + 4. * m_pion * m_pion * P_pion * P_pion * cos_theta * cos_theta;

  double numer1 = (-1.) * b;
  double numer_sqrt = sqrt(b*b - 4. * a * c);
  double denom = 2. * a;

  double EQE = (numer1 + (which_sol + 0.) * numer_sqrt ) / denom;

  return EQE;
}

double PionAnalyzer::Get_EQE_NC_Pion_mX(double P_pion, double cos_theta, double E_binding, double P_beam){
  double m_proton = 938.272;
  double m_pion = 139.57;

  double E_pion = sqrt( pow(P_pion, 2.0) + pow(m_pion, 2.0) );
  double E_beam = sqrt( pow(P_beam, 2.0) + pow(m_pion, 2.0) );

  double A = m_proton - E_binding - E_pion;

  double mX = sqrt(m_pion * m_pion + A * A + 2.0 * A * E_beam + 2.0 * P_beam * P_pion * cos_theta - P_pion * P_pion);

  return mX;
}

PionAnalyzer::PionAnalyzer(){

}

PionAnalyzer::~PionAnalyzer(){

}


