/*
  ==============================================================================

    MainApplication.cpp
    Created: 02 May 2020 4:58:15pm
    Author:  Rochelle Tham

  ==============================================================================
*/

#include "MainApplication.h"
#include "MainWindow.h"
#include "MainComponent.h"

//==============================================================================
// MainApplication members

MainApplication& MainApplication::getApp() {
   MainApplication* const app = dynamic_cast<MainApplication*>(JUCEApplication::getInstance());
   jassert(app != nullptr);
   return *app;
}


void MainApplication::closeAllAlertAndDialogWindows() {
   // delete any open alert or dialog windows.
   std::vector<std::unique_ptr<AlertWindow>> openAlerts;
   std::vector<std::unique_ptr<DialogWindow>> openDialog;

   Desktop &dt = Desktop::getInstance();
   for (auto i = 0; i < dt.getNumComponents(); ++i) {
      if (AlertWindow* aw = dynamic_cast<AlertWindow*>(dt.getComponent(i))) {
         openAlerts.push_back(std::unique_ptr<AlertWindow>(aw));
      } else if (DialogWindow* dw = dynamic_cast<DialogWindow*>(dt.getComponent(i))) {
         openDialog.push_back(std::unique_ptr<DialogWindow>(dw));
      }
   }
}

//==============================================================================
// JUCEApplication overrides

MainApplication::MainApplication() {
}

const String MainApplication::getApplicationName() {
   return ProjectInfo::projectName;
}

const String MainApplication::getApplicationVersion() {
   return ProjectInfo::versionString;
}

bool MainApplication::moreThanOneInstanceAllowed() {
   return false;
}

//Create the application window.
void MainApplication::initialise(const String& commandLine) {
   // Create the application window.
   mainWindow = std::make_unique<MainWindow>(getApplicationName());
//   auto errors = devic
}

void MainApplication::shutdown() {
   // Delete our main window
   mainWindow = nullptr;
}

void MainApplication::systemRequestedQuit() {
   closeAllAlertAndDialogWindows();
   quit();
}

void MainApplication::anotherInstanceStarted(const String& commandLine) {
}

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (MainApplication)
