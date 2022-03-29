#ifndef PionAnalyzer_h
#define PionAnalyzer_h

#include "AnalyzerCore.h"

class PionAnalyzer : public AnalyzerCore {

public:

  void initializeAnalyzer();
  void executeEvent();
  //void executeEventWithVariables(int smearBit, TString smear_flag);
  void executeEventWithVariables(int smearBit, TString smear_flag, std::vector<Gen> particles_all, std::vector<Gen> piplus_all, std::vector<Gen> protons_all, std::vector<Gen> pizeros_all, std::vector<Gen> bkg_particles, std::vector<Gen> nuclei);
  void SR1(int smearBit, TString smear_flag, std::vector<Gen> particles_all, std::vector<Gen> piplus, std::vector<Gen> protons, std::vector<Gen> pizeros, std::vector<Gen> bkg_particles, std::vector<Gen> nuclei);
  void SR2(int smearBit, TString smear_flag, std::vector<Gen> particles_all, std::vector<Gen> piplus, std::vector<Gen> protons, std::vector<Gen> pizeros, std::vector<Gen> bkg_particles, std::vector<Gen> nuclei);
  void SR3(int smearBit, TString smear_flag, std::vector<Gen> particles_all, std::vector<Gen> piplus, std::vector<Gen> protons, std::vector<Gen> pizeros, std::vector<Gen> bkg_particles, std::vector<Gen> nuclei);
  void SR4(int smearBit, TString smear_flag, std::vector<Gen> particles_all, std::vector<Gen> piplus, std::vector<Gen> protons, std::vector<Gen> pizeros, std::vector<Gen> bkg_particles, std::vector<Gen> nuclei);

  int GetNPID(std::vector<Gen> particles, int PID);

  void SR1_FillHist(TString region, TString variable_str, double variable, int atomic_number, int atomic_mass, double weight, int N_bin, double x_min, double x_max);
  void SR2_FillHist(TString region, TString variable_str, double variable, int atomic_number, int atomic_mass, double weight, int N_bin, double x_min, double x_max);

  //void Init();

  PionAnalyzer();
  ~PionAnalyzer();

};

#endif
