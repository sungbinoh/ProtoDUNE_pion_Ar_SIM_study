#ifndef SmearParticles_h
#define SmearParticles_h

#include <map>
#include <vector>
#include <fstream>
#include <sstream>

#include "TRandom3.h"

#include "Gen.h"

class SmearParticles{

public:

  SmearParticles();
  ~SmearParticles();
  
  Gen SmearOutParticle(const Gen& this_Gen, int smearBit);
  std::vector<Gen> SmearOutParticles(const std::vector<Gen>& Gens, int smearBit);
  Gen SmearInputBeam(const Gen& input);

};

#endif
