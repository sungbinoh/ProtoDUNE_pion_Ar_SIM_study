#include "Event.h"

ClassImp(Event)

Event::Event(){
  j_Simulator = "";
}

Event::~Event(){

}

void Event::SetSimulator(TString sim){
  j_Simulator = sim;
}
