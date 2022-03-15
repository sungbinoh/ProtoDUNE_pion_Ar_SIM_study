#include <iostream>

R__LOAD_LIBRARY(libPhysics.so)
R__LOAD_LIBRARY(libTree.so)
R__LOAD_LIBRARY(libHist.so)
R__LOAD_LIBRARY(./lib/libDataFormats.so)
R__LOAD_LIBRARY(./lib/libAnalyzerTools.so)
R__LOAD_LIBRARY(./lib/libAnalyzers.so)

void run_test(){

  cout << "Test Classes" << endl;
  cout << "Test AnalzerTools" << endl;
  SmearParticles a;
  a.TestClass();

  cout << "Call Analyzer Class" << endl;
  PionAnalyzer m;
  //SetTreeName();
  cout << "Setting evnet variables" << endl;
  m.MaxEvent = 1000;

  m.Simulator = "GEANT4";
  

  //Init();
  cout << "Running" << endl;
  m.initializeAnalyzer();
}
