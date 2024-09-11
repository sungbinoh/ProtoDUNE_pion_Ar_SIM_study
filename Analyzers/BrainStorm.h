#ifndef BrainStorm_h
#define BrainStorm_h

#include "AnalyzerCore.h"

struct pionEQEstrc{
  double E_loss;
  double EQE_pion_massless;
  double EQE_pion_plus;
  double EQE_pion_minus;
  double EQE_delta_pion_plus;
  double EQE_delta_pion_minus;
  double mX_QE_pion;
  Gen piplus;
};

struct protonEQEstrc{
  double EQE_proton_massless;
  double EQE_proton_plus;
  double EQE_proton_minus;
  Gen proton;
};

class BrainStorm : public AnalyzerCore {

public:

  void initializeAnalyzer();
  void executeEvent();
  void QE_Study(const std::vector<Gen>& particles_all, const std::vector<Gen>& piplus_all, const std::vector<Gen>& protons_all, const std::vector<Gen>& pizeros_all, const std::vector<Gen>& bkg_particles, const std::vector<Gen>& nuclei);
  void FillPionEQEPlots(TString suffix, pionEQEstrc pionEQEcontainer, int N_n);
  void FillProtonEQEPlots(TString suffix, protonEQEstrc protonEQEcontainer, int N_n);
  //void executeEventWithVariables(int smearBit, TString smear_flag);
  void executeEventWithVariables(int smearBit, TString smear_flag, const std::vector<Gen>& particles_all, const std::vector<Gen>& piplus_all, const std::vector<Gen>& protons_all, const std::vector<Gen>& pizeros_all, const std::vector<Gen>& bkg_particles, const std::vector<Gen>& nuclei);

  BrainStorm();
  ~BrainStorm();

};

#endif
