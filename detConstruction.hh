#pragma once

#include <utility>
#include <tuple>
#include <fstream>

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4SDManager.hh"

#include "TMath.h"

#include "opticalSD.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class detConstruction : public G4VUserDetectorConstruction {
  public:
    detConstruction();
    virtual ~detConstruction();

    void SetOpticalDiagnostic(bool flag);

    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

  private:
    bool mOpticalDiagnosticsFlag;

    std::tuple<std::vector<G4double>, std::vector<G4double>, std::vector<G4double>> GetScintillatorOpticalProps(std::string spectrumFile);
    std::pair<std::vector<G4double>, std::vector<G4double>> GetPMTQuantumEfficiencyFromFile(std::string qeFile);

    G4MaterialPropertiesTable* GetScintillatorBulkProps();
    G4MaterialPropertiesTable* GetWorldBulkProps();
    // G4MaterialPropertiesTable* GetScintillatorSkinProps();
    // G4MaterialPropertiesTable* GetPhotoCathodeBulkProps();
    // G4MaterialPropertiesTable* GetPhotoCathodeSkinProps();
    // G4MaterialPropertiesTable* GetPhotoCathodeScintillatorBoundaryProps();
};
