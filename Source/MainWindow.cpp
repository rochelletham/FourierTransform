#include "MainApplication.h"
#include "MainWindow.h"
#include "MainComponent.h"

MainWindow::MainWindow(String name)
: DocumentWindow(name, Colours::white, DocumentWindow::allButtons) {
   ///The window title bar uses the OS'es window style.
   setUsingNativeTitleBar(true);

   auto maincomp = std::make_unique<MainComponent>();
   setContentOwned(maincomp.release(), false);
   setResizable(true, true);
   setResizeLimits(500, 250, 1000, 500);
   centreWithSize(getWidth(), getHeight());

   setFullScreen (true);

   // make the window visible
   setVisible(true);


}
MainWindow::~MainWindow() {

}

//==============================================================================
// DocumentWindow overrides

void MainWindow::closeButtonPressed() {
   // when the main window is closed signal the app to exit
   JUCEApplication::getInstance()->systemRequestedQuit();
}

