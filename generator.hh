#pragma once

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4SystemOfUnits.hh"
#include "G4GeneralParticleSource.hh"
#include "G4SingleParticleSource.hh"
#include "G4IonTable.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ThreeVector.hh"
#include "G4Event.hh"
#include "G4AnalysisManager.hh"
#include "G4String.hh"

#include "TMath.h"
#include "TH1D.h"
#include "TFile.h"

class generator : public G4VUserPrimaryGeneratorAction {
public:
  generator();
  ~generator() override;

  void GeneratePrimaries(G4Event* anEvent) override;
  void GetEnergySpectrum(G4SingleParticleSource* sps);

private:
  G4GeneralParticleSource *fGeneralParticleSource = nullptr;
};
