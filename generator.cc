#include "generator.hh"

generator::generator(TH1D* hEne) {
  fNeutronSpectrum = hEne;
  fGeneralParticleSource = new G4GeneralParticleSource();

  G4SingleParticleSource *sps = fGeneralParticleSource->GetCurrentSource();
  G4ParticleDefinition *particle = G4ParticleTable::GetParticleTable()->FindParticle("neutron");

  sps->SetParticleDefinition(particle);
  sps->SetNumberOfParticles(10000);

  sps->GetPosDist()->SetPosDisType("Point"); // Point, Beam, Plane, Surface, Volume
  sps->GetPosDist()->SetCentreCoords(G4ThreeVector(0., 0., - (4.5 / 2.) * m));
  sps->GetPosDist()->ConfineSourceToVolume("NULL");

  sps->GetAngDist()->SetAngDistType( "iso"); // Isotropic, Cosine-law, Beam, User-defined
  sps->GetAngDist()->DefineAngRefAxes("angref1", G4ThreeVector(1, 0, 0));
  sps->GetAngDist()->DefineAngRefAxes("angref2", G4ThreeVector(1, 1, 0));

  sps->GetAngDist()->SetMinTheta((TMath::Pi() - 0.008) * rad);
  sps->GetAngDist()->SetMaxTheta(TMath::Pi() * rad);
  sps->GetAngDist()->SetMinPhi(0 * rad);
  sps->GetAngDist()->SetMaxPhi(TMath::Pi() * 2. * rad);

  sps->GetEneDist()->SetEnergyDisType("User"); // Mono, Lin, Pow, Exp, Gaus, Brem, BBody, Cdg (cosmic diffuse gamma), User, Arb, Epn (energy per nucleon)
  GetEnergySpectrum(sps);
}

generator::~generator() {
  delete fGeneralParticleSource;
}

void generator::GeneratePrimaries(G4Event *anEvent) {
  fGeneralParticleSource->GeneratePrimaryVertex(anEvent);
}

void generator::GetEnergySpectrum(G4SingleParticleSource* sps) {
  G4double maxBinContent = fNeutronSpectrum->GetMaximum();
  for (int i = 1; i < fNeutronSpectrum->GetSize() - 1; ++i) {
    G4double ene = fNeutronSpectrum->GetBinCenter(i) * MeV;
    G4double val = fNeutronSpectrum->GetBinContent(i) * 1023. / maxBinContent;
    G4ThreeVector binData(ene, val, 0);
    sps->GetEneDist()->UserEnergyHisto(binData);
  }
}
