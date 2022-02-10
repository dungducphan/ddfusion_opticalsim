#include "detConstruction.hh"
#include "sensitiveDet.hh"

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

detConstruction::detConstruction() : G4VUserDetectorConstruction() {}

detConstruction::~detConstruction() {}

G4VPhysicalVolume* detConstruction::Construct() {
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  G4bool checkOverlaps = true;

  // World
  G4double world_sizeXY = 0.3*m;
  G4double world_sizeZ  = 5*m;
  G4Material* worldMat = nist->FindOrBuildMaterial("G4_AIR");

  G4Box* solidWorld = new G4Box("solidWorld", 0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);
  G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
  G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World_PV", 0, false, 0, checkOverlaps);        

  // Geometry Parameters
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

  G4Tubs* solidEJ200 = new G4Tubs("solidEJ200", 0, 2.51*cm, EJ200HalfThickness * cm, 0, 2*TMath::Pi());
  G4LogicalVolume* logicEJ200 = new G4LogicalVolume(solidEJ200, ej200Mat, "logicEJ200");
  // G4VPhysicalVolume* physEJ200 =
  new G4PVPlacement(0, G4ThreeVector(0,0, (EJ200PosZ / 2.) * m), logicEJ200, "physEJ200", logicWorld, false, 0, checkOverlaps);

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
  // G4PVPlacement* physXP2020 =
  new G4PVPlacement(0, G4ThreeVector(0,0, XP2020PosZNoOffset), logicXP2020, "physXP2020", logicWorld, false, 0, checkOverlaps);

/*
  // FIXME: keep only for sanity check. Turn off in production mode.
  // G4MaterialPropertiesTable* MPT_Air = new G4MaterialPropertiesTable();
  // MPT_Air->AddProperty("RINDEX", photonEnergy, refractiveIndex_Air, 4)->GetSpline();
  // worldMat->SetMaterialPropertiesTable(MPT_Air);

    G4double photonEnergy[4] = {0.001*eV, 1*eV, 10*eV, 100*eV};
    G4double refractiveIndex_EJ200[4] = {1.58, 1.58, 1.58, 1.58};
    G4double refractiveIndex_Air[4] = {1.0, 1.0, 1.0, 1.0};
    G4double reflectivity_Air_EJ200[4] = {1.0, 1.0, 1.0, 1.0};

    FillScintillatorFromFile("EJ200-EmissionSpec.csv");
    G4MaterialPropertiesTable* MPT_EJ200 = new G4MaterialPropertiesTable();
    MPT_EJ200->AddProperty("RINDEX", photonEnergy, refractiveIndex_EJ200, 4)->GetSpline();
    MPT_EJ200->AddProperty("SCINTILLATIONCOMPONENT1", fPhotonEnergy_EJ200, fScintillatingAmplitude_EJ200)->GetSpline();
    MPT_EJ200->AddConstProperty("SCINTILLATIONYIELD", 10000. / MeV);
    MPT_EJ200->AddConstProperty("RESOLUTIONSCALE", 1.0);
    MPT_EJ200->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 2.1 * ns);
    MPT_EJ200->AddConstProperty("SCINTILLATIONRISETIME1", 0.9 * ns);
    MPT_EJ200->AddConstProperty("SCINTILLATIONYIELD1", 1.0);
    ej200->SetMaterialPropertiesTable(MPT_EJ200);

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


    //
    // EJ-200
    //



    // Surface properties applied

    // new G4LogicalBorderSurface("Air_EJ200", physEJ200, physWorld, opSurf_Air_EJ200);
    new G4LogicalBorderSurface("EJ200_XP2020", physEJ200, physXP2020, opSurf_EJ200_XP2020);
    // new G4LogicalSkinSurface("logicalSurf_XP2020", logicXP2020, opSurf_EJ200_XP2020);
    new G4LogicalSkinSurface("logicalSurf_EJ200", logicEJ200, opSurf_Air_EJ200);
*/

  return physWorld;
}
/*
void detConstruction::FillScintillatorFromFile(std::string spectrumFile) {
    auto df_ej200_emission_Spectrum = ROOT::RDF::MakeCsvDataFrame(spectrumFile.c_str());
    auto gr = df_ej200_emission_Spectrum.Graph("Wavelength_nm", "RelYieldAmp");
    for (int i = 0; i < gr->GetN(); ++i) {
        fPhotonEnergy_EJ200.push_back(1239.84193 * eV / gr->GetPointX(i));
        fScintillatingAmplitude_EJ200.push_back(gr->GetPointY(i));
    }
}

void detConstruction::GetPMTQuantumQEFromFile(std::string qeFile) {
    auto df = ROOT::RDF::MakeCsvDataFrame(qeFile.c_str());
    auto gr = df.Graph("Wavelength_nm", "QE");
    for (int i = 0; i < gr->GetN(); ++i) {
        fPhotonEnergy_XP2020.push_back(1239.84193 * eV / gr->GetPointX(i));
        fQE_XP2020.push_back(gr->GetPointY(i));
    }
}

void detConstruction::ConstructSDandField() {
    // Sensitive Detector
    G4String sdName = "B1/PMT";
    sensitiveDet* aPMTSD = new sensitiveDet(sdName);

    G4SDManager::GetSDMpointer()->AddNewDetector(aPMTSD);
    SetSensitiveDetector("XP2020_LV", aPMTSD);
}
*/
