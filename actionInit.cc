#include "actionInit.hh"

actionInit::actionInit() : G4VUserActionInitialization() {}

actionInit::~actionInit() {}

void actionInit::BuildForMaster() const {
  SetUserAction(new runAction());
}
void actionInit::Build() const {
  SetUserAction(new runAction());
  SetUserAction(new generator());
}
