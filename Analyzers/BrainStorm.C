#include "BrainStorm.h"

void BrainStorm::initializeAnalyzer(){
  
  cout << "[[BrainStorm::initializeAnalyzer]] Simulator : " << Simulator << endl;
  debug_mode = true;
  debug_mode = false;

}

void BrainStorm::executeEvent(){

  if(debug_mode) cout << "[[BrainStorm::executeEvent]] : START" << endl;
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

  if(debug_mode) cout << "[[BrainStorm::executeEvent]] : called all particles" << endl;

  // == Collect particles depending on PDG ID. Cut on momuntum : .P() > 0.
  std::vector<Gen> piplus_all = GetPiplus(particles_all, 0.);
  std::vector<Gen> protons_all = GetProtons(particles_all, 0.);
  std::vector<Gen> neutrons_all = GetNeutrons(particles_all, 0.);
  std::vector<Gen> pizeros_all = GetPizeros(particles_all, 0.);
  std::vector<Gen> bkg_particles = GetBkgParticles(particles_all, 0.1); // == pi-, kaon, muon with P > 100 MeV
  std::vector<Gen> nuclei = GetNuclei(particles_all);
  // Momentum smearing only
  //std::vector<Gen> piplus_mom_smear = GetPiplus(smear->SmearOutParticles(piplus_all, 1), 100.);

  if(debug_mode) cout << "[[BrainStorm::executeEvent]] particles_all.size() : " << particles_all.size() << endl;

  // == Fill some general plots
  FillHist("N_particles", particles_all.size(), 1., 40, -0.5, 39.5);
  FillHist("PID_of_first_particle", particles_all.at(0).PID(), 1., 20, 200., 220.);
  FillHist("P_1st_particle", particles_all.at(0).P(), 1., 2000, 0., 2000.);
  FillHist("N_piplus", piplus_all.size(), 1., 10., -0.5, 9.5);
  FillHist("N_protons", protons_all.size(), 1., 10, -0.5, 9.5);
  FillHist("N_neutrons", neutrons_all.size(), 1., 10, -0.5, 9.5);
  FillHist("N_pizeros", pizeros_all.size(), 1., 10, -0.5, 9.5);
  FillHist("N_bkg_particles", bkg_particles.size(), 1., 20, -0.5, 19.5);
  FillHist("N_nuclei", nuclei.size(), 1., 20, -0.5, 19.5);

  QE_Study(particles_all, piplus_all, protons_all, pizeros_all, bkg_particles, nuclei);
  
  // == Run for each smearBit
  TString smear_flags[8] = {"NONE", "P", "Theta", "P_Theta", "Phi", "P_Phi", "Phi_Theta", "All"};
  for(int i_smear=0; i_smear<8; i_smear++){
    //if(i_smear == 0 || i_smear == 7) executeEventWithVariables(i_smear, smear_flags[i_smear], particles_all, piplus_all, protons_all, pizeros_all, bkg_particles, nuclei);
  }
}

void BrainStorm::QE_Study(const std::vector<Gen>& particles_all, const std::vector<Gen>& piplus_all, const std::vector<Gen>& protons_all, const std::vector<Gen>& pizeros_all, const std::vector<Gen>& bkg_particles, const std::vector<Gen>& nuclei){

  // == atomic number of first nuclei
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
        current_atomic_number = current_atomic_number - 2;
        current_atomic_mass = current_atomic_mass - 4;
      }
      else return;
    }
  }

  // == count gamma multiplicity
  int N_gamma = 0;
  for(unsigned int i = 0; i < particles_all.size(); i++){
    if(particles_all.at(i).PID() == 22) N_gamma ++;
  }
  FillHist("N_gamma", N_gamma, 1., 10., -0.5, 9.5);
  
  // == Energy and KE of protons and neutrons
  for(unsigned int i = 0; i < protons_all.size(); i++){
    FillHist("E_proton", protons_all.at(i).E() * 1000., 1., 2000., 0., 2000.);
    FillHist("KE_proton", protons_all.at(i).E() * 1000. - protons_all.at(i).M() * 1000. ,1., 1000., 0., 1000.);
  }
  int N_n = GetNPID(particles_all, 2112);
  for(unsigned int i = 0; i < particles_all.size(); i++){
    if(particles_all.at(i).PID() == 2112){
      double KE_neutron = particles_all.at(i).E() * 1000. - particles_all.at(i).M() * 1000.;
      FillHist("E_neutron", particles_all.at(i).E() * 1000.,1., 2000., 0., 2000.);
      FillHist("KE_neutron", KE_neutron , 1., 1000., 0., 1000.);
    }
  }

  // == Study with at least one piplus at final state
  if(piplus_all.size() < 2) return;
  Gen beam = particles_all.at(0);
  
  for(unsigned int i = 1; i < piplus_all.size(); i++){ // == Loop for daughter pipluses
    double E_loss = (beam.E() - piplus_all.at(i).E()) * 1000.;
    double cos_theta = cos(piplus_all.at(i).Theta());
    double EQE_pion_massless = Get_EQE_pion_massless(piplus_all.at(i).P() * 1000., cos_theta);
    double EQE_pion_plus = Get_EQE_NC_Pion(piplus_all.at(i).P() * 1000., cos_theta, 4., 1.);
    double EQE_pion_minus = Get_EQE_NC_Pion(piplus_all.at(i).P() * 1000., cos_theta, 4., -1.);
    double EQE_delta_pion_plus = Get_EQE_NC_Delta_Pion(piplus_all.at(i).P() * 1000., cos_theta, 4., 1.);
    double EQE_delta_pion_minus = Get_EQE_NC_Delta_Pion(piplus_all.at(i).P() * 1000., cos_theta, 4., -1.);
    double mX_QE_pion = Get_EQE_NC_Pion_mX(piplus_all.at(i).P() * 1000., cos_theta, 4., beam.P() * 1000.);

    pionEQEstrc pionEQEcontainer;
    pionEQEcontainer.E_loss = E_loss;
    pionEQEcontainer.EQE_pion_massless = EQE_pion_massless;
    pionEQEcontainer.EQE_pion_plus = EQE_pion_plus;
    pionEQEcontainer.EQE_pion_minus = EQE_pion_minus;
    pionEQEcontainer.EQE_delta_pion_plus = EQE_delta_pion_plus;
    pionEQEcontainer.EQE_delta_pion_minus = EQE_delta_pion_minus;
    pionEQEcontainer.mX_QE_pion = mX_QE_pion;
    pionEQEcontainer.piplus = piplus_all.at(i);

    double KE_pion = piplus_all.at(1).E() * 1000. - 139.57;
    
    if(i == 1){ // == For the leading piplus
      FillPionEQEPlots("leading_pion", pionEQEcontainer, N_n);
      
      if(piplus_all.size() == 2){ // == Exactly one piplus at final state
	
	FillPionEQEPlots("1pi", pionEQEcontainer, N_n);
	JSFillHist("1pi", "N_p_vs_N_n_OnlyOne_1pi", protons_all.size(), N_n, 1., 10., -0.5, 9.5, 10., -0.5, 9.5);
	
	if(protons_all.size() == 1){
	  Particle Ar_target;
	  Ar_target.SetPxPyPzE(0., 0., 0., 37.225); 
	  Particle residual_reco = beam + Ar_target - piplus_all.at(1) - protons_all.at(0);
	  double residual_mass = residual_reco.M();
	  JSFillHist("1p1pi", "mXN_1p1pi", residual_mass, 1., 2000., 36., 38.);

	  Particle initial_p = piplus_all.at(1) + protons_all.at(0) - beam;
	  double initial_p_P = initial_p.P() * 1000.;

	  double cos_theta_proton = cos(protons_all.at(0).Theta());
	  double EQE_proton_massless = Get_EQE_proton_massless(protons_all.at(0).P() * 1000., cos_theta_proton);
	  double EQE_proton_plus = Get_EQE_NC_Proton(protons_all.at(0).P() * 1000., cos_theta_proton, 4., +1.);
	  double EQE_proton_minus = Get_EQE_NC_Proton(protons_all.at(0).P() * 1000., cos_theta_proton, 4., -1.);
	  double KE_proton = protons_all.at(0).E() * 1000. - 938.272;
	  
	  protonEQEstrc protonEQEcontainer;
	  protonEQEcontainer.EQE_proton_massless = EQE_proton_massless;
	  protonEQEcontainer.EQE_proton_plus = EQE_proton_plus;
	  protonEQEcontainer.EQE_proton_minus = EQE_proton_minus;
	  protonEQEcontainer.proton = protons_all.at(0);

	  FillPionEQEPlots("1p1pi", pionEQEcontainer, N_n);
	  FillProtonEQEPlots("1p1pi", protonEQEcontainer, N_n);
	  
	  JSFillHist("1p1pi", "initial_p_P_1p1pi", initial_p_P, 1., 1500., 0., 1500.);
	  JSFillHist("1p1pi", Form("mX_%dn_1p1pi", N_n), residual_mass, 1., 2000., 36., 38.);
	  JSFillHist("1p1pi", Form("initial_p_P_%dn_1p1pi", N_n), initial_p_P, 1., 1500., 0., 1500.);
	  JSFillHist("1p1pi", Form("KE_pion_vs_KE_proton_%dn_1p1pi", N_n), KE_pion, KE_proton, 1., 1000., 0., 1000., 1000., 0., 1000.); 
	  JSFillHist("1p1pi", "EQE_pion_minus_vs_residual_mass_1p1pi", EQE_pion_minus, residual_mass, 1., 1500., 0., 1500., 2000., 36., 38.);
	  
	  if(residual_mass < 36.307){
	    FillPionEQEPlots("1p1pi_mXcut", pionEQEcontainer, N_n);
	    FillProtonEQEPlots("1p1pi_mXcut", protonEQEcontainer, N_n);
	    JSFillHist("1p1pi_mXcut", "initial_p_P_1p1pi_mXcut", initial_p_P, 1., 1500., 0., 1500.);
	  }
	  
	  if(KE_proton > 300.){
	    FillPionEQEPlots("1p1pi_KEproton300", pionEQEcontainer, N_n);
            FillProtonEQEPlots("1p1pi_KEproton300", protonEQEcontainer, N_n);
	  }

	  if(fabs(EQE_pion_minus - 1009.) < 50.){
	    FillPionEQEPlots("1p1pi_EQEpion50window", pionEQEcontainer, N_n);
            FillProtonEQEPlots("1p1pi_EQEpion50window", protonEQEcontainer, N_n);
	    if(KE_proton > 300.){
	      FillPionEQEPlots("1p1pi_EQEpion50window_KEproton300", pionEQEcontainer, N_n);
	      FillProtonEQEPlots("1p1pi_EQEpion50window_KEproton300", protonEQEcontainer, N_n);
	    }
	  }

	}
      }
    }

    FillPionEQEPlots("all_pions", pionEQEcontainer, N_n);
    for(int j = 0; j < 50; j++){
      double this_E_binding = 0. + (j + 0.) * 1.; // == [MeV]
      double this_EQE_plus = Get_EQE_NC_Pion(piplus_all.at(i).P() * 1000., cos_theta, this_E_binding, 1.);
      double this_EQE_minus = Get_EQE_NC_Pion(piplus_all.at(i).P() * 1000., cos_theta, this_E_binding, -1.);
      double delta_EQE_plus = beam.E() * 1000. - this_EQE_plus;
      double delta_EQE_minus = beam.E() * 1000. - this_EQE_minus;
      TString E_binding_str = Form("%.0f", this_E_binding);
      JSFillHist("Eb_study", "EQE_minus_Eb_" + E_binding_str, this_EQE_minus, 1., 1500., 0., 1500.);
      JSFillHist("Eb_study", "delta_EQE_minus_Eb_" + E_binding_str, delta_EQE_minus, 1., 3000., -1500., 1500.);
    }
  }
}

void BrainStorm::FillPionEQEPlots(TString suffix, pionEQEstrc pionEQEcontainer, int N_n){
  JSFillHist(suffix, "EQE_pion_massless_" + suffix, pionEQEcontainer.EQE_pion_massless, 1., 1500., 0., 1500.);
  JSFillHist(suffix, "EQE_pion_plus_" + suffix, pionEQEcontainer.EQE_pion_plus, 1., 10000., -5000., 5000.);
  JSFillHist(suffix, "EQE_pion_minus_" + suffix, pionEQEcontainer.EQE_pion_minus, 1., 1500., 0., 1500.);
  JSFillHist(suffix, "EQE_delta_pion_plus_" + suffix, pionEQEcontainer.EQE_delta_pion_plus, 1., 10000., -5000., 5000.);
  JSFillHist(suffix, "EQE_delta_pion_minus_" + suffix, pionEQEcontainer.EQE_delta_pion_minus, 1., 1500., 0., 1500.);
  JSFillHist(suffix, "EQE_pion_2D_" + suffix, pionEQEcontainer.EQE_pion_plus, pionEQEcontainer.EQE_pion_minus, 1., 1000, -5000., 5000., 1000, -5000., 5000.);
  JSFillHist(suffix, "mX_QE_pion_" + suffix, pionEQEcontainer.mX_QE_pion, 1., 2000., 0., 2000.);
  JSFillHist(suffix, "EQE_pion_massless_vs_angle_" + suffix, pionEQEcontainer.EQE_pion_massless, pionEQEcontainer.piplus.Theta(), 1., 1500., 0., 1500., 1000., 0., 4.);

  JSFillHist(suffix, "E_loss_pion_" + suffix, pionEQEcontainer.E_loss, 1., 1000., 0., 1000.);
  JSFillHist(suffix, "Outgoing_pion_P_vs_angle_" + suffix, pionEQEcontainer.piplus.P() * 1000., pionEQEcontainer.piplus.Theta(), 1., 1050., 0., 1050., 1000., 0., 4.);
  JSFillHist(suffix, "Eloss_pion_vs_angle_" + suffix, pionEQEcontainer.E_loss, pionEQEcontainer.piplus.Theta(), 1., 1000., 0., 1000., 1000., 0., 4.);

  JSFillHist(suffix, Form("EQE_pion_minus_%dn_", N_n) + suffix, pionEQEcontainer.EQE_pion_minus, 1., 1500., 0., 1500.);
}

void BrainStorm::FillProtonEQEPlots(TString suffix, protonEQEstrc protonEQEcontainer, int N_n){
  double KE_proton = protonEQEcontainer.proton.E() * 1000. - M_proton;
  JSFillHist(suffix, "EQE_proton_massless_" + suffix, protonEQEcontainer.EQE_proton_massless, 1., 1500., 0., 1500.);
  JSFillHist(suffix, "EQE_proton_plus_" + suffix, protonEQEcontainer.EQE_proton_plus, 1., 1500., 0., 1500.);
  JSFillHist(suffix, "EQE_proton_minus_" + suffix, protonEQEcontainer.EQE_proton_minus, 1., 1500., 0., 1500.);
  JSFillHist(suffix, "KE_proton_vs_EQE_proton_minus_" + suffix, KE_proton, protonEQEcontainer.EQE_proton_minus, 1., 150., 0., 1500., 150., 0., 1500.);
  JSFillHist(suffix, Form("EQE_proton_minus_%dn_", N_n) + suffix, protonEQEcontainer.EQE_proton_minus, 1., 1500., 0., 1500.);
}

void BrainStorm::executeEventWithVariables(int smearBit, TString smear_flag, const std::vector<Gen>& particles_all, const std::vector<Gen>& piplus_all, const std::vector<Gen>& protons_all, const std::vector<Gen>& pizeros_all, const std::vector<Gen>& bkg_particles, const std::vector<Gen>& nuclei){

  // == Apply smearing & P cut (piplus > 0.1 GeV, proton > 0.3 GeV, pizero > 0 GeV)
  std::vector<Gen> piplus = GetPiplus(smear->SmearOutParticles(piplus_all, smearBit), 0.1);
  std::vector<Gen> protons = GetProtons(smear->SmearOutParticles(protons_all, smearBit), 0.3);
  std::vector<Gen> pizeros = GetPizeros(smear->SmearOutParticles(pizeros_all, smearBit), 0.);
  
  //SR1(smearBit, smear_flag, particles_all, piplus, protons, pizeros, bkg_particles, nuclei);
}

BrainStorm::BrainStorm(){

}

BrainStorm::~BrainStorm(){

}


