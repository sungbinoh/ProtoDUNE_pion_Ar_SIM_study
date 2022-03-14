#include "Gen.h"

ClassImp(Gen)

Gen::Gen() : Particle() {
  j_IsEmpty = true;
  j_PID = 0;
  j_interType = 0;
  j_massPart = 0.;
  j_X = 0.;
  j_Y = 0.;
  j_Z = 0.;
}

Gen::~Gen(){
}

void Gen::SetIsEmpty(bool b){
  j_IsEmpty = b;
}

void Gen::SetPIDPosition(int pid, double x, double y, double z){
  j_PID = pid;
  j_X = x;
  j_Y = y;
  j_Z = z;
}

void Gen::Print(){

  cout << "(PID, mass, px, py, pz, E, x, y, z) = " << PID() << "\t" << mass() << "\t" << Px() << "\t" << Py() << "\t" << Pz() << "\t" << E() << "\t" << X() << "\t" << Y() << "\t" << Z() << endl;

}
