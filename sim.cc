
#include "detConstruction.hh"
#include "actionInit.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "QGSP_BERT_HP.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4OpticalPhysics.hh"
#include "G4SteppingVerbose.hh"
#include "G4VModularPhysicsList.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

int main(int argc,char** argv) {
  G4UIExecutive* ui = nullptr;
  if ( argc == 1 ) { ui = new G4UIExecutive(argc, argv); }

  G4int precision = 4;
  G4SteppingVerbose::UseBestUnit(precision);

#ifdef G4MULTITHREADED
  G4MTRunManager* runManager = new G4MTRunManager();
#else
  G4RunManager* runManager = new G4RunManager();
#endif

  auto detModel = new detConstruction();
  // detModel->SetOpticalDiagnostic(true);
  runManager->SetUserInitialization(detModel);
  
  G4VModularPhysicsList* physicsList = new QGSP_BERT_HP();
  physicsList->ReplacePhysics(new G4EmStandardPhysics_option4());
  G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
  auto opticalParams= G4OpticalParameters::Instance();
  opticalParams->SetWLSTimeProfile("delta");
  opticalParams->SetScintTrackSecondariesFirst(true);
  opticalParams->SetCerenkovMaxPhotonsPerStep(100);
  opticalParams->SetCerenkovMaxBetaChange(10.0);
  opticalParams->SetCerenkovTrackSecondariesFirst(true);
  physicsList->RegisterPhysics(opticalPhysics);

  runManager->SetUserInitialization(physicsList);

  runManager->SetUserInitialization(new actionInit());

  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();

  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  
  if ( ! ui ) {
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  } else {
    UImanager->ApplyCommand("/control/execute vis.mac");
    ui->SessionStart();
    delete ui;
  }

  delete visManager;
  delete runManager;
}
