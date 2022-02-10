#include "actionInit.hh"
#include "generator.hh"

actionInit::actionInit() : G4VUserActionInitialization() {}

actionInit::~actionInit() {}

void actionInit::Build() const {
  SetUserAction(new generator());
}
