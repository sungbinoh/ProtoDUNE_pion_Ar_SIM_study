#ifndef Gen_h
#define Gen_h

#include "Particle.h"

class Gen: public Particle {
public:

  Gen();
  ~Gen();

  //==== For empty Gen
  void SetIsEmpty(bool b);
  inline bool IsEmpty() const { return j_IsEmpty; }

  void SetPIDPosition(int pid, double x, double y, double z);
  inline int PID() const { return j_PID; }
  inline int interType() const { return j_interType; }
  inline double mass() const { return j_massPart; }
  inline double X() const { return j_X; }
  inline double Y() const { return j_Y; }
  inline double Z() const { return j_Z; }

  virtual void Print();

private:

  bool j_IsEmpty;
  int j_PID, j_interType;
  double j_massPart, j_X, j_Y, j_Z;

  ClassDef(Gen,1)
};

#endif
