#include "SmearParticles.h"

TRandom3 gRan(1800);

Gen SmearParticles::SmearOutParticle(const Gen& this_Gen, int smearBit){
  // == smearBit (smearing on)
  //  0 : non
  //  1 : momentum only
  //  2 : θ only
  //  3 : momemtum + θ
  //  4 : φ only
  //  5 : momentum + φ
  //  6 : φ + θ
  //  7 : all
  //  8 : non ....
 
  Gen out = this_Gen;
 
  double momRes=0;
  int tmppdg = this_Gen.PID();
  // == momRes in fraction 0.0xx
  if(tmppdg==211){//pi+ 
    momRes = 0.02;
  }
  else if(tmppdg==2212){//proton 
    momRes = 0.02;
  }
  else if(tmppdg==111){//pi0 
    momRes = 0.05;
  }
  else{//no smearing
    return (this_Gen);
  }

  // == Angular Res : 5 degree
  const double thetaRes=5*TMath::DegToRad();
  const double phiRes  =5*TMath::DegToRad();

  // == Smearing
  const double momDelta   = (smearBit&1)? gRan.Gaus(0,momRes)   : 0;
  const double thetaDelta = (smearBit&2)? gRan.Gaus(0,thetaRes) : 0;
  const double phiDelta   = (smearBit&4)? gRan.Gaus(0,phiRes)   : 0;

  const double mom   = this_Gen.P()*(1+momDelta);
  const double theta = this_Gen.Theta()+thetaDelta;
  const double phi   = this_Gen.Phi()+phiDelta;
  const double mass = this_Gen.M();

  TVector3 v3;
  v3.SetMagThetaPhi(mom, theta, phi);

  out.SetVectM(v3, mass);
  
  return out;
}

std::vector<Gen> SmearParticles::SmearOutParticles(const std::vector<Gen>& Gens, int smearBit){
  std::vector<Gen> out;
  for(unsigned int i=0; i<Gens.size(); i++){
    Gen this_Gen = Gens.at(i);
    out.push_back(SmearOutParticle(this_Gen, smearBit));
  }

  return out;
}

Gen SmearParticles::SmearInputBeam(const Gen& input){

  // FIXME : Need to decide smearing on incoming particle beam
  Gen out = input;
  
  return out;
}
