#include "opticalSD.hh"

opticalSD::opticalSD(const G4String &name) : G4VSensitiveDetector(name) {}

opticalSD::~opticalSD() {}

G4bool opticalSD::ProcessHits(G4Step * aStep, G4TouchableHistory *) {
  G4Track *track = aStep->GetTrack();
  if (track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) return false;

  G4StepPoint *prePoint = aStep->GetPreStepPoint();
  G4double energy = prePoint->GetTotalEnergy();
  G4cout << "Scintillator energy: " << energy/eV << " eV." << G4endl;
  track->SetTrackStatus(fStopAndKill);

  return true;
}
