#include "G4VUserActionInitialization.hh"

class actionInit : public G4VUserActionInitialization {
  public:
    actionInit();
    virtual ~actionInit();

    virtual void Build() const;
};
