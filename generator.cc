#include "generator.hh"

generator::generator() {
  fGeneralParticleSource = new G4GeneralParticleSource();

  G4SingleParticleSource *sps = fGeneralParticleSource->GetCurrentSource();
  G4ParticleDefinition *particle = G4ParticleTable::GetParticleTable()->FindParticle("neutron");

  sps->SetParticleDefinition(particle);
  sps->SetNumberOfParticles(10); // TODO: lower intensity for testing. Get 10000/shot in production.

  sps->GetPosDist()->SetPosDisType("Point"); // Point, Beam, Plane, Surface, Volume
  sps->GetPosDist()->SetCentreCoords(G4ThreeVector(0., 0., -2. * m));
  sps->GetPosDist()->ConfineSourceToVolume("NULL");

  sps->GetAngDist()->SetAngDistType( "iso"); // Isotropic, Cosine-law, Beam, User-defined
  sps->GetAngDist()->DefineAngRefAxes("angref1", G4ThreeVector(1, 0, 0));
  sps->GetAngDist()->DefineAngRefAxes("angref2", G4ThreeVector(1, 1, 0));

  sps->GetAngDist()->SetMinTheta((TMath::Pi() - 0.001) * rad); // TODO: Get wider beam in production.
  sps->GetAngDist()->SetMaxTheta(TMath::Pi() * rad);
  sps->GetAngDist()->SetMinPhi(0 * rad);
  sps->GetAngDist()->SetMaxPhi(TMath::Pi() * 2. * rad);

  sps->GetEneDist()->SetEnergyDisType("Exp"); // Mono, Lin, Pow, Exp, Gaus, Brem, BBody, Cdg (cosmic diffuse gamma), User, Arb, Epn (energy per nucleon)
  sps->GetEneDist()->SetEzero(4 * MeV); // I~Exp(-E/E0)
}

generator::~generator() { delete fGeneralParticleSource; }

void generator::GeneratePrimaries(G4Event *anEvent) {
  fGeneralParticleSource->GeneratePrimaryVertex(anEvent);
}
