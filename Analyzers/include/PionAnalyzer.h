#ifndef PionAnalyzer_h
#define PionAnalyzer_h

#include "AnalyzerCore.h"

class PionAnalyzer : public AnalyzerCore {

public:

  void initializeAnalyzer();
  void executeEvent();
  void Init();

  PionAnalyzer();
  ~PionAnalyzer();

};

#endif
