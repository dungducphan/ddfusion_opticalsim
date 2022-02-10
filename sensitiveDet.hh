#pragma once

#include "G4VSensitiveDetector.hh"
#include <vector>
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"

class G4DataVector;
class G4Step;

class sensitiveDet : public G4VSensitiveDetector {
public:
    sensitiveDet(const G4String& name);
    virtual ~sensitiveDet();

    virtual G4bool  ProcessHits(G4Step*, G4TouchableHistory*);
};
