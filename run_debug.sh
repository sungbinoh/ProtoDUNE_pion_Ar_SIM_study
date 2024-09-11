root -l -b<<EOF
#include "./Analyzers/BrainStorm.h"
SmearParticles a
a.TestClass()
//cout << "aaaaa" << endl
BrainStorm m;
m.MaxEvent = 2000000
//m.MaxEvent = 2000
//m.MaxEvent = -1
m.LogEvery = 1000
m.MCSample = "Piplus_1GeV_Ar"
m.Simulator = "GEANT4";
//m.Simulator = "FLUKA"
m.SetTreeName()
//m.AddFile("/data6/Users/suoh/FNAL/pion_study/samples/GEANT4/XS_Pi+_G4_lAr_1GeV_10b_0.root") // == GEANT4
//m.AddFile("/data6/Users/suoh/FNAL/pion_study/samples/FLUKA/18files/pion1GeVa001_Out.root") // == FLUKA 
m.AddFile("/Users/sungbino/OneDrive/OneDrive/ProtoDUNE-SP/SIM_study/ProtoDUNE_pion_Ar_SIM_study/input/XS_Pi+_G4_lAr_1GeV_10b_0.root") // == GEANT4
//m.AddFile("/Users/sungbino/OneDrive/OneDrive/ProtoDUNE-SP/SIM_study/ProtoDUNE_pion_Ar_SIM_study/input/18files/pion1GeV_Out_merged.root") // FLUKA
//m.AddFile("/Users/sungbino/OneDrive/OneDrive/ProtoDUNE-SP/SIM_study/ProtoDUNE_pion_Ar_SIM_study/input/pion_noeva_nofsi_1GeV001_Out.root") // == FLUKA no FSI
m.SetOutfilePath("hists_GEANT4.root");
//m.SetOutfilePath("hists_FLUKA_fsi.root");
//m.SetOutfilePath("hists_FLUKA_no_fsi.root");
m.Init();
cout << "Running" << endl;
m.initializeAnalyzer();
m.initializeAnalyzerTools();
m.SwitchToTempDir();
m.Loop();

m.WriteHist();

EOF
