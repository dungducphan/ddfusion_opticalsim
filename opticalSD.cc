#include "opticalSD.hh"

opticalSD::opticalSD(const G4String &name) : G4VSensitiveDetector(name) {}

opticalSD::~opticalSD() {}

G4bool opticalSD::ProcessHits(G4Step * aStep, G4TouchableHistory *) {
  G4Track *track = aStep->GetTrack();
  if (track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) return false;

  G4StepPoint *prePoint = aStep->GetPreStepPoint();

  G4double energy = prePoint->GetTotalEnergy();
  G4double x_hit = prePoint->GetPosition().getX();
  G4double y_hit = prePoint->GetPosition().getY();
  G4double z_hit = prePoint->GetPosition().getZ();
  G4double t_hit = prePoint->GetGlobalTime();

  G4AnalysisManager* man = G4AnalysisManager::Instance();
  man->FillNtupleDColumn(0, 1239.84193 / (energy / eV));
  man->FillNtupleDColumn(1, x_hit / m);
  man->FillNtupleDColumn(2, y_hit / m);
  man->FillNtupleDColumn(3, z_hit / m);
  man->FillNtupleDColumn(4, t_hit / ns);
  man->AddNtupleRow(0);

  track->SetTrackStatus(fStopAndKill);

  return true;
}
