/*
  ==============================================================================

    MainApplication.h
    Created: 02 May 2020 4:58:15pm
    Author:  Rochelle Tham

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class MainWindow;


class MainApplication : public JUCEApplication {
public:

   MainApplication();

//==============================================================================
   // JUCEApplication overrides

   const String getApplicationName() override;

   const String getApplicationVersion() override;

   bool moreThanOneInstanceAllowed() override;

   void initialise (const String& commandLine) override;

   /// Add your application's shutdown code here.
   void shutdown() override;

   void systemRequestedQuit() override;

   void anotherInstanceStarted (const String& commandLine) override;

   //==============================================================================
   // MainApplication members

   /// Returns our application instance.
   static MainApplication& getApp();


private:

   void closeAllAlertAndDialogWindows();
   std::unique_ptr<MainWindow> mainWindow;
};
