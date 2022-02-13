#include "detConstruction.hh"
#include "sensitiveDet.hh"

detConstruction::detConstruction() : G4VUserDetectorConstruction(), mOpticalDiagnosticsFlag(false) {}

detConstruction::~detConstruction() {}

G4VPhysicalVolume* detConstruction::Construct() {
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  G4bool checkOverlaps = true;

  // World
  G4double world_sizeXY = 0.3*m;
  G4double world_sizeZ  = 5*m;
  G4Material* worldMat = nist->FindOrBuildMaterial("G4_Galactic");

  G4Box* solidWorld = new G4Box("solidWorld", 0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);
  G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
  G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World_PV", 0, false, 0, checkOverlaps);        

  // Detector Geometry Parameters
  G4double EJ200HalfThickness = 1;
  G4double XP2020HalfThickness = 0.1;
  G4double EJ200PosZ = 4.5;
  G4double XP2020PosZNoOffset = (EJ200PosZ / 2.) * m + EJ200HalfThickness * cm + XP2020HalfThickness * mm;

  // EJ-200
  G4Element* elH  = nist->FindOrBuildElement("H");
  G4Element* elC  = nist->FindOrBuildElement("C");
  G4Material* ej200Mat = new G4Material("EJ200Mat", 1.0221 * g/cm3, 2);
  ej200Mat->AddElement(elH, 0.084838648);
  ej200Mat->AddElement(elC, 0.915161352);
  ej200Mat->SetMaterialPropertiesTable(this->GetScintillatorBulkProps());

  G4Tubs* solidEJ200 = new G4Tubs("solidEJ200", 0, 2.51*cm, EJ200HalfThickness * cm, 0, 2*TMath::Pi());
  G4LogicalVolume* logicEJ200 = new G4LogicalVolume(solidEJ200, ej200Mat, "logicEJ200");
  G4VPhysicalVolume* physEJ200 = new G4PVPlacement(0, G4ThreeVector(0,0, (EJ200PosZ / 2.) * m), logicEJ200, "physEJ200", logicWorld, false, 0, checkOverlaps);

  // Diagnostics Homogneous SD
  // TODO: Diagnostic is currently giving some weird results. Investigate!
  if (mOpticalDiagnosticsFlag) {
    G4Sphere *solidDHSD = new G4Sphere("solidDHSD", 0, 0.5 * cm, 0, TMath::Pi() * 2, 0, TMath::Pi());
    G4LogicalVolume *logicDHSD = new G4LogicalVolume(solidDHSD, ej200Mat, "logicDHSD");
    G4VPhysicalVolume *physDHSD = new G4PVPlacement(0, G4ThreeVector(0, 1.5*cm, 0), logicDHSD, "physDHSD", logicEJ200, false, 0, checkOverlaps);
  }

  // XP2020 
  G4Element* elK = nist->FindOrBuildElement("K");
  G4Element* elCs = nist->FindOrBuildElement("Cs");
  G4Element* elSb = nist->FindOrBuildElement("Sb");
  G4Material* biAlkaliMat = new G4Material("Bi-Alkali", 3.46E6 * g/m3, 3);
  biAlkaliMat->AddElement(elK,  2);
  biAlkaliMat->AddElement(elCs, 1);
  biAlkaliMat->AddElement(elSb, 1);
  
  G4Tubs* solidXP2020 = new G4Tubs("solidXP2020", 0, 2.51*cm, XP2020HalfThickness * mm, 0, 2*TMath::Pi());
  G4LogicalVolume* logicXP2020 = new G4LogicalVolume(solidXP2020, biAlkaliMat, "logicXP2020");
  G4PVPlacement* physXP2020 = new G4PVPlacement(0, G4ThreeVector(0,0, XP2020PosZNoOffset), logicXP2020, "physXP2020", logicWorld, false, 0, checkOverlaps);

/*
  G4double refractiveIndex_Air[4] = {1.0, 1.0, 1.0, 1.0};
  G4double reflectivity_Air_EJ200[4] = {1.0, 1.0, 1.0, 1.0};
  G4MaterialPropertiesTable* MPT_Air = new G4MaterialPropertiesTable();
  MPT_Air->AddProperty("RINDEX", photonEnergy, refractiveIndex_Air, 4)->GetSpline();
  worldMat->SetMaterialPropertiesTable(MPT_Air);

    // Surface Properties of EJ200 and Air
    // Todo: need to make the reflection inside the scintillator container more realistic
    G4MaterialPropertiesTable* MPT_opSurf_Air_EJ200 = new G4MaterialPropertiesTable();
    MPT_opSurf_Air_EJ200->AddProperty("REFLECTIVITY", photonEnergy, reflectivity_Air_EJ200, 4)->GetSpline();
    G4OpticalSurface* opSurf_Air_EJ200 = new G4OpticalSurface("Air_EJ200");
    opSurf_Air_EJ200->SetType(dielectric_metal);
    opSurf_Air_EJ200->SetFinish(polishedtioair);
    opSurf_Air_EJ200->SetModel(unified);
    opSurf_Air_EJ200->SetMaterialPropertiesTable(MPT_opSurf_Air_EJ200);

    GetPMTQuantumQEFromFile("XP2020_QE.csv");
    // Todo: check the validity of these properties for the bi-alkali material of XP-2020 PMT
    std::vector<G4double> ephoton           = { 7.0 * eV, 7.14 * eV };
    std::vector<G4double> photocath_ReR     = { 1.92, 1.92 };
    std::vector<G4double> photocath_ImR     = { 1.69, 1.69 };
    G4MaterialPropertiesTable *MPT_XP2020 = new G4MaterialPropertiesTable();
    MPT_XP2020->AddProperty("EFFICIENCY", fPhotonEnergy_XP2020, fQE_XP2020,fPhotonEnergy_XP2020.size())->GetSpline();
    MPT_XP2020->AddProperty("REALRINDEX", ephoton, photocath_ReR);
    MPT_XP2020->AddProperty("IMAGINARYRINDEX", ephoton, photocath_ImR);
    
    // Surface Properties of EJ200 and PMT
    G4OpticalSurface* opSurf_EJ200_XP2020 = new G4OpticalSurface("EJ200_XP2020");
    opSurf_EJ200_XP2020->SetType(dielectric_dielectric);
    opSurf_EJ200_XP2020->SetFinish(polished);
    opSurf_EJ200_XP2020->SetModel(unified);
    opSurf_EJ200_XP2020->SetMaterialPropertiesTable(MPT_XP2020);
    
    // Surface Properties of EJ200 and PMT
    G4OpticalSurface* opSurf_Air_XP2020 = new G4OpticalSurface("EJ200_XP2020");
    opSurf_Air_XP2020->SetType(dielectric_dielectric);
    opSurf_Air_XP2020->SetFinish(polished);
    opSurf_Air_XP2020->SetModel(unified);
    opSurf_Air_XP2020->SetMaterialPropertiesTable(MPT_XP2020);

    // Surface properties applied

    // new G4LogicalBorderSurface("Air_EJ200", physEJ200, physWorld, opSurf_Air_EJ200);
    new G4LogicalBorderSurface("EJ200_XP2020", physEJ200, physXP2020, opSurf_EJ200_XP2020);
    // new G4LogicalSkinSurface("logicalSurf_XP2020", logicXP2020, opSurf_EJ200_XP2020);
    new G4LogicalSkinSurface("logicalSurf_EJ200", logicEJ200, opSurf_Air_EJ200);
*/

  return physWorld;
}

std::tuple<std::vector<G4double>, std::vector<G4double>, std::vector<G4double>> detConstruction::GetScintillatorOpticalProps(std::string spectrumFile) {
  std::vector<G4double> PhotonEnergy;
  std::vector<G4double> RelScintillatingAmplitude;
  std::vector<G4double> RefractionIndex;

  auto df_ej200_emission_Spectrum = ROOT::RDF::MakeCsvDataFrame(spectrumFile.c_str());
  auto gr = df_ej200_emission_Spectrum.Graph("Wavelength_nm", "RelYieldAmp");
  G4double maxAmp = -1.;
  for (int i = 0; i < gr->GetN(); ++i) {
    G4double ene = 1239.84193 * eV / gr->GetPointX(i);
    G4double relScintAmp = gr->GetPointY(i) * 1239.84193 / TMath::Power(gr->GetPointX(i), 2.);
    maxAmp = maxAmp < relScintAmp ? relScintAmp : maxAmp;
    PhotonEnergy.push_back(ene);
    RelScintillatingAmplitude.push_back(relScintAmp);
    RefractionIndex.push_back(1.58);
  }
  for (auto& amp : RelScintillatingAmplitude) amp /= maxAmp;

  return std::make_tuple(PhotonEnergy, RelScintillatingAmplitude, RefractionIndex);
}

/*
std::pair<std::vector<G4double>, std::vector<G4double>> detConstruction::GetPMTQuantumEfficiencyFromFile(std::string qeFile) {
  std::vector<G4double> PhotonEnergy;
  std::vector<G4double> QuantumEfficiency;

  auto df = ROOT::RDF::MakeCsvDataFrame(qeFile.c_str());
  auto gr = df.Graph("Wavelength_nm", "QE");
  for (int i = 0; i < gr->GetN(); ++i) {
    PhotonEnergy.push_back(1239.84193 / gr->GetPointX(i));
    QuantumEfficiency.push_back(gr->GetPointY(i));
  }

  return std::make_pair(PhotonEnergy, QuantumEfficiency);
}
*/
G4MaterialPropertiesTable* detConstruction::GetScintillatorBulkProps() {
  auto ScintOpSpec = this->GetScintillatorOpticalProps("EJ200-EmissionSpec.csv");
  auto MPT_EJ200 = new G4MaterialPropertiesTable();
  MPT_EJ200->AddProperty("RINDEX", std::get<0>(ScintOpSpec), std::get<2>(ScintOpSpec), false, true);
  MPT_EJ200->AddProperty("SCINTILLATIONCOMPONENT1", std::get<0>(ScintOpSpec), std::get<1>(ScintOpSpec), false, true);
  MPT_EJ200->AddConstProperty("SCINTILLATIONYIELD", 10. / MeV); // TODO: 10000
  MPT_EJ200->AddConstProperty("RESOLUTIONSCALE", 1.0);
  MPT_EJ200->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.1 * ns);
  MPT_EJ200->AddConstProperty("SCINTILLATIONRISETIME1", 0.9 * ns);
  MPT_EJ200->AddConstProperty("SCINTILLATIONYIELD1", 1.0);

  return MPT_EJ200;
}

void detConstruction::ConstructSDandField() {
  if (mOpticalDiagnosticsFlag) {
    G4String name = "opticalSD";
    opticalSD *opticalDiagnosticSD = new opticalSD(name);
    G4SDManager::GetSDMpointer()->AddNewDetector(opticalDiagnosticSD);
    SetSensitiveDetector("logicDHSD", opticalDiagnosticSD);
  }
}

void detConstruction::SetOpticalDiagnostic(bool flag) {
  mOpticalDiagnosticsFlag = flag;
}
