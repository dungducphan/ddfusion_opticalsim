#include "actionInit.hh"

actionInit::actionInit(TH1D* hIn) : G4VUserActionInitialization() {
  fNeutronSpectrum = hIn;
}

actionInit::~actionInit() {
}

void actionInit::BuildForMaster() const {
  SetUserAction(new runAction());
}
void actionInit::Build() const {
  SetUserAction(new runAction());
  SetUserAction(new generator(fNeutronSpectrum));
}
