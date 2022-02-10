
#include "detConstruction.hh"
#include "actionInit.hh"

#include "QGSP_BERT_HP.hh"
#include "G4OpticalPhysics.hh"
#include "G4RunManagerFactory.hh"
#include "G4SteppingVerbose.hh"
#include "G4UImanager.hh"
#include "QBBC.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

int main(int argc,char** argv) {
  G4UIExecutive* ui = nullptr;
  if ( argc == 1 ) { ui = new G4UIExecutive(argc, argv); }

  G4int precision = 4;
  G4SteppingVerbose::UseBestUnit(precision);

  auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default, 1);

  auto detModel = new detConstruction();
  detModel->SetOpticalDiagnostic(true);
  runManager->SetUserInitialization(detModel);
  
  G4VModularPhysicsList* physicsList = new QGSP_BERT_HP();
  physicsList->RegisterPhysics(new G4OpticalPhysics());
  runManager->SetUserInitialization(physicsList);

  runManager->SetUserInitialization(new actionInit());

  runManager->Initialize();

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
