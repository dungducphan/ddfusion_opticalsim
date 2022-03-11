#include "detConstruction.hh"
#include <fstream>

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
  // worldMat->SetMaterialPropertiesTable(this->GetWorldBulkProps());

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
  new G4LogicalSkinSurface("logicOpSkin_EJ200", logicEJ200, this->GetScintillatorSkinProps());

  // Diagnostics Homogneous SD
  if (mOpticalDiagnosticsFlag) {
    G4Sphere *solidDHSD = new G4Sphere("solidDHSD", 0, 0.5 * cm, 0, TMath::Pi() * 2, 0, TMath::Pi());
    G4LogicalVolume *logicDHSD = new G4LogicalVolume(solidDHSD, ej200Mat, "logicDHSD");
    new G4PVPlacement(0, G4ThreeVector(0, 1.5*cm, 0), logicDHSD, "physDHSD", logicEJ200, false, 0, checkOverlaps);
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
  new G4LogicalBorderSurface("logicOpSkin_XP2020", physEJ200, physXP2020, this->GetPhotoCathodeScintillatorBoundaryProps());

  G4Material* copper = nist->FindOrBuildMaterial("G4_Cu");
  G4Box* solidCuShield = new G4Box("solidCuShield", 10. * cm, 10. * cm, 1.5 * cm);
  G4LogicalVolume* logicCuShield = new G4LogicalVolume(solidCuShield, copper, "logicCuShield");
  new G4PVPlacement(0, G4ThreeVector(0, 0, (EJ200PosZ / 2. - 0.1) * m), logicCuShield, "physCuShield", logicWorld, false, 0, checkOverlaps);

  return physWorld;
}

std::tuple<std::vector<G4double>, std::vector<G4double>, std::vector<G4double>> detConstruction::GetScintillatorOpticalProps(std::string spectrumFile) {
  std::vector<G4double> PhotonEnergy;
  std::vector<G4double> RelScintillatingAmplitude;
  std::vector<G4double> RefractionIndex;

  std::ifstream indata(spectrumFile.c_str());

  G4double maxAmp = -1.;
  double tmp_wavelength;
  double tmp_relAmp;
  while (indata >> tmp_wavelength >> tmp_relAmp) {
    G4double ene = 1239.84193 * eV / tmp_wavelength;
    G4double relScintAmp = tmp_relAmp; // * 1239.84193 / TMath::Power(tmp_wavelength, 2.); // TODO: check if Jacobian is needed here.
    maxAmp = maxAmp < relScintAmp ? relScintAmp : maxAmp;
    PhotonEnergy.push_back(ene);
    RelScintillatingAmplitude.push_back(relScintAmp);
    RefractionIndex.push_back(1.58);
  }
  indata.close();

  for (auto& amp : RelScintillatingAmplitude) amp /= maxAmp;

  return std::make_tuple(PhotonEnergy, RelScintillatingAmplitude, RefractionIndex);
}

std::tuple<std::vector<G4double>, std::vector<G4double>, std::vector<G4double>, std::vector<G4double>> detConstruction::GetPMTQuantumEfficiencyFromFile(std::string qeFile) {
  std::vector<G4double> PhotonEnergy;
  std::vector<G4double> QuantumEfficiency;
  std::vector<G4double> PhotoCathodeReR;
  std::vector<G4double> PhotoCathodeImR;

  std::ifstream indata(qeFile.c_str());
  G4double tmp_wavelength;
  G4double tmp_qe;
  while (indata >> tmp_wavelength >> tmp_qe) {
    PhotonEnergy.push_back(1239.84193 * eV / tmp_wavelength);
    QuantumEfficiency.push_back(tmp_qe / 100.);
    PhotoCathodeReR.push_back(1.92);
    PhotoCathodeImR.push_back(1.69);
  }

  return std::make_tuple(PhotonEnergy, QuantumEfficiency, PhotoCathodeReR, PhotoCathodeImR);
}

G4MaterialPropertiesTable* detConstruction::GetScintillatorBulkProps() {
  auto ScintOpSpec = this->GetScintillatorOpticalProps("EJ200-EmissionSpec.csv");
  auto MPT_EJ200 = new G4MaterialPropertiesTable();

  int N = std::get<0>(ScintOpSpec).size();

  auto photonEnergy = new G4double[N];
  auto relAmp = new G4double[N];
  auto rIndex = new G4double[N];

  for (int i = 0; i < N; ++i) {
    photonEnergy[i] = std::get<0>(ScintOpSpec).at(N - 1 - i);
    rIndex[i] = std::get<2>(ScintOpSpec).at(N - 1 - i);
    relAmp[i] = std::get<1>(ScintOpSpec).at(N - 1 - i);
    G4cout << photonEnergy[i] / eV << " :: " << relAmp[i] << G4endl;
  }

  MPT_EJ200->AddProperty("RINDEX", photonEnergy, rIndex, N, false, true);
  MPT_EJ200->AddProperty("SCINTILLATIONCOMPONENT1", photonEnergy, relAmp, N, false, true);
  MPT_EJ200->AddConstProperty("SCINTILLATIONYIELD", 10000. / MeV);
  MPT_EJ200->AddConstProperty("RESOLUTIONSCALE", 1.0);
  MPT_EJ200->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.1 * ns);
  MPT_EJ200->AddConstProperty("SCINTILLATIONRISETIME1", 0.9 * ns);
  MPT_EJ200->AddConstProperty("SCINTILLATIONYIELD1", 1.0);

  return MPT_EJ200;
}

G4MaterialPropertiesTable* detConstruction::GetWorldBulkProps() {
  G4double photonEnergy[4] = {1E-2*eV, 0.1 * eV, 1 * eV, 10 * eV};
  G4double refractiveIndex_Air[4] = {1.0, 1.0, 1.0, 1.0};
  G4MaterialPropertiesTable* MPT_Air = new G4MaterialPropertiesTable();
  MPT_Air->AddProperty("RINDEX", photonEnergy, refractiveIndex_Air, 4)->GetSpline();

  return MPT_Air;
}

void detConstruction::ConstructSDandField() {
  if (mOpticalDiagnosticsFlag) {
    G4String name = "opticalSD";
    opticalSD *opticalDiagnosticSD = new opticalSD(name);
    G4SDManager::GetSDMpointer()->AddNewDetector(opticalDiagnosticSD);
    SetSensitiveDetector("logicDHSD", opticalDiagnosticSD);
  }

  G4String name = "pmtSD";
  opticalSD *pmtSD = new opticalSD(name);
  G4SDManager::GetSDMpointer()->AddNewDetector(pmtSD);
  SetSensitiveDetector("logicXP2020", pmtSD);
}

void detConstruction::SetOpticalDiagnostic(bool flag) {
  mOpticalDiagnosticsFlag = flag;
}

G4OpticalSurface* detConstruction::GetScintillatorSkinProps() {
  G4double photonEnergy[2] = {0.1 * eV, 100 * eV};
  G4double reflectivity[2] = {1, 1};
  G4MaterialPropertiesTable* MPT_opSurf_EJ200 = new G4MaterialPropertiesTable();
  MPT_opSurf_EJ200->AddProperty("REFLECTIVITY", photonEnergy, reflectivity, 2)->GetSpline();
  G4OpticalSurface* opSurf_EJ200 = new G4OpticalSurface("OpSkin_EJ200");
  opSurf_EJ200->SetType(dielectric_metal);
  opSurf_EJ200->SetFinish(polished);
  opSurf_EJ200->SetModel(glisur);
  opSurf_EJ200->SetMaterialPropertiesTable(MPT_opSurf_EJ200);

  return opSurf_EJ200;
}

G4OpticalSurface *detConstruction::GetPhotoCathodeScintillatorBoundaryProps() {
  G4OpticalSurface* opSurf_XP2020 = new G4OpticalSurface("OpSkin_XP2020");
  opSurf_XP2020->SetType(dielectric_metal);
  opSurf_XP2020->SetFinish(polished);
  opSurf_XP2020->SetModel(glisur);

  auto opSpec = this->GetPMTQuantumEfficiencyFromFile("XP2020_QE.csv");
  G4MaterialPropertiesTable *MPT_XP2020 = new G4MaterialPropertiesTable();
  MPT_XP2020->AddProperty("EFFICIENCY", std::get<0>(opSpec), std::get<1>(opSpec));
  MPT_XP2020->AddProperty("REALRINDEX", std::get<0>(opSpec), std::get<2>(opSpec));
  MPT_XP2020->AddProperty("IMAGINARYRINDEX", std::get<0>(opSpec), std::get<3>(opSpec));
  opSurf_XP2020->SetMaterialPropertiesTable(MPT_XP2020);

  return opSurf_XP2020;
}

