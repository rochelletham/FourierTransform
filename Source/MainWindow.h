#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class MainWindow  : public DocumentWindow
{
public:
   MainWindow (String name);

   ~MainWindow();

   //==============================================================================
   // DocumentWindow overrides

   void closeButtonPressed() override;

private:
   JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
};
