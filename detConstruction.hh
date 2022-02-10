#pragma once

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

#include "TMath.h"
#include "ROOT/RCsvDS.hxx"

class G4VPhysicalVolume;
class G4LogicalVolume;

class detConstruction : public G4VUserDetectorConstruction {
  public:
    detConstruction();
    virtual ~detConstruction();

    virtual G4VPhysicalVolume* Construct();
    // virtual void ConstructSDandField();

    // void FillScintillatorFromFile(std::string spectrumFile);
    // void GetPMTQEFromFile(std::string qeFile);
};
