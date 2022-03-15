#include <iostream>

R__LOAD_LIBRARY(libPhysics.so)
R__LOAD_LIBRARY(libTree.so)
R__LOAD_LIBRARY(libHist.so)
R__LOAD_LIBRARY(./lib/libDataFormats.so)
R__LOAD_LIBRARY(./lib/libAnalyzerTools.so)
R__LOAD_LIBRARY(./lib/libAnalyzers.so)

void run_test(){

  cout << "Test Classes" << endl;
  SmearParticles a;
  a.TestClass();

  cout << "Call Analyzer Class" << endl;
  PionAnalyzer m;
  //SetTreeName();
  cout << "Setting event variables" << endl;
  m.MaxEvent = 1000;
  m.MCSample = "Piplus_1GeV_Ar";
  m.Simulator = "GEANT4";
  m.SetTreeName();
  m.AddFile("/data6/Users/suoh/FNAL/pion_study/samples/GEANT4/XS_Pi+_G4_lAr_1GeV_10b_0.root"); // == GEANT4
  m.SetOutfilePath("hists.root");  
  m.Init();
  cout << "Running" << endl;
  m.initializeAnalyzer();
  m.initializeAnalyzerTools();
  m.SwitchToTempDir();
  m.Loop();

  m.WriteHist();
}
