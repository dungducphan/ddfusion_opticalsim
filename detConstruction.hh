#pragma once

#include <utility>
#include <tuple>

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

    std::tuple<std::vector<double>, std::vector<double>, std::vector<double>> GetScintillatorOpticalProps(std::string spectrumFile);
    std::pair<std::vector<double>, std::vector<double>> GetPMTQuantumEfficiencyFromFile(std::string qeFile);
};
