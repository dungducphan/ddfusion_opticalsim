#include "G4VUserActionInitialization.hh"

#include "generator.hh"
#include "runaction.hh"

#include "TH1D.h"

class actionInit : public G4VUserActionInitialization {
  public:
    actionInit(TH1D*);
    virtual ~actionInit();

    virtual void Build() const;
    virtual void BuildForMaster() const;

    TH1D* fNeutronSpectrum;
};
